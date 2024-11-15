#include <gtest/gtest.h>

#include <build-watch/BuildWatch.hpp>

TEST(BuildWatchTest, construction) {
    using namespace btl;
    BuildWatch watcher;
    (void) watcher;
}