cmake_minimum_required(VERSION 3.0)
project(HelloPro)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_FLAGS "-std=c17 -Wall -O0")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -O0")
set(INCDIR "include")

file(GLOB SOURCES src/*.cc include/*.hh)
add_executable(hello_pro ${SOURCES})
target_include_directories(hello_pro PRIVATE include)
