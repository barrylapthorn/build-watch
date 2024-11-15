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

#include "Epoll.hpp"
#include "INotifyWatch.hpp"
#include "INotifyWrapper.hpp"
#include <filesystem>
#include <memory>
#include <sys/inotify.h>
#include <vector>

namespace btl {
class INotify
{
public:
    INotify();

    /// Repeatedly call this to watch all folders, non blocking
    void watchOnce();

    /// Add a watch for the given directory
    /// @param directory
    /// @param callback
    void addWatch(std::filesystem::path const& directory, const INotifyCallback& callback);

    /// Add a watch for the given directory
    /// @param directory
    /// @param flags inotify flags
    /// @param callback
    void addWatch(std::filesystem::path const& directory, int flags, const INotifyCallback& callback);

    /// Remove the given watch
    /// @param watch
    void remove(const INotifyWatch& watch);

    /// Remove the watch on the given directory
    /// @param directory
    void remove(const std::filesystem::path& directory);

    /// Set the directory for the given cookie, if cookie is set.
    ///
    ///     in.moveFrom("a/b/c", 1234);
    ///     in.moveTo("d/e/f", 1234);
    ///
    /// equals
    ///
    ///     mv a/b/c d/e/f
    ///
    void moveTo(std::filesystem::path const& directory, std::uint32_t cookie);

    /// Set the cookie on the given directory, if exists.
    ///
    ///     in.moveFrom("a/b/c", 1234);
    ///     in.moveTo("d/e/f", 1234);
    ///
    /// equals
    ///
    ///     mv a/b/c d/e/f
    ///
    void moveFrom(std::filesystem::path const& directory, std::uint32_t cookie);

private:
    void processEvent();

    INotifyWrapper inotifyWrapper{};
    std::vector<std::unique_ptr<INotifyWatch>> watches{};
    Epoll epoll{};
};
} // namespace btl