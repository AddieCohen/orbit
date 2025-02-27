# Copyright (c) 2021 The Orbit Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

cmake_minimum_required(VERSION 3.15)

project(CrashHandler)

add_library(CrashHandler STATIC)

target_include_directories(CrashHandler PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
                                        PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_sources(CrashHandler PUBLIC include/CrashHandler/CrashHandler.h
                                   include/CrashHandler/CrashOptions.h
                            PRIVATE CrashHandler.cpp)
target_link_libraries(CrashHandler PUBLIC CONAN_PKG::crashpad
                                   PRIVATE OrbitBase
                                           OrbitVersion)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CrashOptions.cpp.in
               ${CMAKE_CURRENT_BINARY_DIR}/CrashOptions.cpp)
target_sources(CrashHandler PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/CrashOptions.cpp")
