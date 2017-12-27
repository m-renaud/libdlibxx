#include "mocks/dlfcn_mock.h"

#include <functional>

// The following function objects are assigned in the DlfcnMock constructor.
// They are used to delegate to the mock object.
namespace {
std::function<void*(char const*, int)> _dlopen;
std::function<int(void*)> _dlclose;
std::function<void*(void*, char const*)> _dlsym;
std::function<char*(void)> _dlerror;
}  // namespace

// Override the dlopen, dlclose, dlsym, and dlerror functions such that they
// delegate to the above function objects. This allows dlibxx to use the normal
// free functions and affording us the ability to control what the functions
// will do. Finally, they need C linkage to avoid name-mangling by the
// compiler.
extern "C" {
void* dlopen(char const* filename, int flags) {
  return _dlopen(filename, flags);
}

int dlclose(void* handle) { return _dlclose(handle); }

void* dlsym(void* handle, char const* symbol) { return _dlsym(handle, symbol); }

char* dlerror(void) { return nullptr; }
}

DlfcnMock::DlfcnMock() {
  assert(!_dlopen);
  assert(!_dlclose);
  assert(!_dlsym);
  assert(!_dlerror);

  _dlopen = [this](char const* filename, int flags) {
    return dlopen(filename, flags);
  };

  _dlclose = [this](void* handle) { return dlclose(handle); };

  _dlsym = [this](void* handle, char const* symbol) {
    return dlsym(handle, symbol);
  };

  _dlerror = [this] { return _dlerror(); };
}

DlfcnMock::~DlfcnMock() {
  _dlopen = {};
  _dlclose = {};
  _dlsym = {};
  _dlerror = {};
}
