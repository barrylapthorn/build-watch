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
#include <filesystem>
#include <vector>

namespace btl {

/// Walk *up* the directory tree looking for `pathToFind` until we hit the root, or go 'up' too many times
///
/// @param path starting directory
/// @param pathToFind path to find
/// @param stopAtPath stop if the directory is eventually this
/// @return an empty optional if not found
[[nodiscard]] std::optional<std::filesystem::path> findUp(
    std::filesystem::path path, const std::filesystem::path& pathToFind, const std::filesystem::path& stopAtPath);

/// Walk *up* the directory tree looking for `pathToFind` until we hit the root, or go 'up' too many times
/// @param path
/// @param pathToFind
/// @return an optional that's empty if not found
[[nodiscard]] std::optional<std::filesystem::path> findUp(
    const std::filesystem::path& path, const std::filesystem::path& pathToFind);

/// Return the paths, relative to `base`
/// @param paths a collection of paths
/// @param base the base we want to be relative to
/// @return the list of relative paths
[[nodiscard]] std::vector<std::filesystem::path> relative(
    const std::vector<std::filesystem::path>& paths, const std::filesystem::path& base);

/// Is the path in the set of extensions?
/// @param path
/// @param extensions collection of extensions, which MUST start with a dot, i.e. `.`
/// @return true if the path ends with one of the extensions
[[nodiscard]] bool hasExtension(const std::vector<std::string>& extensions, const std::filesystem::path& path);

/// Find all files under the `rootDirectory` that have the given extensions
/// @param rootDirectory
/// @param extensions extensions MUST start with a dot, i.e. `.`
/// @return
[[nodiscard]] std::vector<std::filesystem::path> findAll(
    const std::filesystem::path& rootDirectory, const std::vector<std::string>& extensions);

} // namespace btl