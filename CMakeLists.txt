cmake_minimum_required(VERSION 2.8.8)

project(libdlibxx)

add_definitions("-std=c++11")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

option(DLIBXX_ENABLE_TESTS OFF "Enable testing")

include_directories(include)

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
### Find dependencies
## We require version 1.58.0 or later for the `value` function in
## `boost::optional` that allows us to interchange with `std::optional`
find_package (Boost 1.58.0 REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
### Build libdlibxx
add_library(
  dlibxx SHARED
  "${PROJECT_SOURCE_DIR}/src/dlibxx.cxx"
)

target_link_libraries(dlibxx dl)

set_target_properties(
  dlibxx PROPERTIES
  SOVERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
)

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
### Install Rules
install(TARGETS dlibxx DESTINATION "lib")
install(FILES "${PROJECT_SOURCE_DIR}/include/dlibxx.hxx" DESTINATION "include")
install(FILES "${PROJECT_SOURCE_DIR}/LICENSE" DESTINATION "share/licenses/${PROJECT_NAME}")


# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
### Examples
add_subdirectory(examples)

if (DLIBXX_ENABLE_TESTS)
  enable_testing()
  add_subdirectory(external/googletest EXCLUDE_FROM_ALL)
  add_subdirectory(test)
endif()
