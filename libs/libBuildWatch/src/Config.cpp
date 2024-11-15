#include <BuildWatch/Config.hpp>

namespace btl {

std::optional<TemplateFile> Config::findFilename(const std::string& src) const
{
    namespace rv = std::ranges;

    if (const auto iter = rv::find_if(files, [&](const TemplateFile& watcher) { return watcher.src == src; });
        iter != files.end()) {
        return std::make_optional(*iter);
    }

    return std::nullopt;
}

Config Config::defaultConfiguration()
{
    return {{TemplateFile::defaultConfiguration()}, { ".gitignore" }};
}

void to_json(nlohmann::json& j, const Config& config)
{
    j = nlohmann::json{{"files", nlohmann::json::array()}, {"ignoreFiles", nlohmann::json::array()}};
    for (const auto& [src, dest, extensions] : config.files) {
        auto element = nlohmann::json{{"src", src}, {"dest", dest}, {"extensions", extensions}};
        j.at("files").push_back(element);
    }

    for (const auto& value : config.ignoreFiles) {
        j.at("ignoreFiles").push_back(value);
    }
}

void from_json(const nlohmann::json& j, Config& config)
{
    for (const auto& node : j.at("files")) {
        TemplateFile templateFile;
        templateFile.src = node.at("src");
        templateFile.dest = node.at("dest");
        templateFile.extensions = node.at("extensions");
        config.files.push_back(templateFile);
    }

    config.ignoreFiles = j.at("ignoreFiles");
}

std::string to_string(const Config& config)
{
    nlohmann::json j;
    btl::to_json(j, config);
    return j.dump(2);
}

std::ostream& operator<<(std::ostream& os, const Config& config)
{
    os << to_string(config);
    return os;
}
} // namespace btl