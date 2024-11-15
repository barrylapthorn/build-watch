# GOOGLE TEST
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)


#  GOOGLE BENCHMARK
#  otherwise tests itself on every build
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(googlebenchmark
        URL https://github.com/google/benchmark/archive/refs/tags/v1.8.0.zip
)
FetchContent_MakeAvailable(googlebenchmark)


#  FMT lib - more up to date than std:: equivalent
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

#  SPDLOG
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(spdlog)
set(SPDLOG_FMT_EXTERNAL ON)

#target_compile_definitions(spdlog PUBLIC SPDLOG_FMT_EXTERNAL)
#target_compile_definitions(spdlog_header_only INTERFACE SPDLOG_FMT_EXTERNAL)

# CLI11
FetchContent_Declare(
        cli11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11
        GIT_TAG        v2.3.2
)
FetchContent_MakeAvailable(cli11)

#  JSON
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

#  Portable stacktrace - pre C++23
# https://github.com/jeremy-rifkin/cpptrace?tab=readme-ov-file
FetchContent_Declare(
        cpptrace
        GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
        GIT_TAG        v0.7.2 # <HASH or TAG>
)
FetchContent_MakeAvailable(cpptrace)

# Get git sha info and so on
# https://github.com/andrew-hardin/cmake-git-version-tracking/tree/masterc/
FetchContent_Declare(cmake_git_version_tracking
        GIT_REPOSITORY https://github.com/andrew-hardin/cmake-git-version-tracking.git
        GIT_TAG 904dbda1336ba4b9a1415a68d5f203f576b696bb
)
FetchContent_MakeAvailable(cmake_git_version_tracking)