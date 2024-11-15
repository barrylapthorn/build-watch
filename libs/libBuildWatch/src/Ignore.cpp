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

#include "Ignore.hpp"

#include "spdlog/spdlog.h"
#include <algorithm>
#include <cassert>
#include <fmt/std.h>
#include <ranges>
#include <regex>
#include <utility>

namespace rg = std::ranges;
namespace vw = std::views;
namespace fs = std::filesystem;

namespace {
void trim(std::string& s)
{
    const auto nonSpace = [](unsigned char c) { return !std::isspace(c); };

    // erase at the back
    s.erase(rg::find_if(s | vw::reverse, nonSpace).base(), s.end());

    // erase at the front
    s.erase(s.begin(), rg::find_if(s, nonSpace));
}

std::string escapeStringForRegex(const std::string& s)
{
    static constexpr char metacharacters[] = R"(\.^$+()[]{}|?*)";
    std::string out;
    out.reserve(s.size());
    for (const auto ch : s) {
        if (std::strchr(metacharacters, ch)) {
            out.push_back('\\');
        }
        out.push_back(ch);
    }
    return out;
}
} // namespace

namespace btl {

Ignore::Ignore(std::filesystem::path repoRoot, std::filesystem::path const& path)
    : patterns(toRegex(read(path)))
    , repoRoot(std::move(repoRoot))
{}

Ignore::Ignore(std::filesystem::path repoRoot, std::vector<std::string> const& lines)
    : patterns(toRegex(lines))
    , repoRoot(std::move(repoRoot))
{}

std::vector<std::string> Ignore::read(std::filesystem::path const& path)
{
    std::ifstream is(path);
    if (!is) {
        throw std::runtime_error(fmt::format("Failed to open file {}", path));
    }

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(is, line)) {
        trim(line);
        if (line.empty() || line.at(0) == '#') {
            continue;
        }

        lines.push_back(line);
    }
    return lines;
}

std::string Ignore::toRegex(std::string const& pattern)
{
    std::string regexPattern;
    size_t i = 0;
    while (i < pattern.size()) {
        if (pattern[i] == '*') {
            if (i + 1 < pattern.size() && pattern[i + 1] == '*') {
                // Handle '**' (matches zero or more directories)
                regexPattern += ".*";
                i++;
            } else {
                // Handle single '*'
                regexPattern += "[^/]*";
            }
        } else if (pattern[i] == '?') {
            regexPattern += ".";
        } else if (pattern[i] == '/') {
            regexPattern += "/"; // Keep directory separators as-is
        } else if (pattern[i] == '.') {
            regexPattern += "\\.";
        } else {
            regexPattern += escapeStringForRegex(std::string(1, pattern[i]));
        }
        i++;
    }

    // Ensure directory-specific patterns ending with '/' match subpaths
    if (!pattern.empty() && pattern.back() == '/') {
        regexPattern += ".*";
    }
    return regexPattern;
}

std::vector<Ignore::IgnoreRegex> Ignore::toRegex(std::vector<std::string> const& lines)
{
    std::vector<IgnoreRegex> result;
    for (auto line : lines) {
        trim(line);
        if (line.empty() || line.at(0) == '#') {
            continue;
        }
        // Since we're watching *directories*, skip any gitignore rules that aren't directories.
        if (! rg::contains(line, '/')) {
            continue;
        }
        bool isNegative = false;
        if (line.at(0) == '!') {
            isNegative = true;
            line = line.substr(1);
        }
        result.push_back({isNegative, toRegex(line)});
    }

    // And add .git and .hg directories.

    result.push_back({false, toRegex(".git/")});
    result.push_back({false, toRegex(".hg/")});
    return result;
}

bool Ignore::ignore(std::filesystem::path const& path) const
{
    using namespace std::literals;

    assert(path.is_absolute() && "paths passed in here should be absolute");
    assert(!repoRoot.empty() && "repo root should not be empty - are you using a default constructed instance?");

    // gitignore matches paths relative to root.
    const auto relpath = fs::relative(path, repoRoot);
    spdlog::trace("Ignore::ignore relative path calculated as: {}", relpath);

    bool isIgnored = false;

    // Walk through regexes in line order, to figure out if we should ignore this path.
    for (const auto& pattern : patterns) {
        // Yes, std::regex :(
        std::regex regexPattern(pattern.regex);

        // Also test with a trailing `/` for a directory match.  Not using `is_directory` because that
        // seems to use filesystem calls.
        if (std::regex_match(relpath.string(), regexPattern) || std::regex_match(relpath.string() + "/", regexPattern)) {
            isIgnored = !pattern.isNegative; // Overwrite based on match type
        }
    }

    return isIgnored;
}

}