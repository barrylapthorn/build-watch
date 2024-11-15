include(CTest)

enable_testing()

add_executable(lib-build-watch-tests
    BuildWatchTest.cpp
)

target_include_directories(lib-build-watch-tests 
    PRIVATE 
    spdlog 
    lib-build-watch
)

target_link_libraries(lib-build-watch-tests
        PRIVATE
        compiler_options
        
        gtest 
        gtest_main 
        gmock 
        
        spdlog::spdlog
        
        lib-build-watch
)

include(GoogleTest)

gtest_discover_tests(lib-build-watch-tests)

add_test(NAME lib-build-watch-tests COMMAND lib-build-watch-tests)
