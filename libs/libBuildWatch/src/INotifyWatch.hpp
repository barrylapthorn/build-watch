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

#include "MoveOnly.hpp"
#include <filesystem>
#include <fmt/std.h>
#include <spdlog/spdlog.h>
#include <sys/inotify.h>

namespace btl {
class INotifyWatch;
using INotifyCallback = std::function<void(const inotify_event&, const INotifyWatch&)>;

class INotifyWatch
{
public:
    INotifyWatch(int fd, std::filesystem::path const& directory, const int flags, INotifyCallback callback)
        : fd(fd)
        , directory(directory)
        , callback(std::move(callback))
    {
        if (!std::filesystem::is_directory(directory)) {
            throw std::runtime_error(fmt::format("Path is not a directory: {}", directory));
        }

        wd = inotify_add_watch(fd, directory.c_str(), flags);

        if (wd == -1) {
            throw std::system_error(errno, std::system_category());
        }

        spdlog::trace("INotifyWatch::INotifyWatch Added inotify watch fd={} wd={} dir={}", fd, wd.get(), directory);
    }

    ~INotifyWatch()
    {
        remove();
        spdlog::trace(
            "INotifyWatch::~INotifyWatch Removed inotify watch fd={} wd={} dir={}", fd.get(), wd.get(), directory);
    }

    void remove()
    {
        inotify_rm_watch(fd, wd);
        directory.clear();
    }

    [[nodiscard]] std::filesystem::path const& getDirectory() const { return directory; }

    void onEvent(const inotify_event& event) const { return callback(event, *this); }

    [[nodiscard]] bool operator==(const int wd) const { return wd == this->wd.get(); }

    [[nodiscard]] bool operator==(const INotifyWatch& other) const
    {
        return other.wd == wd && other.directory == directory && other.fd == fd;
    }

    friend class INotify;

private:
    MoveOnly<int, -1> fd{};
    MoveOnly<int, -1> wd{};
    std::uint32_t cookie{};
    std::filesystem::path directory{};
    INotifyCallback callback{};
};

} // namespace btl