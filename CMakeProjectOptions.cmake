# Start adding in some program options

    option(project_ENABLE_CACHE "Enable ccache" ON)
#    option(project_ENABLE_COVERAGE "Enable coverage" OFF)


if(project_ENABLE_CACHE)
    include(cmake/cache.cmake)
    project_enable_cache()
endif()

#if(project_ENABLE_COVERAGE)
#    include(cmake/coverage.cmake)
#    project_enable_coverage(project_options)
#endif()