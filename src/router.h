#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include <google/cloud/functions/framework.h>

namespace gcf = ::google::cloud::functions;

namespace app {

using Handler = std::function<gcf::HttpResponse(gcf::HttpRequest const&)>;

/// Simple path + method router for the Cloud Functions Framework.
/// Register handlers with get()/post(), then call handle() from the
/// single gcf::MakeFunction entry point.
class Router {
 public:
  void get(const std::string& path, Handler handler);
  void post(const std::string& path, Handler handler);

  gcf::HttpResponse handle(gcf::HttpRequest const& request) const;

 private:
  /// Key: "METHOD /path"
  std::unordered_map<std::string, Handler> routes_;

  static std::string key(const std::string& method, const std::string& path);
};

}  // namespace app
