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
#include <spdlog/spdlog.h>
#include <sys/inotify.h>
#include <system_error>

namespace btl {

/// RRID around inotify
class INotifyWrapper
{
public:
    INotifyWrapper()
    {
        fd = inotify_init1(IN_NONBLOCK);
        if (fd == -1) {
            throw std::system_error(std::error_code{errno, std::system_category()});
        }

        spdlog::trace("INotifyWrapper::INotifyWrapper inotify fd={}", fd.get());
    }

    ~INotifyWrapper()
    {
        spdlog::trace("INotifyWrapper::~INotifyWrapper Closing inotify fd={}", fd.get());
        close(fd);
    }

    /// You probably don't want to use this.  Do NOT close it.
    [[nodiscard]] int getFd() const { return fd.get(); }

private:
    MoveOnly<int, -1> fd{};
};

} // namespace btl