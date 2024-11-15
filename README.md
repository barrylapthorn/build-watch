# Build Watch

MIT Licensed.

## What is it?

Watches for changes in your source tree, and writes out new versions of `CMakeLists.txt` 
(by default), using Mustache as the template engine. 

Tries to parse `.gitignore` file (or rather, the root one), and ignore any directories therein.


## Getting Started

In your *template* file, you'll want to add this to `CMakeLists.txt.mustache`

```cmake
# elided...

add_library( MY-LIBRARY-OR-EXE
{{#files}}
    {{relpath}}
{{/files}}
)

# elided...
```

which will write out `CMakeLists.txt` when you add/modify/delete any source files that match your
configuration.

You probably want to commit both files.

A default config file can be generated via:

```shell
build-watch -g
```

will print a default config to `stdout` (for use with CMake by default).

Copy that to `<your root>/.config/BuildWatch/config.json`.

Then, finally:

```shell
cd <your root>
build-watch

# or

build-watch <your-root>
```

> Note: you *really* should run this at the root level (same as your `.git` folder).


An example can be seen here [`apps/build-watch/src`](apps/build-watch/src).


## Developing


### Prerequisites

- cmake v3.30+ (probably 3.25 is fine)
- [`just`](https://github.com/casey/just)
- gcc 14 or clang
- (optional) `mold` for faster linking (via cmake presets)


### Building

It's cmake and there are a bunch of presets you can use.  Or just use the `just` tasks:

```shell
just release
```

and you're done.

If you like long hand, then:

```shell
    cmake --preset "unixlike-gcc-debug" -S {{root_dir}}
    cmake --build --preset "build-unixlike-gcc-debug" --parallel
    ctest --preset "test-unixlike-gcc-debug"
```

We leverage `cmake` presets, and everything is built to `out/`.  

Other presets are available in [`CMakePresets.json`](CMakePresets.json).


### Testing

There's a `just` target called `dogfood` that builds and runs it at the root of this repo.


## Packaging

```shell
just package
```

will create a `tar.gz` release build package.


## Advanced

Bazel example:

```
cc_binary(
    name = "build-watch",
    srcs = [
    {{#files}}
        "{{relpath}}"{{^last}}, {{/last}}
    {{/files}}
    ],
)
```

The `last` field is `true` if the element is the last in the list.  Using mustache inverted sections
therefore means we write a comma for all cases except the last.

Associated config:

```json
{
  "files": [
    {
      "src": "CMakeLists.txt.mustache",
      "dest": "CMakeLists.txt",
      "extensions": [
        ".hpp",
        ".cpp",
        ".h"
      ]
    },
    {
      "src": "BUILD.mustache",
      "dest": "BUILD",
      "extensions": [
        ".hpp",
        ".cpp",
        ".h"
      ]
    }
  ],
  "ignoreFiles": [
    ".gitignore"
  ]
}
```