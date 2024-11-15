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

#include <gtest/gtest.h>

#include "FileUtils.hpp"
#include "Ignore.hpp"
#include <regex>

namespace fs = std::filesystem;

TEST(IgnoreTest, regex)
{
    const auto gitignorePath = btl::findUp(fs::current_path(), fs::path(".gitignore"));

    if (!gitignorePath) {
        FAIL() << "Failed to find up path";
    }

    std::regex regexPattern("out/.*");

    const auto repoRoot = gitignorePath->parent_path();
    const auto testAbsolutePath = repoRoot / "out/build/unixlike-gcc-debug-mold/_deps/zstd-src/contrib/gen_html";

    ASSERT_TRUE(std::regex_match("out/thing", regexPattern)) << "should match regex when partial path starts with out/";
    ASSERT_FALSE(std::regex_match(testAbsolutePath.string(), regexPattern)) << "should NOT match regex when absolute path contains out/";


    const auto lines = std::vector<std::string>{ "out/"};
    const btl::Ignore gitignore(repoRoot, lines);
    ASSERT_TRUE(gitignore.ignore(testAbsolutePath));
}


TEST(IgnoreTest, shouldIgnoreGitFolder)
{
    const auto gitignorePath = btl::findUp(fs::current_path(), fs::path(".gitignore"));

    if (!gitignorePath) {
        FAIL() << "Failed to find up path";
    }
    const auto repoRoot = gitignorePath->parent_path();

    const auto lines = std::vector<std::string>{ "out/"}; // .git gets added automatically.
    const btl::Ignore gitignore(repoRoot, lines);

    const auto testAbsolutePath = repoRoot / ".git/objects/1";
    ASSERT_TRUE(gitignore.ignore(testAbsolutePath));

    const auto testGitDirectory = repoRoot / ".git";
    ASSERT_TRUE(gitignore.ignore(testGitDirectory));
}
