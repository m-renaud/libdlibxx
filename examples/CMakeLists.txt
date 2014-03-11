cmake_minimum_required(VERSION 2.8.8)

add_executable(demo demo.cxx)
target_link_libraries(demo dlibxx)

add_library(a SHARED plugin-a.cxx)
add_library(b SHARED plugin-b.cxx)
add_library(c SHARED plugin-c.cxx)