# Justfile to build everything easily.
# https://github.com/casey/just

scripts_dir := justfile_directory() + '/scripts'
docs_dir := justfile_directory() + '/docs'
root_dir := justfile_directory()
build_dir := justfile_directory() + '/out'

default:
    @just --list --justfile {{justfile()}}

# configure using the basic gcc/debug preset
configure:
    cmake --preset "unixlike-gcc-debug" -S {{root_dir}}

# build whatever is configured
build: debug

# run *my* tests in debug - see the cmakepresets.json for the regex/include filter
test: debug
    ctest --preset "test-unixlike-gcc-debug"

# run *my* tests in release
test-release: release
    ctest --preset "test-unixlike-gcc-release"

# run unit tests and output results junit style
junit: debug
    ctest --preset "test-unixlike-gcc-debug" --output-junit {{build_dir}}/xunit/results.xml

# Build debug with gcc
debug:
    cmake --preset "unixlike-gcc-debug" -S {{root_dir}}
    cmake --build --preset "build-unixlike-gcc-debug" --parallel

# Build release with gcc
release:
    cmake --preset "unixlike-gcc-release" -S {{root_dir}}
    cmake --build --preset "build-unixlike-gcc-release" --parallel

# Build debug with clang (and mold) toolchain
clang-debug:
    cmake --preset "unixlike-clang-debug" -S {{root_dir}}
    cmake --build --preset "build-unixlike-clang-debug" --parallel

# Build release with clang (and mold) toolchain
clang-release:
    cmake --preset "unixlike-clang-release" -S {{root_dir}}
    cmake --build --preset "build-unixlike-clang-release" --parallel

# run RELEASE benchmarks as tests
benchmark: release
    ctest --preset "unixlike-gcc-release" -R "\.*_benchmarks"

# reformat code
reformat: has_clang_format
    find {{root_dir}}/apps -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i
    find {{root_dir}}/libs -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i

# clean debug build
clean:
    cmake --preset "unixlike-gcc-debug" --target clean
    cmake --preset "unixlike-clang-debug" --target clean

clean-release:
    cmake --preset "unixlike-gcc-release" --target clean
    cmake --preset "unixlike-clang-release" --target clean

deep-clean:
    rm -r buildDir
    git clean -dfX subprojects/

[private]
has_clang_format:
    @which clang-format > /dev/null 2>&1

# convenience for working with the venv
[private]
venv:
    python3 -m venv {{docs_dir}}/.venv

# build the docs
docs:
    {{scripts_dir}}/docs.bash build

# run an interactive web server for the docs
docs-serve:
    {{scripts_dir}}/docs.bash

all: test docs
    @echo "TODO:  deploy code, tags, etc."