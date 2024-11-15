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
#include <nlohmann/json.hpp>
#include <string>

namespace btl {
/// This is the object that forms the list in the config files.
/// Takes a src file (mustache template)
/// And a list of extensions, and a destination file name.
struct TemplateFile
{
    std::string src;
    std::string dest;
    std::vector<std::string> extensions;

    [[nodiscard]] bool hasExtension(const std::string& extension) const
    {
        return std::ranges::contains(extensions, extension);
    }

    static TemplateFile defaultConfiguration()
    {
        return TemplateFile{"CMakeLists.txt.mustache", "CMakeLists.txt", {".cpp", ".hpp", ".h"}};
    }
};

/// The root object in the config files.
struct Config
{
    std::vector<TemplateFile> files;
    std::vector<std::string> ignoreFiles;

    /// Find a file name in the list of files
    [[nodiscard]] std::optional<TemplateFile> findFilename(const std::string& src) const;

    /// Get the default config
    static Config defaultConfiguration();

    friend std::ostream& operator<<(std::ostream& os, const Config& config);
};

/// Convert config to json
void to_json(nlohmann::json& j, const Config& config);

/// Convert json to config
void from_json(const nlohmann::json& j, Config& config);

/// To (json) string
std::string to_string(const Config& config);

/// Easy output
std::ostream& operator<<(std::ostream& os, const Config& config);
} // namespace btl