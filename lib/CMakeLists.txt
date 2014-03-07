cmake_minimum_required(VERSION 2.8.8)

add_library(
  dlibxx SHARED
  "${PROJECT_SOURCE_DIR}/lib/src/dlibxx.cxx"
)

target_link_libraries(dlibxx dl)

set_target_properties(
  dlibxx PROPERTIES
  SOVERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
)

install(TARGETS dlibxx DESTINATION lib)
install(FILES "${PROJECT_SOURCE_DIR}/lib/include/dlibxx.hxx" DESTINATION include)