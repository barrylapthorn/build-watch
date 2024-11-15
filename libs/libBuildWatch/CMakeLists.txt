# Generated from mustache template file.  Commit both.

add_library(libBuildWatch
    include/BuildWatch/BuildWatchTask.hpp
    include/BuildWatch/Config.hpp
    include/BuildWatch/ConfigReader.hpp
    src/BuildWatch.cpp
    src/BuildWatch.hpp
    src/BuildWatchTask.cpp
    src/Config.cpp
    src/ConfigReader.cpp
    src/Epoll.cpp
    src/Epoll.hpp
    src/FileUtils.cpp
    src/FileUtils.hpp
    src/INotify.cpp
    src/INotify.hpp
    src/INotifyEvent.hpp
    src/INotifyWatch.hpp
    src/INotifyWrapper.hpp
    src/Ignore.cpp
    src/Ignore.hpp
    src/MoveOnly.hpp
)

target_include_directories(libBuildWatch
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(libBuildWatch
    PUBLIC
    fmt::fmt
    spdlog::spdlog
    nlohmann_json::nlohmann_json

    PRIVATE
    compiler_options
    cpptrace::cpptrace
    third-party
    libTestHelpers
)

if (PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif ()
