#include "greeting_service.h"

namespace app {

GreetingService::GreetingService(std::string default_target)
    : default_target_(std::move(default_target)) {}

std::string GreetingService::greet(const std::string& name) const {
  const auto& target = name.empty() ? default_target_ : name;
  return "Hello " + target;
}

}  // namespace app
