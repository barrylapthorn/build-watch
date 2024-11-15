/*
 * Copyright (c) 2024.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "BuildWatch.hpp"
#include "BuildWatch/Config.hpp"
#include "FileUtils.hpp"
#include "INotifyEvent.hpp"
#include <algorithm>
#include <fmt/std.h>
#include <fstream>
#include <mustache.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <sys/inotify.h>

namespace fs = std::filesystem;
namespace rg = std::ranges;

namespace btl {

void BuildWatch::useIgnoreFile(const Config& config)
{
    bool foundIgnore = false;
    for (const auto& ignoreFile : config.ignoreFiles) {
        if (const auto path = btl::findUp(fs::current_path(), fs::path(ignoreFile))) {
            ignore = Ignore(path->parent_path(), *path);
            spdlog::info("Using .*ignore file: {}", *path);
            foundIgnore = true;
            break;
        }
    }
    if (!foundIgnore) {
        spdlog::warn("No .*ignore file found, we're watching all directories");
    }
}

BuildWatch::BuildWatch(const std::filesystem::path& rootDirectory, const Config& config, bool dryRun)
    : rootPath(rootDirectory)
    , config(config)
    , dryRun(dryRun)
{
    spdlog::debug(fmt::format("Watching root directory: {}", rootDirectory.string()));
    nlohmann::json json;
    to_json(json, config);
    spdlog::info(fmt::format("Using: {}", nlohmann::json(json).dump(-1)));

    if (rootDirectory.empty()) {
        rootPath = fs::current_path();
    } else {
        rootPath = rootDirectory;
    }

    if (!fs::exists(rootPath)) {
        throw std::runtime_error(fmt::format("Supplied root-directory does not exist: {}", rootPath));
    }

    useIgnoreFile(config);

    watchDirectory(rootDirectory);
    spdlog::info("Watching...");
}

// ReSharper disable once CppDFAConstantFunctionResult
std::string BuildWatch::defaultConfig()
{
    using json = nlohmann::json;
    json j;
    btl::to_json(j, Config::defaultConfiguration());
    return j.dump(2);
}

void BuildWatch::watchDirectory(const std::filesystem::path& directory)
{
    using namespace std::literals;

    constexpr std::array ignores = { ".git"sv, ".hg"sv};

    if (!fs::is_directory(directory)) {
        spdlog::warn(fmt::format("Cannot watch, is not a directory: {}", directory.string()));
        return;
    }

    inotify.addWatch(directory, [this](const inotify_event& event, const INotifyWatch& watch) {
        this->onEvent(event, watch);
    });

    for (const auto& subdirectory : fs::recursive_directory_iterator(directory)) {
        if (!subdirectory.is_directory()) {
            continue;
        }

        const auto relpath = fs::relative(subdirectory, rootPath);
        if (relpath.begin() != relpath.end() && rg::contains(ignores,  relpath.begin()->string()))
        {
            continue;
        }

        if (ignore.ignore(subdirectory)) {
            spdlog::trace("Ignoring directory due to .*ignore file: {}", subdirectory.path());
            continue;
        }

        spdlog::debug("Watching subdir: {}", subdirectory.path());
        inotify.addWatch(subdirectory, [this](const inotify_event& event, const INotifyWatch& watch) {
            this->onEvent(event, watch);
        });
    }
}

void BuildWatch::watchOnce()
{
    inotify.watchOnce();
}

void BuildWatch::onCreateOrMoveFile(const inotify_event& event, const INotifyWatch& watch)
{
    const auto path = watch.getDirectory() / event.name;

    // Skip if we're not a file
    if (!fs::is_regular_file(path)) {
        spdlog::debug("Created, but not a regular file: {}", path.string());
        return;
    }

    // Is it a new template file?
    if (const auto& templateFile = config.findFilename(event.name)) {
        spdlog::debug("Template created: {}", path.string());
        writeTemplate(*templateFile, path);

        if (const auto& templatePath = findUp(watch.getDirectory().parent_path(), templateFile->src, rootPath)) {
            spdlog::debug("Template creation also affects scope of parent template: {}", templatePath->string());
            writeTemplate(*templateFile, *templatePath);
        }
        return;
    }

    // Now loop through all template files and look for those, above `path`
    // If we find them, and this file matches the extensions (or is a template)
    // regenerate.
    for (const auto& templateFile : config.files) {
        if (!templateFile.hasExtension(path.extension())) {
            spdlog::debug("File extension does not match those for this watcher: {}", templateFile.src);
            continue;
        }

        // Is there a matching template file above this file?
        if (const auto& templatePath = findUp(watch.getDirectory(), templateFile.src, rootPath)) {
            writeTemplate(templateFile, *templatePath);
            continue;
        }

        spdlog::trace("Could not find template file {} above {}", templateFile.src, path.string());
    }
}

void BuildWatch::onCreated(const inotify_event& event, const INotifyWatch& watch)
{
    const auto path = watch.getDirectory() / event.name;

    // Watch a new or moved directory
    if (event.mask & IN_ISDIR) {
        spdlog::debug("Directory created {}", path.string());
        watchDirectory(path);
        return;
    }

    onCreateOrMoveFile(event, watch);
}

void BuildWatch::onDeleted(const inotify_event& event, const INotifyWatch& watch)
{
    const auto path = watch.getDirectory() / event.name;

    // Do not watch any deleted or moved directories
    if (event.mask & IN_ISDIR) {
        inotify.remove(path);
        spdlog::debug("Directory deleted {}", path.string());
        return;
    }

    // Regenerate any files
    for (const auto& templateFile : config.files) {
        const auto self = watch.getDirectory() / templateFile.src;

        if (!templateFile.hasExtension(path.extension()) && path != self) {
            spdlog::debug("File extension does not match this watcher: {} for {}", templateFile.src, path.string());
            continue;
        }

        // Skip generated files from templates - leave it to the user to delete
        if (path == self) {
            spdlog::warn("Template deleted, you might want to delete the generated file: {}", path.string());
            if (const auto& templatePath = findUp(watch.getDirectory().parent_path(), templateFile.src, rootPath)) {
                spdlog::debug("Template deletion also affects scope of parent template: {}", templatePath->string());
                writeTemplate(templateFile, *templatePath);
            }

            continue;
        }

        // And is there a matching template file above it?
        if (const auto& templatePath = findUp(watch.getDirectory(), templateFile.src, rootPath)) {
            writeTemplate(templateFile, *templatePath);
            continue;
        }

        spdlog::trace("Could not find template file {} above {}", templateFile.src, path.string());
    }
}

void BuildWatch::onModified(const inotify_event& event, const INotifyWatch& watch)
{
    const auto path = watch.getDirectory() / event.name;

    // Watch a new or moved directory
    if (event.mask & IN_ISDIR) {
        spdlog::warn("Weird, modified directory?? {}", path.string());
        return;
    }

    // Skip if we're not a file
    if (!fs::is_regular_file(path)) {
        spdlog::debug("Created, but not a regular file: {}", path.string());
        return;
    }

    if (const auto& watcher = config.findFilename(event.name)) {
        spdlog::debug("Template created: {}", path.string());
        writeTemplate(*watcher, path);
        return;
    }

    spdlog::trace("Modify event, but skipping: {}", path.string());
}

void BuildWatch::onMovedFrom(const inotify_event& event, const INotifyWatch& watch)
{
    const auto path = watch.getDirectory() / event.name;

    // Watch a new or moved directory
    if (event.mask & IN_ISDIR) {
        inotify.moveFrom(path, event.cookie);
        spdlog::debug("Directory moved from {}", path.string());
        return;
    }

    onDeleted(event, watch);
}

void BuildWatch::onMovedTo(const inotify_event& event, const INotifyWatch& watch)
{
    // Moved, add new watch
    if (event.mask & IN_ISDIR) {
        const auto path = watch.getDirectory() / event.name;
        inotify.moveTo(path, event.cookie);
        spdlog::debug("Directory moved to {}", path.string());

        for (const auto& templateFile : config.files) {
            if (const auto& templatePath = findUp(path, templateFile.src, rootPath)) {
                writeTemplate(templateFile, *templatePath);
            }
        }
    } else {
        onCreateOrMoveFile(event, watch);
    }
}

void BuildWatch::onEvent(const inotify_event& event, const INotifyWatch& watch)
{
    if (event.len) {
        const auto path = watch.getDirectory() / event.name;

        /// These must match what we asked for in inotify
        spdlog::trace("Cookie={}  Event={}  Path={}", event.cookie, to_string(INotifyEvent{event.mask}), path.string());

        if (event.mask & IN_CREATE) {
            onCreated(event, watch);
        }
        if (event.mask & IN_DELETE) {
            onDeleted(event, watch);
        }
        if (event.mask & IN_MODIFY) {
            onModified(event, watch);
        }
        if (event.mask & IN_MOVED_FROM) {
            onMovedFrom(event, watch);
        }
        if (event.mask & IN_MOVED_TO) {
            onMovedTo(event, watch);
        }
    }
}

bool skipFileUnderDifferentTemplateFile(
    std::vector<std::filesystem::path> const& nestedConfig, const std::filesystem::path& path)
{
    return std::ranges::any_of(nestedConfig, [&path](const auto& nestedTemplateFile) {
        return path.parent_path().string().contains(nestedTemplateFile.parent_path().string());
    });
}

std::vector<std::filesystem::path> getAllFiles(
    const std::filesystem::path& templateFile,
    const std::vector<std::string>& extensions,
    const bool relativeToTemplate = true)
{
    namespace fs = std::filesystem;
    namespace rv = std::ranges;

    std::vector<fs::path> paths;

    if (!fs::is_regular_file(templateFile)) {
        throw std::runtime_error(fmt::format("Expected path to file as anchor: {}", templateFile.string()));
    }

    std::vector<fs::path> nestedConfig;
    for (const auto& entry : fs::recursive_directory_iterator(templateFile.parent_path())) {
        if (entry.is_regular_file() && entry.path().filename().string() == templateFile.filename().string()
            && entry.path() != templateFile) {
            nestedConfig.emplace_back(entry.path());
        }
    }

    for (const auto& entry : fs::recursive_directory_iterator(templateFile.parent_path())) {
        if (!is_regular_file(entry.path()) || !rv::contains(extensions, entry.path().extension().string())) {
            continue;
        }

        if (skipFileUnderDifferentTemplateFile(nestedConfig, entry.path())) {
            spdlog::debug("Skipping file {} as it is within a nested template", entry.path().filename().string());
            continue;
        }

        if (relativeToTemplate) {
            paths.emplace_back(fs::relative(entry.path(), templateFile.parent_path()));
        } else {
            paths.emplace_back(entry.path());
        }
    }

    // Return in sorted order
    std::ranges::sort(paths, [](const std::filesystem::path& lhs, const std::filesystem::path& rhs) {
        return lhs.string() < rhs.string();
    });

    return paths;
}

void BuildWatch::writeTemplate(const TemplateFile& templateFile, const std::filesystem::path& templatePath)
{
    // if (watcher.relativePaths) { files = btl::relative(files, templatePath.parent_path()); }
    const auto destPath = templatePath.parent_path() / templateFile.dest;

    spdlog::info("Reading {}", templatePath.string());
    // https://github.com/kainjow/Mustache

    std::ifstream istr(templatePath.string());
    if (!istr) {
        spdlog::error("Could not open template file {}", templatePath.string());
        return;
    }

    std::stringstream str;
    str << istr.rdbuf();                    // read the file
    std::string templateString = str.str(); // str holds the content of the file

    using namespace kainjow::mustache;
    mustache tmpl(templateString);
    data files{data::type::list};

    auto matchingFiles = getAllFiles(templatePath, templateFile.extensions);
    std::ranges::sort(matchingFiles, [](const std::filesystem::path& lhs, const std::filesystem::path& rhs) {
        return lhs.string() < rhs.string();
    });

    for (const auto& file : matchingFiles) {
        const bool isLast = (file == matchingFiles.back());
        data d;
        d.set("relpath", file.string());
        d.set("last", data(isLast ? data::type::bool_true : data::type::bool_false));
        files << d;
    }
    if (dryRun) {
        spdlog::info("Writing {}", destPath.string());
        tmpl.render({"files", files}, std::cout);
    } else {
        spdlog::info("Writing {}", destPath.string());
        std::ofstream os(destPath);
        tmpl.render({"files", files}, os);
    }
}

} // namespace btl