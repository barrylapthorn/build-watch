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

#include "BuildWatch/Config.hpp"

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

TEST(ConfigTest, construct)
{
    btl::Config config;
    config.files.push_back({.src = "src.txt", .dest = "dest.txt", .extensions = {".cpp", ".hpp"}});

    ASSERT_EQ(config.files.size(), 1);
    ASSERT_EQ(config.files[0].dest, "dest.txt");
    ASSERT_EQ(config.files[0].src, "src.txt");
    ASSERT_THAT(config.files[0].extensions, testing::ElementsAre(".cpp", ".hpp"));
}

TEST(ConfigTest, multiple)
{
    btl::Config config;
    config.files.push_back({.src = "src.txt", .dest = "dest.txt", .extensions = {".cpp", ".hpp"}});
    config.files.push_back({.src = "py.src.txt", .dest = "py.dest.txt", .extensions = {".py"}});

    ASSERT_EQ(config.files.size(), 2);
    ASSERT_EQ(config.files[0].dest, "dest.txt");
    ASSERT_EQ(config.files[0].src, "src.txt");
    ASSERT_THAT(config.files[0].extensions, testing::ElementsAre(".cpp", ".hpp"));

    ASSERT_EQ(config.files[1].dest, "py.dest.txt");
    ASSERT_EQ(config.files[1].src, "py.src.txt");
    ASSERT_THAT(config.files[1].extensions, testing::ElementsAre(".py"));
}

TEST(ConfigTest, serializationAndEquality)
{
    btl::Config expectedConfig;
    expectedConfig.files.push_back({.src = "src.txt", .dest = "dest.txt", .extensions = {".cpp", ".hpp"}});
    expectedConfig.files.push_back({.src = "py.src.txt", .dest = "py.dest.txt", .extensions = {".py"}});

    nlohmann::json expectedJson;
    btl::to_json(expectedJson, expectedConfig);
    const std::string actual = expectedJson.dump();
    const std::string expected
        = R"({"files":[{"dest":"dest.txt","extensions":[".cpp",".hpp"],"src":"src.txt"},{"dest":"py.dest.txt","extensions":[".py"],"src":"py.src.txt"}],"ignoreFiles":[]})";
    ASSERT_EQ(actual, expected);

    nlohmann::json actualJson = nlohmann::json::parse(actual);
    btl::Config actualConfig;
    btl::from_json(actualJson, actualConfig);
    ASSERT_EQ(actualJson, expectedJson);

    ASSERT_EQ(expectedConfig.files.size(), actualConfig.files.size());
    ASSERT_EQ(expectedConfig.files.at(0).src, actualConfig.files.at(0).src);
    ASSERT_EQ(expectedConfig.files.at(0).dest, actualConfig.files.at(0).dest);

    ASSERT_EQ(expectedConfig.files.at(1).src, actualConfig.files.at(1).src);
    ASSERT_EQ(expectedConfig.files.at(1).dest, actualConfig.files.at(1).dest);
}
