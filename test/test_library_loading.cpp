#include "dlibxx.hxx"

#include <gmock/gmock.h>
#include "mocks/dlfcn_mock.h"

#include <tuple>

using namespace testing;

class TestLoadLibrary : public Test {
 public:
  TestLoadLibrary() : Test{} {
    ON_CALL(dlfcn, dlopen(_, _))
        .WillByDefault(Return(reinterpret_cast<void*>(1)));
    ON_CALL(dlfcn, dlclose(_)).WillByDefault(Return(0));
  }

 protected:
  NiceMock<DlfcnMock> dlfcn;
  dlibxx::handle lib;
};

TEST_F(TestLoadLibrary, Loaded_WhenLibraryCannotBeFound_WillReturnFalse) {
  auto const lib_name = std::string{"test"};
  EXPECT_CALL(dlfcn, dlopen(StrEq(lib_name), _))
      .WillRepeatedly(Return(nullptr));

  lib.load(lib_name);

  EXPECT_FALSE(lib.loaded());
}

TEST_F(TestLoadLibrary, Loaded_WhenLibraryIsFound_WillReturnTrue) {
  auto const lib_name = std::string{"test"};
  EXPECT_CALL(dlfcn, dlopen(StrEq(lib_name), _))
      .WillRepeatedly(Return(reinterpret_cast<void*>(1)));

  lib.load(lib_name);

  EXPECT_TRUE(lib.loaded());
}

TEST_F(TestLoadLibrary, OperatorBool_WhenLibraryCannotBeFound_WillReturnFalse) {
  auto const lib_name = std::string{"test"};
  EXPECT_CALL(dlfcn, dlopen(StrEq(lib_name), _))
      .WillRepeatedly(Return(nullptr));

  lib.load(lib_name);

  EXPECT_FALSE(lib);
}

TEST_F(TestLoadLibrary, OperatorBool_WhenLibraryIsFound_WillReturnTrue) {
  auto const lib_name = std::string{"test"};
  EXPECT_CALL(dlfcn, dlopen(StrEq(lib_name), _))
      .WillRepeatedly(Return(reinterpret_cast<void*>(1)));

  lib.load(lib_name);

  EXPECT_TRUE(lib);
}

class TestDlopenFlags
    : public TestLoadLibrary,
      public WithParamInterface<std::tuple<dlibxx::resolve, dlibxx::options>> {
};

TEST_P(TestDlopenFlags, Load_WillCorrectlyPassFlagsToDlopen) {
  dlibxx::resolve const resolve_policy = std::get<0>(GetParam());
  dlibxx::options const resolve_options = std::get<1>(GetParam());

  auto const expected_flags =
      static_cast<int>(resolve_policy) | static_cast<int>(resolve_options);

  lib.resolve_policy(resolve_policy);
  lib.set_options(resolve_options);

  EXPECT_CALL(dlfcn, dlopen(_, expected_flags));
  lib.load("test");
}

#include <vector>
std::vector<dlibxx::options> combine_resolve_options() {
  std::vector<dlibxx::options> result = {};
  result.push_back(dlibxx::options::none);

  for (auto scope : {dlibxx::options::global, dlibxx::options::local}) {
    result.push_back(scope);
    result.push_back(scope | dlibxx::options::no_delete);
    result.push_back(scope | dlibxx::options::no_load);
    result.push_back(scope | dlibxx::options::deep_bind);
    result.push_back(scope | dlibxx::options::no_delete |
                     dlibxx::options::no_load);
    result.push_back(scope | dlibxx::options::no_delete |
                     dlibxx::options::deep_bind);
    result.push_back(scope | dlibxx::options::no_delete |
                     dlibxx::options::no_load | dlibxx::options::deep_bind);
    result.push_back(scope | dlibxx::options::no_load |
                     dlibxx::options::deep_bind);
  }

  return result;
}

INSTANTIATE_TEST_CASE_P(, TestDlopenFlags,
                        Combine(Values(dlibxx::resolve::lazy,
                                       dlibxx::resolve::now),
                                ValuesIn(combine_resolve_options())));

class TestClose : public Test {};

TEST_F(TestClose, Close_WhenHandleIsValid_WillClose) {
  NiceMock<DlfcnMock> dlfcn;
  ON_CALL(dlfcn, dlopen(_, _))
      .WillByDefault(Return(reinterpret_cast<void*>(1)));
  ON_CALL(dlfcn, dlclose(_)).WillByDefault(Return(0));

  dlibxx::handle lib{"test"};

  EXPECT_CALL(dlfcn, dlclose(_));
  lib.close();
}

TEST_F(TestClose, Close_WhenHandleIsValid_WillInvalidateHandle) {
  NiceMock<DlfcnMock> dlfcn;
  ON_CALL(dlfcn, dlopen(_, _))
      .WillByDefault(Return(reinterpret_cast<void*>(1)));
  ON_CALL(dlfcn, dlclose(_)).WillByDefault(Return(0));

  dlibxx::handle lib{"test"};

  lib.close();
  EXPECT_FALSE(lib);
}

TEST_F(TestClose, Close_WhenHandleInvalid_WillNotClose) {
  NiceMock<DlfcnMock> dlfcn;
  ON_CALL(dlfcn, dlopen(_, _)).WillByDefault(Return(nullptr));
  ON_CALL(dlfcn, dlclose(_)).WillByDefault(Return(0));

  dlibxx::handle lib{"test"};

  EXPECT_CALL(dlfcn, dlclose(_)).Times(0);
  lib.close();
}

TEST_F(TestClose, Destructor_WhenHandleIsValid_WillClose) {
  NiceMock<DlfcnMock> dlfcn;
  ON_CALL(dlfcn, dlopen(_, _))
      .WillByDefault(Return(reinterpret_cast<void*>(1)));
  ON_CALL(dlfcn, dlclose(_)).WillByDefault(Return(0));

  dlibxx::handle lib{"test"};

  EXPECT_CALL(dlfcn, dlclose(_));
}

TEST_F(TestClose, Destructor_WhenHandleInvalid_WillNotClose) {
  NiceMock<DlfcnMock> dlfcn;
  ON_CALL(dlfcn, dlopen(_, _)).WillByDefault(Return(nullptr));
  ON_CALL(dlfcn, dlclose(_)).WillByDefault(Return(0));

  dlibxx::handle lib{"test"};

  EXPECT_CALL(dlfcn, dlclose(_)).Times(0);
}
