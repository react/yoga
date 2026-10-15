# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.13...3.26)
project(yoga-all)
set(CMAKE_VERBOSE_MAKEFILE on)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/project-defaults.cmake)

add_subdirectory(yoga)
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

option(BUILD_FUZZ_TESTS "Build fuzz tests" OFF)

if ('${CMAKE_CXX_COMPILER_ID}' MATCHES 'Clang' AND  BUILD_FUZZ_TESTS)
    add_subdirectory(fuzz)
endif()

# cmake install config
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# declare target to install
install(TARGETS yogacore EXPORT yoga-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# install header files
install(DIRECTORY
    "${CMAKE_CURRENT_LIST_DIR}/yoga"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# install target config
install(EXPORT yoga-targets
    FILE yogaTargets.cmake
    NAMESPACE yoga::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yoga
)


# generate config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/yoga-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/yogaConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yoga
)

# install config file
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/yogaConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yoga
)
