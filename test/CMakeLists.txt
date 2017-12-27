project (libdlibxx_test LANGUAGES CXX)

add_executable(
  ${PROJECT_NAME}

  ${PROJECT_SOURCE_DIR}/test_library_loading.cpp
  ${PROJECT_SOURCE_DIR}/test_symbol_loading.cpp
  ${PROJECT_SOURCE_DIR}/mocks/dlfcn_mock.cpp
)

target_include_directories(
  ${PROJECT_NAME}

  SYSTEM PUBLIC
  ${PROJECT_SOURCE_DIR}
  gmock
)

target_link_libraries(
  ${PROJECT_NAME}

  LINK_PRIVATE
  dlibxx
  gtest
  gmock
  gmock_main
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_BINARY_DIR}/${PROJECT_NAME})
