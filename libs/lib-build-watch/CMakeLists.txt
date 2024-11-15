
add_library(lib-build-watch
        include/build-watch/BuildWatch.hpp
        include/build-watch/INotify.hpp
        include/build-watch/INotifyWatch.hpp
        include/build-watch/Watch.hpp
        src/BuildWatch.cpp
        src/INotify.cpp
        src/INotifyWatch.cpp
)

target_include_directories(lib-build-watch
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(lib-build-watch
        PUBLIC
        fmt::fmt
        spdlog::spdlog

        PRIVATE
        compiler_options
        third-party
)

if (PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif ()
