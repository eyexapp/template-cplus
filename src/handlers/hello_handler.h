#pragma once

#include <string>

#include <google/cloud/functions/framework.h>
#include <nlohmann/json.hpp>

#include "../services/greeting_service.h"

namespace gcf = ::google::cloud::functions;

namespace app::handlers {

/// GET /hello?name=...  → {"greeting": "Hello ..."}
inline gcf::HttpResponse hello(gcf::HttpRequest const& request,
                               GreetingService const& service) {
  // Parse "?name=value" from the target string.
  std::string name;
  auto target = std::string(request.target());
  auto qpos = target.find("?name=");
  if (qpos != std::string::npos) {
    name = target.substr(qpos + 6);
    // Strip any further query params.
    auto amp = name.find('&');
    if (amp != std::string::npos) {
      name = name.substr(0, amp);
    }
  }

  nlohmann::json body = {{"greeting", service.greet(name)}};

  return gcf::HttpResponse{}
      .set_header("Content-Type", "application/json")
      .set_payload(body.dump());
}

}  // namespace app::handlers
