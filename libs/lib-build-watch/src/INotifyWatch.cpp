#include <build-watch/INotifyWatch.hpp>

namespace btl {

INotifyWatch::INotifyWatch(int fd, const std::filesystem::path& path, const std::uint32_t flags) : fd(fd), path_(path) {
    wd = inotify_add_watch(fd, path.c_str(), flags);
    if (wd < 0) { throw std::system_error(errno, std::system_category()); }
}

 INotifyWatch::INotifyWatch(INotifyWatch&& other) noexcept
: fd(std::exchange(other.fd, -1)), wd(std::exchange(other.wd, -1), path_(std::exchange(other.path_, {}){}
} // namespace btl