find_package(functions_framework_cpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)

# Static library containing all app logic — linked by both the main binary and tests.
add_library(app_lib STATIC
  router.cc
  services/greeting_service.cc
)
target_include_directories(app_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app_lib PUBLIC
  functions-framework-cpp::framework
  spdlog::spdlog
  nlohmann_json::nlohmann_json
  Threads::Threads
)

# Main executable
add_executable(app main.cc)
target_link_libraries(app PRIVATE app_lib)
