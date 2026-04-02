#pragma once

#include <string>

namespace app {

/// Pure business logic — generates greeting messages.
class GreetingService {
 public:
  explicit GreetingService(std::string default_target);

  std::string greet(const std::string& name) const;

 private:
  std::string default_target_;
};

}  // namespace app
