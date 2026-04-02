#pragma once

#include <cstdlib>
#include <string>

namespace app {

/// Reads configuration from environment variables with sensible defaults.
struct Config {
  std::string target = env("TARGET", "World");
  std::string port = env("PORT", "8080");
  std::string log_level = env("LOG_LEVEL", "info");

 private:
  static std::string env(const char* name, const char* fallback) {
    const auto* value = std::getenv(name);
    return value != nullptr ? value : fallback;
  }
};

}  // namespace app
