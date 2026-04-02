#include <spdlog/spdlog.h>

#include "config.h"
#include "handlers/health_handler.h"
#include "handlers/hello_handler.h"
#include "router.h"
#include "services/greeting_service.h"

namespace gcf = ::google::cloud::functions;

int main(int argc, char* argv[]) {
  app::Config config;
  spdlog::set_level(spdlog::level::from_str(config.log_level));
  spdlog::info("Starting server (target={}, port={})", config.target, config.port);

  app::GreetingService greeting_service(config.target);

  app::Router router;
  router.get("/health", app::handlers::health);
  router.get("/hello", [&greeting_service](gcf::HttpRequest const& req) {
    return app::handlers::hello(req, greeting_service);
  });

  auto handler = gcf::MakeFunction(
      [&router](gcf::HttpRequest const& request) { return router.handle(request); });

  return gcf::Run(argc, argv, std::move(handler));
}
