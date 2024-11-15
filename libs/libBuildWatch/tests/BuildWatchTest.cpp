#include <TestHelpers/TempDirectory.hpp>
#include "BuildWatch.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

TEST(BuildWatchTest, construction)
{
    using namespace btl;
    BuildWatch watcher;
    (void) watcher;
}

TEST(BuildWatchTest, defaultConfig)
{
    using namespace btl;
    BuildWatch watcher;
    ASSERT_FALSE(watcher.defaultConfig().empty());
    ASSERT_GT(watcher.defaultConfig().size(), 0);
}
