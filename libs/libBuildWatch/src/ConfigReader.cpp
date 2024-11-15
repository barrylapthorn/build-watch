#include "BuildWatch/ConfigReader.hpp"
#include "BuildWatch/Config.hpp"
#include "FileUtils.hpp"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace btl {

ConfigReader::ConfigReader(const std::filesystem::path& path, std::string defaultValue)
    : rootPath(path)
{
    /// the default path
    const auto configRelPath = std::filesystem::path(".config") / "BuildWatch" / "config.json";

    if (const auto configPath = findUp(path, configRelPath)) {
        spdlog::info("Found config in {}", configPath.value().string());
        // overwrite
        defaultValue = getFileContent(configPath.value());
        rootPath = configPath.value().parent_path();
    } else {
        spdlog::info("Using default config, no directory found in parent directories called .config/BuildWatch/");
    }

    from_json(nlohmann::json::parse(defaultValue), config);
}

const Config& ConfigReader::get() const
{
    return config;
}

std::filesystem::path ConfigReader::root() const
{
    return rootPath;
}

std::string ConfigReader::getFileContent(std::filesystem::path const& configPath) const
{
    std::ifstream inStream(configPath.string());
    std::stringstream str;
    str << inStream.rdbuf();                // read the file
    std::string templateString = str.str(); // str holds the content of the file
    return templateString;
}
} // namespace btl