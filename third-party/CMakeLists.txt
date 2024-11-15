

add_library(third-party
        mustache.hpp
)

target_include_directories(third-party
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(third-party
        PRIVATE
        compiler_options
)

# Unneeded if we have cpp files
set_target_properties(third-party PROPERTIES LINKER_LANGUAGE CXX)

