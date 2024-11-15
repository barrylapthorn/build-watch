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
#include <string>
#include <vector>

namespace btl {

/// Take a gitignore file, read it, and then store it and apply to filesystem path and return true/false
/// Really rough v1.  Doesn't care about nested gitignores.
class Ignore
{
public:
    Ignore() = default;
    Ignore(std::filesystem::path  repoRoot, std::filesystem::path const& path);
    Ignore(std::filesystem::path  repoRoot, std::vector<std::string> const& lines);

    /// Returns true if we should ignore this path
    [[nodiscard]] bool ignore(std::filesystem::path const& path) const;

private:
    struct IgnoreRegex
    {
        bool isNegative{};
        std::string regex;
    };

    std::vector<IgnoreRegex> patterns{};
    std::filesystem::path repoRoot{};

    [[nodiscard]] static std::vector<std::string> read(std::filesystem::path const& path) ;
    [[nodiscard]] static std::string toRegex(std::string const& pattern);
    [[nodiscard]] static std::vector<IgnoreRegex> toRegex(std::vector<std::string> const& lines);
};



}
