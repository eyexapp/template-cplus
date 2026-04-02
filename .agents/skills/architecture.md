---
name: architecture
type: knowledge
version: 1.0.0
agent: CodeActAgent
triggers:
  - architecture
  - cmake
  - handler
  - service
  - cloud functions
  - vcpkg
---

# Architecture — C++20 (Google Cloud Functions Framework)

## Cloud Functions Framework

```cpp
#include <google/cloud/functions/framework.h>
namespace gcf = google::cloud::functions;

gcf::HttpResponse handle_request(gcf::HttpRequest request) {
    auto handler = HandlerFactory::create(request.verb(), request.target());
    return handler->execute(request);
}

int main(int argc, char* argv[]) {
    return gcf::Run(argc, argv, handle_request);
}
```

## Project Structure

```
├── CMakeLists.txt         ← Build config (CMake + vcpkg)
├── vcpkg.json             ← Dependency manifest
├── src/
│   ├── main.cpp           ← Entry point (Cloud Functions)
│   ├── handlers/          ← HTTP request handlers
│   │   ├── user_handler.h
│   │   ├── user_handler.cpp
│   │   └── handler_factory.h
│   ├── services/          ← Business logic
│   │   ├── user_service.h
│   │   └── user_service.cpp
│   ├── core/              ← Domain models, errors
│   │   ├── models/
│   │   │   └── user.h
│   │   └── errors.h
│   └── utils/             ← JSON helpers, string utils
│       └── json_utils.h
├── tests/
│   ├── unit/
│   └── integration/
└── .github/workflows/
```

## Handler → Service → Core Pattern

```cpp
// handlers/user_handler.cpp
class UserHandler {
    UserService& service_;
public:
    explicit UserHandler(UserService& service) : service_(service) {}

    gcf::HttpResponse handle_get(const gcf::HttpRequest& req) {
        auto id = extract_id(req.target());
        auto user = service_.find_by_id(id);
        if (!user) return not_found("User");
        return json_response(200, user->to_json());
    }

    gcf::HttpResponse handle_post(const gcf::HttpRequest& req) {
        auto dto = CreateUserDTO::from_json(req.payload());
        auto user = service_.create(dto);
        return json_response(201, user.to_json());
    }
};
```

## CMake Build

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_function CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(functions_framework_cpp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

add_executable(my_function src/main.cpp src/handlers/user_handler.cpp ...)
target_link_libraries(my_function PRIVATE
    functions-framework-cpp::framework
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)
```

## Dependency Management — vcpkg

```json
{
  "dependencies": [
    "functions-framework-cpp",
    "nlohmann-json",
    "spdlog",
    "gtest"
  ]
}
```

## Rules

- C++20 features: concepts, ranges, `std::format`, structured bindings.
- Smart pointers only — no raw `new`/`delete`.
- RAII for resource management.
- Handlers are thin — delegate to services.
