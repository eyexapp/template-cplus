#include <gtest/gtest.h>

#include <google/cloud/functions/framework.h>

#include "router.h"

namespace gcf = ::google::cloud::functions;

namespace {

gcf::HttpRequest make_request(const std::string& method, const std::string& target) {
  return gcf::HttpRequest{}.set_verb(method).set_target(target);
}

}  // namespace

TEST(RouterTest, MatchesRegisteredRoute) {
  app::Router router;
  router.get("/ping", [](gcf::HttpRequest const& /*req*/) {
    return gcf::HttpResponse{}.set_result(200).set_payload("pong");
  });

  auto response = router.handle(make_request("GET", "/ping"));
  EXPECT_EQ(response.result(), 200);
  EXPECT_EQ(response.payload(), "pong");
}

TEST(RouterTest, Returns404ForUnknownPath) {
  app::Router router;
  auto response = router.handle(make_request("GET", "/unknown"));
  EXPECT_EQ(response.result(), 404);
}

TEST(RouterTest, Returns404ForMethodMismatch) {
  app::Router router;
  router.get("/only-get", [](gcf::HttpRequest const& /*req*/) {
    return gcf::HttpResponse{}.set_result(200);
  });

  auto response = router.handle(make_request("POST", "/only-get"));
  EXPECT_EQ(response.result(), 404);
}

TEST(RouterTest, StripsQueryStringBeforeMatching) {
  app::Router router;
  router.get("/search", [](gcf::HttpRequest const& /*req*/) {
    return gcf::HttpResponse{}.set_result(200).set_payload("found");
  });

  auto response = router.handle(make_request("GET", "/search?q=hello"));
  EXPECT_EQ(response.result(), 200);
  EXPECT_EQ(response.payload(), "found");
}
