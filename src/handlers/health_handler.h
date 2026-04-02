#pragma once

#include <google/cloud/functions/framework.h>
#include <nlohmann/json.hpp>

namespace gcf = ::google::cloud::functions;

namespace app::handlers {

/// GET /health → {"status": "ok"}
inline gcf::HttpResponse health(gcf::HttpRequest const& /*request*/) {
  nlohmann::json body = {{"status", "ok"}};

  return gcf::HttpResponse{}
      .set_header("Content-Type", "application/json")
      .set_payload(body.dump());
}

}  // namespace app::handlers
