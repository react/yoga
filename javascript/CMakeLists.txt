# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.13...3.26)
set(CMAKE_VERBOSE_MAKEFILE on)
project(yoga)

file(GLOB SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../yoga/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../yoga/**/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

include_directories(..)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(
    EMSCRIPTEN_HAS_UNBOUND_TYPE_NAMES=0)

set(COMPILE_OPTIONS
    -flto
    -fno-exceptions
    -fno-rtti
    -g0
    -Os
    "SHELL:-s STRICT=1")

add_compile_options(${COMPILE_OPTIONS})

add_link_options(
    ${COMPILE_OPTIONS}
    "SHELL:--closure 1"
    "SHELL:--memory-init-file 0"
    "SHELL:--no-entry"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s ASSERTIONS=0"
    "SHELL:-s DYNAMIC_EXECUTION=0"
    "SHELL:-s EXPORT_NAME='loadYoga'"
    "SHELL:-s FETCH_SUPPORT_INDEXEDDB=0"
    "SHELL:-s FILESYSTEM=0"
    "SHELL:-s MALLOC='emmalloc'"
    "SHELL:-s MODULARIZE=1"
    "SHELL:-s EXPORT_ES6=1"
    "SHELL:-s WASM=1"
    "SHELL:-s TEXTDECODER=0"
    "SHELL:-s SUPPORT_BIG_ENDIAN=1"
    # SINGLE_FILE=1 combined with ENVIRONMENT='web' creates code that works on
    # both bundlders and Node.
    "SHELL:-s SINGLE_FILE=1"
    "SHELL:-s ENVIRONMENT='web'")

link_libraries(embind)

add_library(yogaObjLib OBJECT ${SOURCES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/binaries)

add_executable(yoga-wasm-base64-esm $<TARGET_OBJECTS:yogaObjLib>)
