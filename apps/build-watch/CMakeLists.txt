
# Configure a file to give us some static info at compile time.

configure_file(src/info.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/src/info.hpp)


add_executable(build-watch
    src/info.hpp
    src/main.cpp
)

target_link_libraries(build-watch
    PRIVATE
    compiler_options

    CLI11::CLI11
    spdlog
    fmt::fmt
    nlohmann_json::nlohmann_json
    cpptrace::cpptrace
    cmake_git_version_tracking

    libBuildWatch
)

install(
    TARGETS
    build-watch
    RUNTIME DESTINATION
    bin
)


set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

set(CPACK_PACKAGE_DIRECTORY "${PROJECT_SOURCE_DIR}/out")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

include(CPack)