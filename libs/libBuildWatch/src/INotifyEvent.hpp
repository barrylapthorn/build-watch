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
#include "FileUtils.hpp"
#include <sstream>
#include <sys/inotify.h>

namespace btl {
struct INotifyEvent
{
    std::uint32_t mask{};

    explicit INotifyEvent(const std::uint32_t mask)
        : mask(mask)
    {}
};

inline std::string to_string(const INotifyEvent& event)
{
    std::stringstream ss;
    if (event.mask & IN_ACCESS) {
        ss << "IN_ACCESS|";
    }
    if (event.mask & IN_MODIFY) {
        ss << "IN_MODIFY|";
    }
    if (event.mask & IN_ATTRIB) {
        ss << "IN_ATTRIB|";
    }
    if (event.mask & IN_CLOSE_WRITE) {
        ss << "IN_CLOSE_WRITE|";
    }
    if (event.mask & IN_CLOSE_NOWRITE) {
        ss << "IN_CLOSE_NOWRITE|";
    }
    if (event.mask & IN_CLOSE) {
        ss << "IN_CLOSE|";
    }
    if (event.mask & IN_OPEN) {
        ss << "IN_OPEN|";
    }
    if (event.mask & IN_MOVED_FROM) {
        ss << "IN_MOVED_FROM|";
    }
    if (event.mask & IN_MOVED_TO) {
        ss << "IN_MOVED_TO|";
    }
    // if (event.mask & IN_MOVE) { ss << "IN_MOVE|"; } // IN_MOVE = IN_MOVED_FROM | IN_MOVED_TO
    if (event.mask & IN_CREATE) {
        ss << "IN_CREATE|";
    }
    if (event.mask & IN_DELETE) {
        ss << "IN_DELETE|";
    }
    if (event.mask & IN_DELETE_SELF) {
        ss << "IN_DELETE_SELF|";
    }
    if (event.mask & IN_MOVE_SELF) {
        ss << "IN_MOVE_SELF|";
    }
    auto result = ss.str();
    if (result.empty()) {
        return result;
    }
    result.pop_back();
    return result;
}
} // namespace btl