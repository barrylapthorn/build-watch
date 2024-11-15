include(CTest)

enable_testing()

add_executable(libTestHelpersTest
    TempDirectoryTest.cpp
)

target_link_libraries(libTestHelpersTest
    PRIVATE
    compiler_options

    gtest
    gtest_main
    gmock

    spdlog::spdlog

    libTestHelpers
)

include(GoogleTest)

gtest_discover_tests(libTestHelpersTest)

add_test(NAME libTestHelpersTest COMMAND libTestHelpersTest)
