#include "dlibxx.hxx"

#include <gmock/gmock.h>
#include "mocks/dlfcn_mock.h"

using namespace testing;

void load_me(int a, char b) {}

class TestLoadSymbol : public Test {
 public:
  TestLoadSymbol() : Test{} {
    ON_CALL(dlfcn, dlopen(_, _))
        .WillByDefault(Return(reinterpret_cast<void*>(1)));
    ON_CALL(dlfcn, dlclose(_)).WillByDefault(Return(0));
  }

 protected:
  NiceMock<DlfcnMock> dlfcn;
  dlibxx::handle lib;
};

TEST_F(TestLoadSymbol, Lookup_WhenHandleInvalid_WillNotLoadSymbols) {
  EXPECT_CALL(dlfcn, dlsym(_, _)).Times(0);
  lib.lookup<void(int, char)>("load_me");
}

TEST_F(TestLoadSymbol, Lookup_WhenHandleIsValid_WillLoadSymbol) {
  lib.load("test");

  EXPECT_CALL(dlfcn, dlsym(_, _));
  lib.lookup<void(int, char)>("load_me");
}

TEST_F(TestLoadSymbol, Lookup_WhenSymbolExists_WillReturnValidOptional) {
  lib.load("test");

  EXPECT_CALL(dlfcn, dlsym(_, _)).WillRepeatedly(Return((void*)&load_me));

  auto function = lib.lookup<void(int, char)>("load_me");

  EXPECT_TRUE(function);
}

TEST_F(TestLoadSymbol, Lookup_WhenOptionalIsValid_FunctionWillBeCallable) {
  lib.load("test");

  EXPECT_CALL(dlfcn, dlsym(_, _)).WillRepeatedly(Return((void*)&load_me));

  auto function = lib.lookup<void(int, char)>("load_me");

  EXPECT_NO_THROW(function.value()(int{}, char{}));
}

TEST_F(TestLoadSymbol, Lookup_WhenSymbolNotFound_WillReturnEmptyOptional) {
  lib.load("test");

  auto error = "Symbol not found";
  ON_CALL(dlfcn, dlerror()).WillByDefault(Return(const_cast<char*>(error)));

  auto function = lib.lookup<void(int, char)>("load_me");

  EXPECT_THROW(function.value()(int{}, char{}), std::bad_function_call);
}

class TestFunctionPtrCast : public Test {};

TEST_F(TestFunctionPtrCast,
       FptrCast_WhenGivenValidPointer_WillConvertToCallableFunction) {
  auto result = dlibxx::util::fptr_cast<void, int, char>((void*)&load_me);
  EXPECT_TRUE(result);
}

TEST_F(TestFunctionPtrCast,
       FptrCast_WhenGivenNullPointer_WillNotConvertToCallableFunction) {
  auto result = dlibxx::util::fptr_cast<void, int, char>(nullptr);
  EXPECT_FALSE(result);
}

TEST_F(TestFunctionPtrCast,
       FptrCast_WhenFunctionIsCallable_WillNotThrowException) {
  auto result = dlibxx::util::fptr_cast<void, int, char>((void*)&load_me);
  EXPECT_NO_THROW(result(int{}, char{}));
}
