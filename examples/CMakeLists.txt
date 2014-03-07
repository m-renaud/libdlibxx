cmake_minimum_required(VERSION 2.8.8)

add_executable(demo simple-dl-demo.cxx)
target_link_libraries(demo dlibxx)

add_executable(driver driver.cxx)
target_link_libraries(driver dlibxx)

add_library(a SHARED plugin-a.cxx)
add_library(b SHARED plugin-b.cxx)
add_library(c SHARED plugin-c.cxx)