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

#include "INotify.hpp"
#include "INotifyWatch.hpp"
#include <algorithm>
#include <fmt/std.h>
#include <spdlog/spdlog.h>
#include <sys/epoll.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <utility>

namespace rg = std::ranges;
namespace fs = std::filesystem;

namespace btl {

void INotify::addWatch(std::filesystem::path const& directory, const INotifyCallback& callback)
{
    // These flags must match what we're watching
    addWatch(directory, IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_TO | IN_MOVED_FROM, callback);
}

void INotify::addWatch(std::filesystem::path const& directory, int flags, const INotifyCallback& callback)
{
    watches.emplace_back(std::make_unique<INotifyWatch>(inotifyWrapper.getFd(), directory, flags, callback));
}

void INotify::remove(const INotifyWatch& watch)
{
    std::erase_if(watches, [&watch](const auto& other) { return *other == watch; });
}

void INotify::remove(const std::filesystem::path& directory)
{
    // Remove any subdirectories watched as well, obviously.
    const auto e = rg::remove_if(watches, [&directory](const auto& pWatch) {
        fs::path p = pWatch->getDirectory();
        const auto root = p.root_path();
        while (p != root) {
            if (p == directory) {
                return true;
            }
            p = p.parent_path();
        }
        return false;
    });

    watches.erase(e.begin(), e.end());
}

void INotify::moveTo(std::filesystem::path const& directory, std::uint32_t cookie)
{
    const auto cookieIter = rg::find_if(watches, [&cookie](const auto& pWatch) { return pWatch->cookie == cookie; });
    if (cookieIter != watches.end()) {
        spdlog::trace("Watch cookie {} exists, setting directory: {}", cookie, directory.string());
        (*cookieIter)->directory = directory;
    } else {
        spdlog::debug("Watch cookie {} not found", cookie);
    }
}

void INotify::moveFrom(std::filesystem::path const& directory, std::uint32_t cookie)
{
    // from `man inotify`
    // rename("dir1/myfile", "dir2/myfile");
    //  Generates an IN_MOVED_FROM  event  for  dir1,  an  IN_MOVED_TO  event  for  dir2,  and  an
    //  IN_MOVE_SELF  event  for  myfile.   The IN_MOVED_FROM and IN_MOVED_TO events will have the
    //  same cookie value.
    const auto directoryIter = rg::find_if(watches, [&directory](const auto& pWatch) {
        return pWatch->directory == directory;
    });
    if (directoryIter != watches.end()) {
        spdlog::trace("Directory exists, setting cookie to {}: {}", cookie, directory.string());
        (*directoryIter)->cookie = cookie;
    } else {
        spdlog::debug("Moved-from directory not watched: {}", directory);
    }
}

void INotify::watchOnce()
{
    std::vector<epoll_event> events(10);
    const auto eventCount = epoll_wait(epoll.fd(), events.data(), events.size(), 0);
    if (eventCount < 0) {
        if (errno == EINTR) {
            return; // Interrupted by signal, retry
        }
        spdlog::error("epoll_wait");
        return;
    }
    for (int i = 0; i < eventCount; ++i) {
        processEvent();
    }
}

INotify::INotify()
{
    epoll.add(inotifyWrapper.getFd());
}

void INotify::processEvent()
{
    constexpr std::int64_t eventSize = sizeof(struct inotify_event);
    constexpr std::int64_t bufferSize = 1024 * (eventSize + 16);

    std::vector<char> buffer(bufferSize);

    const auto length = read(inotifyWrapper.getFd(), buffer.data(), buffer.size());
    if (length < 0) {
        spdlog::warn("INotify: read() failed: {}", strerror(errno));
        return;
    }

    std::int64_t i = 0;
    while (i < length) {
        const auto pEvent = reinterpret_cast<struct inotify_event*>(&buffer.at(i));
        if (pEvent) {
            // Yes, linear search, for now
            const auto iter = rg::find_if(watches, [pEvent](const auto& other) { return other->wd == pEvent->wd; });
            if (iter != watches.end()) {
                (*iter)->onEvent(*pEvent);
            } else {
                spdlog::warn("INotify: unknown wd = {}", pEvent->wd);
            }
        } else {
            spdlog::warn("INotify: read() failed: {}", strerror(errno));
        }
        i += eventSize + pEvent->len;
    }
}

} // namespace btl