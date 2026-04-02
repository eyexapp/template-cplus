---
name: testing
type: knowledge
version: 1.0.0
agent: CodeActAgent
triggers:
  - test
  - gtest
  - google test
  - mock
  - unit test
---

# Testing — C++20 (GoogleTest + GoogleMock)

## Unit Tests

```cpp
#include <gtest/gtest.h>
#include "services/user_service.h"

class UserServiceTest : public ::testing::Test {
protected:
    MockUserRepository mock_repo;
    UserService service{mock_repo};
};

TEST_F(UserServiceTest, CreateUserSuccess) {
    CreateDTO dto{"Alice", "alice@test.com"};
    EXPECT_CALL(mock_repo, save(testing::_))
        .WillOnce(testing::Return(User{"id-1", "Alice", "alice@test.com"}));

    auto user = service.create(dto);
    EXPECT_EQ(user.name, "Alice");
    EXPECT_EQ(user.email, "alice@test.com");
}

TEST_F(UserServiceTest, CreateUserDuplicateThrows) {
    EXPECT_CALL(mock_repo, find_by_email("a@b.com"))
        .WillOnce(testing::Return(std::optional<User>{mock_user}));

    EXPECT_THROW(service.create(CreateDTO{"Bob", "a@b.com"}), AppError);
}
```

## HTTP Handler Testing

```cpp
TEST(UserHandlerTest, GetUserReturns200) {
    MockUserService mock_service;
    UserHandler handler(mock_service);

    EXPECT_CALL(mock_service, find_by_id("123"))
        .WillOnce(testing::Return(std::optional<User>{mock_user}));

    auto request = make_test_request("GET", "/users/123");
    auto response = handler.handle_get(request);

    EXPECT_EQ(response.result_int(), 200);
}
```

## GoogleMock

```cpp
class MockUserRepository : public UserRepository {
public:
    MOCK_METHOD(std::optional<User>, find_by_id, (const std::string&), (const, override));
    MOCK_METHOD(User, save, (const CreateDTO&), (override));
    MOCK_METHOD(std::optional<User>, find_by_email, (const std::string&), (const, override));
};
```

## CMake Test Config

```cmake
enable_testing()
find_package(GTest REQUIRED)

add_executable(unit_tests tests/unit/test_user_service.cpp)
target_link_libraries(unit_tests PRIVATE GTest::gtest_main GTest::gmock)
add_test(NAME unit_tests COMMAND unit_tests)
```

## Rules

- `TEST_F` for tests with shared fixtures.
- `EXPECT_*` (continues on failure) over `ASSERT_*` (aborts).
- GoogleMock for interface mocking — `MOCK_METHOD`.
- `ctest` or `./unit_tests` to run.
