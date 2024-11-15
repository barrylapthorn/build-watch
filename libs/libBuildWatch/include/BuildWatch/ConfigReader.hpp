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

#include <BuildWatch/Config.hpp>
#include <filesystem>
#include <string>

namespace btl {

/// Gets the config
///
/// -  `$(pwd)/.config/BuildWatch/config.json`
///
/// otherwise walks up the folder tree until it finds:
///
/// -  `<parent path>/.config/BuildWatch/config.json`
///
/// otherwise returns default value
class ConfigReader
{
public:
    /// Read the configuration from the supplied path, or works upwards.  If nothing is found,
    /// `defaultValue` is used instead.
    explicit ConfigReader(const std::filesystem::path& startingPath, std::string defaultValue);

    /// Get the watchers from the config
    /// @return the `defaultValue` if config isn't found, otherwise the file content
    [[nodiscard]] const Config& get() const;

    /// Returns the root where we found the configuration, otherwise, the startingPath
    /// supplied during construction.
    /// @return
    [[nodiscard]] std::filesystem::path root() const;

private:
    [[nodiscard]] std::string getFileContent(std::filesystem::path const& configPath) const;

    std::filesystem::path rootPath{};
    Config config{};
};
} // namespace btl