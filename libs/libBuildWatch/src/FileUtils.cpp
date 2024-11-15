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

#include "FileUtils.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

namespace btl {

std::optional<std::filesystem::path> findUp(
    std::filesystem::path path, const std::filesystem::path& pathToFind, const std::filesystem::path& stopAtPath)
{
    namespace fs = std::filesystem;
    // is_directory physically tests the directory via filesystem calls
    // if (!fs::is_directory(path)) {
    //     throw std::invalid_argument(fmt::format("path is not a directory: {}", path.string()));
    // }
    int depth = 0;
    while (path != stopAtPath && depth < 100) {
        const auto absolutePath = path / pathToFind;
        spdlog::trace("Searching in {} for {}", path.parent_path().string(), pathToFind.string());
        if (fs::exists(absolutePath)) {
            spdlog::debug("Found {}", absolutePath.string());
            return std::make_optional(absolutePath);
        }

        path = path.parent_path();
        ++depth; // break out of any stupid recursion bugs
    }

    return std::nullopt;
}

std::optional<std::filesystem::path> findUp(const std::filesystem::path& path, const std::filesystem::path& pathToFind)
{
    return findUp(path, pathToFind, path.root_path());
}

std::vector<std::filesystem::path> relative(
    const std::vector<std::filesystem::path>& paths, const std::filesystem::path& base)
{
    namespace fs = std::filesystem;

    std::vector<fs::path> result;
    for (const auto& path : paths) {
        result.push_back(fs::relative(path, base));
    }
    return result;
}

bool hasExtension(const std::vector<std::string>& extensions, const std::filesystem::path& path)
{
    namespace rg = std::ranges;
    return rg::contains(extensions, path.extension().string());
}

std::vector<std::filesystem::path> findAll(
    const std::filesystem::path& rootDirectory, const std::vector<std::string>& extensions)
{
    namespace fs = std::filesystem;
    namespace rv = std::ranges;

    std::vector<std::filesystem::path> result;

    for (const auto& entry : fs::recursive_directory_iterator(rootDirectory)) {
        if (is_regular_file(entry.path()) && rv::contains(extensions, entry.path().extension().string())) {
            result.push_back(entry.path());
        }
    }

    return result;
}

}