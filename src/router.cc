#include "router.h"

#include <spdlog/spdlog.h>

namespace app {

void Router::get(const std::string& path, Handler handler) {
  routes_[key("GET", path)] = std::move(handler);
}

void Router::post(const std::string& path, Handler handler) {
  routes_[key("POST", path)] = std::move(handler);
}

gcf::HttpResponse Router::handle(gcf::HttpRequest const& request) const {
  // Strip query string from target for route matching.
  auto target = std::string(request.target());
  auto qpos = target.find('?');
  if (qpos != std::string::npos) {
    target = target.substr(0, qpos);
  }

  auto route_key = key(std::string(request.verb()), target);
  auto it = routes_.find(route_key);

  if (it != routes_.end()) {
    spdlog::info("{} {}", request.verb(), request.target());
    return it->second(request);
  }

  spdlog::warn("404 {} {}", request.verb(), request.target());
  return gcf::HttpResponse{}
      .set_result(404)
      .set_header("Content-Type", "application/json")
      .set_payload(R"({"error":"not found"})");
}

std::string Router::key(const std::string& method, const std::string& path) {
  return method + " " + path;
}

}  // namespace app
