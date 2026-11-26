## Building Yoga native lib for Java
Instruction how to build Yoga native lib on local MacOs/Linux machine.

### Prerequisites
Required tools:
* ninja
* cmake
* g++

### Steps

1. Modify `cmake/project-defaults.cmake` file by commenting out `-Werror`:

    ```diff
    diff --git a/cmake/project-defaults.cmake b/cmake/project-defaults.cmake
    index 987529c..c28d542 100644
    --- a/cmake/project-defaults.cmake
    +++ b/cmake/project-defaults.cmake
    @@ -33,7 +33,7 @@ add_compile_options(
         -fexceptions
         # Enable warnings and warnings as errors
         -Wall
    -    -Werror
    +    #-Werror
         # Disable RTTI
         $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
         # Use -O2 (prioritize speed)
    ```
2. Modify `java/CMakeLists.txt` file:

    ```diff
    diff --git a/java/CMakeLists.txt b/java/CMakeLists.txt
    index d8afd67..185a6fb 100644
    --- a/java/CMakeLists.txt
    +++ b/java/CMakeLists.txt
    @@ -5,8 +5,13 @@
     
     cmake_minimum_required(VERSION 3.13...3.26)
     project(yogajni)
    +find_package(JNI REQUIRED)
    +include_directories(${JNI_INCLUDE_DIRS})
     set(CMAKE_VERBOSE_MAKEFILE on)
     
    +set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR})
    +message(CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    +
     set(YOGA_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
     include(${YOGA_ROOT}/cmake/project-defaults.cmake)
     
    @@ -18,5 +23,9 @@ file(GLOB VERSION_SCRIPT CONFIGURE_DEPENDS
         ${CMAKE_CURRENT_SOURCE_DIR}/yogajni.version)
     
     add_library(yoga SHARED ${SOURCES})
    +
    +# TODO export lib with .so extension on MacOS with Apple Silicon
    +#set_target_properties(yoga PROPERTIES SUFFIX ".so")
    +
     target_link_libraries(yoga yogacore)
    -target_link_options(yoga PRIVATE -Wl,--version-script=${VERSION_SCRIPT})
    +target_link_options(yoga PRIVATE "-Wl")
    ```

3. Create a `./build_java` build file.

    ```bash
    #!/bin/sh
    # Copyright (c) Meta Platforms, Inc. and affiliates.
    #
    # This source code is licensed under the MIT license found in the
    # LICENSE file in the root directory of this source tree.
    
    cd "$(dirname "$0")/java"
    rm -rf build
    
    if [ "$#" -eq 0 ]; then
      build_type="Debug"
    else
      build_type="$1"
    fi
    
    if which ninja; then
      set -e
      cmake -B build -S . -D CMAKE_OSX_ARCHITECTURES=arm64 -D CMAKE_BUILD_TYPE="$build_type" -G Ninja
    else
      set -e
      cmake -B build -S . -D CMAKE_BUILD_TYPE="$build_type"
    fi
    
    cmake --build build
    
    find . \( -name "*.so" -o -name "*.dylib" \)
    
    ```
4. Run `./build_java` from the terminal.

    MacOS systems will create a `.dylib`, whereas Linux an `.so` version of the library. 
    There's possiblity to build yoga lib for two architectures (only for MacOs), just update `CMAKE_OSX_ARCHITECTURES` flag to: `CMAKE_OSX_ARCHITECTURES:STRING="x86_64;arm64"`.

6. Artifact location will be printed in output. Output may contain several warnings - which are a non-issue. If there's a library location printed out at the end, the build was successful.
