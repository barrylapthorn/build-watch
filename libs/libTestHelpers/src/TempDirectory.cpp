#include <TestHelpers/TempDirectory.hpp>
#include <cassert>
#include <fmt/std.h>
#include <random>

namespace btl {

namespace fs = std::filesystem;

TempDirectory::TempDirectory()
    : path_(createTempDirectory())
{}

TempDirectory::TempDirectory(TempDirectory&& other) noexcept
    : path_{std::exchange(other.path_, {})}
{}

TempDirectory& TempDirectory::operator=(TempDirectory&& other) noexcept
{
    path_ = std::exchange(other.path_, {});
    return *this;
}

TempDirectory::~TempDirectory()
{
    if (fs::exists(path_) && fs::is_directory(path_)) {
        fs::remove_all(path_);
        assert(!fs::exists(path_) && "Failed to delete temp directory");
    }
}

std::filesystem::path TempDirectory::createTempDirectory()
{
    const auto tempDirectoryPath = std::filesystem::temp_directory_path();
    std::uint64_t tries{};
    std::random_device dev;
    std::mt19937 prng(dev());
    std::uniform_int_distribution<uint64_t> rand(0);

    std::filesystem::path path;
    while (true) {
        std::stringstream ss;
        ss << std::hex << rand(prng);
        path = tempDirectoryPath / ss.str();
        // true if the directory was created.
        if (std::filesystem::create_directory(path)) {
            break;
        }
        if (constexpr std::uint32_t maxTries = 100; tries == maxTries) {
            throw std::runtime_error(fmt::format("Could not create temp directory in {}", path));
        }
        ++tries;
    }
    return path;
}

} // namespace btl