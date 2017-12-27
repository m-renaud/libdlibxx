#pragma once

#include <dlfcn.h>

#include <gmock/gmock.h>

class DlfcnMock {
 public:
  DlfcnMock();
  ~DlfcnMock();
  MOCK_METHOD2(dlopen, void*(char const* filename, int flags));
  MOCK_METHOD1(dlclose, int(void* handle));
  MOCK_METHOD2(dlsym, void*(void* handle, char const* symbol));
  MOCK_METHOD0(dlerror, char*(void));
};
