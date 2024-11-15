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

#pragma once

#include <BuildWatch/Config.hpp>
#include "INotify.hpp"
#include "INotifyWatch.hpp"
#include "Ignore.hpp"
#include <filesystem>
#include <string>
#include <sys/inotify.h>

namespace btl {

/// Recursively searches for the given template file(s) and monitors folders beneath
/// for file changes that match the given set of extensions.
///
/// Starts watching on construction.  Throws if there's a problem.
class BuildWatch
{
public:
    /// Default constructor
    BuildWatch() = default;

    void useIgnoreFile(const Config& config);
    /// Build a watcher
    /// @param rootDirectory source root to watch
    /// @param config list of template files and extensions to watch as specified in config
    /// @param dryRun whether to just print to stdout (and not write the build files)
    BuildWatch(const std::filesystem::path& rootDirectory, const Config& config, bool dryRun);

    /// Destruction
    ~BuildWatch() = default;

    /// Return the default configuration (that we print to stdout via `-g`)
    static std::string defaultConfig();

    /// Process any pending notifications and return (non-blocking).
    void watchOnce();

private:
    void onCreateOrMoveFile(const inotify_event& event, const INotifyWatch& watch);
    void onCreated(const inotify_event& event, const INotifyWatch& watch);
    void onDeleted(const inotify_event& event, const INotifyWatch& watch);
    void onModified(const inotify_event& event, const INotifyWatch& watch);
    void onMovedFrom(const inotify_event& event, const INotifyWatch& watch);
    void onMovedTo(const inotify_event& event, const INotifyWatch& watch);

    /// Watch directory and sub-dirs
    void watchDirectory(const std::filesystem::path& directory);

    void onEvent(const inotify_event& event, const INotifyWatch& watch);

    void writeTemplate(const TemplateFile& templateFile, const std::filesystem::path& templatePath);

    std::filesystem::path rootPath{};

    INotify inotify{};

    Config config{};

    bool dryRun{};

    Ignore ignore{};
};
} // namespace btl