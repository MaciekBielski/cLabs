cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_FLAGS "-std=c17 -Wall -O0")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -O0")

project(hello_pro)
# include_directories(include)

file(GLOB SOURCES src/*.cpp)

add_executable(hello_pro ${SOURCES})
