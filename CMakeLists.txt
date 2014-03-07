cmake_minimum_required(VERSION 2.8.8)

project(libdlibxx)

add_definitions("-std=c++11")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

include_directories(lib/include)

add_subdirectory(lib)
add_subdirectory(examples)

