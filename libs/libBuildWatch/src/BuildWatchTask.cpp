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
#include <BuildWatch/BuildWatchTask.hpp>
#include <BuildWatch/Config.hpp>
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>
#include <fmt/std.h>
#include <spdlog/spdlog.h>
#include <stop_token>

namespace btl {
void BuildWatchTask::start(std::filesystem::path const& root, Config const& config, const bool dryRun)
{
    thread_ = std::jthread([=](const std::stop_token& token) {
        CPPTRACE_TRY
        {
            spdlog::trace("Starting...");
            BuildWatch watcher(root, config, dryRun);
            while (!token.stop_requested()) {
                watcher.watchOnce();
                spdlog::trace("Waiting on task thread...");
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        CPPTRACE_CATCH(std::exception const& ex)
        {
            spdlog::error("Exception caught: {}", ex.what());
            cpptrace::from_current_exception().print();
        }
        spdlog::info("Stopped");
    });
}

void BuildWatchTask::stop()
{
    thread_.request_stop();
}

BuildWatchTask::~BuildWatchTask()
{
    stop();
}
} // namespace btl