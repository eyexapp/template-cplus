#include <gtest/gtest.h>

#include "services/greeting_service.h"

using app::GreetingService;

TEST(GreetingServiceTest, GreetsWithGivenName) {
  GreetingService service("World");
  EXPECT_EQ(service.greet("Alice"), "Hello Alice");
}

TEST(GreetingServiceTest, UsesDefaultTargetWhenNameIsEmpty) {
  GreetingService service("DefaultTarget");
  EXPECT_EQ(service.greet(""), "Hello DefaultTarget");
}

TEST(GreetingServiceTest, HandlesLongNames) {
  GreetingService service("World");
  std::string long_name(200, 'x');
  EXPECT_EQ(service.greet(long_name), "Hello " + long_name);
}
