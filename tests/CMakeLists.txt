# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.


cmake_minimum_required(VERSION 3.16...3.26)
project(tests)
set(CMAKE_VERBOSE_MAKEFILE on)

include(FetchContent)
include(GoogleTest)

set(YOGA_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
include(${YOGA_ROOT}/cmake/project-defaults.cmake)

# Fetch GTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(${YOGA_ROOT}/yoga ${CMAKE_CURRENT_BINARY_DIR}/yoga)

file(GLOB SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp)

enable_testing()

add_executable(yogatests ${SOURCES})
target_link_libraries(yogatests yogacore GTest::gtest_main)

gtest_discover_tests(yogatests)
