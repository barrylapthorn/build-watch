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

#include "Epoll.hpp"
#include <cstring>
#include <ranges>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

namespace btl {

Epoll::Epoll()
{
    epollFd = epoll_create1(0);
    if (epollFd < 0) {
        throw std::runtime_error(fmt::format("epoll_create1 = {}", strerror(errno)));
    }
}

Epoll::~Epoll()
{
    for (const auto& fd : std::views::keys(events)) {
        epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr);
    }

    if (const int ret = close(epollFd); ret < 0) {
        spdlog::warn("Epoll: close({}): {}", errno, strerror(errno));
    }
}

void Epoll::add(const int otherFd)
{
    auto& event = events[otherFd];
    event.data.fd = otherFd;
    event.events = EPOLLIN;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, otherFd, &event);
}

void Epoll::remove(int otherFd)
{
    if (!events.contains(otherFd)) {
        spdlog::warn("Epoll::remove: non existent fd {}", otherFd);
    } else {
        epoll_ctl(epollFd, EPOLL_CTL_DEL, otherFd, nullptr);
        events.erase(otherFd);
    }
}

Epoll::operator int() const
{
    return epollFd.get();
}

int Epoll::fd() const
{
    return epollFd.get();
}
} // namespace btl