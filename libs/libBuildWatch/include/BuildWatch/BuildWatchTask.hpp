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
#include <filesystem>
#include <thread>

namespace btl {
/// Run the build watch task in a thread so that we can cancel it and release various resources gracefully.
class BuildWatchTask
{
public:
    ~BuildWatchTask();

    /// Start watching the source tree.  Non-blocking.  Will start a thread and return straight away.
    /// @param root root path to watch
    /// @param config list of template files (as per configuration)
    /// @param dryRun whether to just print to stdout but NOT rewrite the build files.
    void start(std::filesystem::path const& root, Config const& config, const bool dryRun);

    /// Stop the watcher thread, if running.
    void stop();

private:
    std::jthread thread_{};
};
} // namespace btl