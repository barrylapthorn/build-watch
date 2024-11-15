# Best way to share compiler options between projects
# e.g. https://jeremimucha.com/2021/02/cmake-fundamentals-part5/

# we *must* add this in the PRIVATE section of other targets so that
# it doesn't 'leak' out and we can extend as needed.

add_library(compiler_options INTERFACE)

target_compile_options(compiler_options
        INTERFACE
        -Wall
        -Werror
        -Wno-error=deprecated-declarations
        -Wno-error=attributes
        # $<$<COMPILE_LANGUAGE:CXX>:-Weffc++> # example, don't use.
        # Don't mess around with these at the moment, just use cmake defaults
#        $<$<CONFIG:RELEASE>:-O3>
#        $<$<CONFIG:RelWithDebInfo>:-O2>
#        $<$<CONFIG:RelWithDebInfo>:-g>
#        $<$<CONFIG:RelWithDebInfo>:-DNDEBUG>
        #        $<$<CONFIG:DEBUG>:-ggdb3>
)

target_compile_features(compiler_options
        INTERFACE
        cxx_std_20
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
# don't pick up non-standard extensions, e.g. -std=gnu++17
set(CMAKE_CXX_EXTENSIONS OFF)
