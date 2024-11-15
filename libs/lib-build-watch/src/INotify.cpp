#include <build-watch/INotify.hpp>

namespace btl {

INotify::INotify(const std::function<void(std::filesystem::path const&)>& createOrDeleteCallback) : callback(createOrDeleteCallback) {
    fd = inotify_init(); // init1 and NON_BLOCK and could use epoll
    if (fd < 1) { throw std::system_error(errno, std::system_category()); }
}

INotify::~INotify() { close(fd); }

void INotify::handleEvent(inotify_event const& event) {
    namespace fs = std::filesystem;
    namespace rv = std::ranges;

    spdlog::debug("Handle event {}", event.name);
    if (event.len > 0) {
        const auto path = watchedDirectories[event.wd] / fs::path(event.name);
        if (event.mask & IN_CREATE) {
            if (event.mask & IN_ISDIR) {
                addWatch(path);
                spdlog::info("Watching new directory {}", path.string());
            } else {
                callback(path);
            }
        }
    }
}

void INotify::watch() {
    constexpr auto eventSize = sizeof(struct inotify_event);
    constexpr int bufferSize = (1024 * (eventSize + 16));
    std::vector<char> buffer(bufferSize);
    while (true) {
        const std::size_t length = read(fd, buffer.data(), buffer.size());
        if (length <= 0) { throw std::system_error(errno, std::system_category()); }
        std::size_t i = 0;
        while (i < length) {
            const auto* pEvent = reinterpret_cast<inotify_event*>(&buffer[i]);
            if (!pEvent) {
                spdlog::warn("No inotify event");
                break;
            }
            handleEvent(*pEvent);
            i += eventSize + pEvent->len;
        }
    }
}


} // namespace btl