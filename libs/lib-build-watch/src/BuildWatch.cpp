#include <algorithm>
#include <build-watch/BuildWatch.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <sys/inotify.h>

namespace btl {
BuildWatch::BuildWatch(const std::string& rootDirectory, const std::vector<std::string>& includes, const std::vector<std::string>& excludes)
    : includes(includes), excludes(excludes) {
    startWatching(rootDirectory);
}

BuildWatch::BuildWatch(const std::string& rootDirectory, const std::vector<std::string>& templateFiles,
                       const std::vector<std::string>& includes, const std::vector<std::string>& excludes)
    : templateFiles(templateFiles), includes(includes), excludes(excludes) {

    startWatching(rootDirectory);
}

std::vector<std::filesystem::path> BuildWatch::getTemplateDirectories() {
    std::vector<std::filesystem::path> paths;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(rootPath)) {
        if (!entry.is_regular_file()) { continue; }
        const auto filename = entry.path().filename().string();
        if (!std::ranges::contains(templateFiles, filename)) { continue; }

        paths.emplace_back(entry.path().parent_path());
        spdlog::debug("Found {} in {}", filename, entry.path().parent_path().string());
    }
    return paths;
}

std::vector<std::filesystem::path> BuildWatch::getAllSubdirectories(const std::vector<std::filesystem::path>& paths) {
    std::vector<std::filesystem::path> allPaths;
    for (const auto& templatePath : paths) {
        for (const auto& path : std::filesystem::directory_iterator(templatePath)) { allPaths.emplace_back(path.path()); }
    }
    return allPaths;
}

void BuildWatch::startWatching(const std::string& rootDirectory) {
    namespace fs = std::filesystem;
    namespace rv = std::ranges;

    if (rootDirectory.empty()) {
        rootPath = fs::current_path();
    } else {
        rootPath = rootDirectory;
    }

    if (!fs::exists(rootPath)) { throw std::runtime_error(fmt::format("Supplied root-directory does not exist: {}", rootPath.c_str())); }

    const auto paths = getAllSubdirectories(getTemplateDirectories());

    if (paths.empty()) { throw std::runtime_error("No subdirectories found containing template files"); }

    spdlog::info("Watching {}", rootPath.c_str());
}
} // namespace btl