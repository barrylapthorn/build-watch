include(CTest)

enable_testing()

add_executable(libBuildWatchTests
    BuildWatchTest.cpp
    ConfigTest.cpp
    FileUtilsTest.cpp
    INotifyTest.cpp
    IgnoreTest.cpp
)

target_include_directories(libBuildWatchTests
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/
)

target_link_libraries(libBuildWatchTests
    PRIVATE
    compiler_options

    gtest
    gtest_main
    gmock

    spdlog::spdlog

    libBuildWatch
    libTestHelpers
)

include(GoogleTest)

gtest_discover_tests(libBuildWatchTests)

add_test(NAME libBuildWatchTests COMMAND libBuildWatchTests)
