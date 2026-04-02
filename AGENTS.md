# AGENTS.md — C++20 Backend Service (Cloud Functions Framework)

## Project Identity

| Key | Value |
|-----|-------|
| Language | C++20 |
| Framework | Google Cloud Functions Framework for C++ |
| Build System | CMake (presets) + vcpkg |
| Logging | spdlog (structured) |
| JSON | nlohmann/json |
| Testing | GoogleTest (GTest) |
| Container | Docker (scratch-based) + Docker Compose |
| Deploy Target | Cloud Run |

---

## Architecture — Handlers → Services → Core

```
src/
├── main.cc                  ← ENTRY: Init spdlog, create services, register routes, gcf::Run
├── config.h                 ← INFRA: Header-only env config reader (TARGET, PORT, LOG_LEVEL)
├── router.h / router.cc     ← INFRA: Path+Method → Handler dispatch map
├── handlers/
│   ├── hello_handler.h      ← PRESENTATION: GET /hello → JSON (uses GreetingService)
│   └── health_handler.h     ← PRESENTATION: GET /health → {"status":"ok"}
└── services/
    ├── greeting_service.h   ← BUSINESS: Interface: greet(name) → string
    └── greeting_service.cc  ← BUSINESS: Implementation

tests/
├── greeting_service_test.cc ← Unit tests for GreetingService
└── router_test.cc           ← Unit tests for Router dispatch
```

### Request Flow
```
HTTP Request → gcf::Run → Router::dispatch → Handler → Service → JSON Response
```

### Strict Layer Rules

| Layer | Can Import From | NEVER Imports |
|-------|----------------|---------------|
| `handlers/` | services/, config.h, nlohmann/json | main.cc |
| `services/` | config.h, stdlib only | handlers/, router |
| `router` | handlers/ (via function pointers) | services/ |
| `main.cc` | everything (composition root) | — |

---

## Adding New Code — Where Things Go

### New Handler
1. Create `src/handlers/<name>_handler.h` — function matching `gcf::HttpResponse(gcf::HttpRequest const&)`
2. Create service in `src/services/` if business logic needed
3. Register route in `src/main.cc`: `router.get("/path", handler)`
4. Add `.cc` files to `src/CMakeLists.txt` `app_lib` sources
5. Write tests in `tests/<name>_test.cc`, add to `tests/CMakeLists.txt`

### New Service
1. Create `src/services/<name>_service.h` (interface) + `.cc` (implementation)
2. Add `.cc` to `app_lib` sources in `src/CMakeLists.txt`
3. Inject into handler via lambda capture in `main.cc`
4. Write tests in `tests/<name>_service_test.cc`

### Handler Pattern
```cpp
#pragma once
#include <google/cloud/functions/framework.h>
#include <nlohmann/json.hpp>
#include "../services/greeting_service.h"

namespace app::handlers {

inline gcf::HttpResponse hello(gcf::HttpRequest const& req, GreetingService& service) {
    auto name = req.query("name").value_or("World");
    auto greeting = service.greet(name);

    gcf::HttpResponse response;
    response.set_header("Content-Type", "application/json");
    response.set_payload(nlohmann::json{{"message", greeting}}.dump());
    return response;
}

}  // namespace app::handlers
```

### Route Registration
```cpp
// main.cc — composition root
GreetingService service;
Router router;

router.get("/health", app::handlers::health);
router.get("/hello", [&service](auto const& req) {
    return app::handlers::hello(req, service);
});
```

---

## Design & Architecture Principles

### Static Library Pattern (`app_lib`)
- All `src/` code compiles as a STATIC library
- Both `main.cc` and test executables link to `app_lib`
- This makes all code testable without HTTP overhead

### Handlers Are Thin
- Parse request parameters
- Call service method
- Serialize response to JSON
- NO business logic in handlers

### Services Are Pure
- Pure business logic
- No HTTP awareness (no `gcf::` types)
- Easy to test in isolation

### Manual Dependency Injection
- `main.cc` is the composition root
- Services created and injected via lambda captures
- No DI framework — explicit wiring

---

## Error Handling

### HTTP-Level Errors
```cpp
gcf::HttpResponse response;
if (!name.has_value()) {
    response.set_result(gcf::HttpResponse::kBadRequest);
    response.set_payload(R"({"error":"name parameter required"})");
    return response;
}
```

### Service-Level Errors
- Return `std::optional<T>` or `std::expected<T, Error>` (C++23) for recoverable errors
- Use exceptions ONLY for truly exceptional conditions
- Log errors with spdlog — never expose internal details in response

---

## Code Quality

### Naming Conventions
| Artifact | Convention | Example |
|----------|-----------|---------|
| File | `snake_case.cc` / `.h` | `greeting_service.cc` |
| Class | `PascalCase` | `GreetingService` |
| Function | `snake_case` | `greet`, `hello` |
| Namespace | `snake_case` | `app::handlers` |
| Constant | `kPascalCase` | `kMaxRetries` |
| Macro | `SCREAMING_SNAKE` | `APP_VERSION` |

### Modern C++20 Patterns
- Use `std::string_view` for non-owning string parameters
- Use `auto` when type is obvious from context
- `#pragma once` for header guards
- `constexpr` for compile-time constants
- Range-based for loops
- Structured bindings

---

## Testing Strategy

| Level | What | Where | Tool |
|-------|------|-------|------|
| Unit | Services, router dispatch | `tests/` | GoogleTest |
| Integration | Full handler chain | `tests/` | GoogleTest + HTTP mocks |

### GTest Pattern
```cpp
#include <gtest/gtest.h>
#include "../src/services/greeting_service.h"

TEST(GreetingServiceTest, GreetWithName) {
    GreetingService service;
    auto result = service.greet("World");
    EXPECT_EQ(result, "Hello, World!");
}

TEST(GreetingServiceTest, GreetWithEmpty) {
    GreetingService service;
    auto result = service.greet("");
    EXPECT_FALSE(result.empty());
}
```

---

## Security & Performance

### Security
- Header-only config reads ENV vars — no config files in image
- Scratch-based Docker image — minimal attack surface
- No `unsafe` memory operations without validation
- Input validation in handlers before passing to services

### Performance
- Static library linking — zero dynamic dispatch overhead
- nlohmann/json is header-only — optimized at compile time
- Minimal Docker image (scratch base)
- spdlog is async-capable for high-throughput logging

---

## Commands

| Action | Command |
|--------|---------|
| Build (debug) | `cmake --preset debug && cmake --build --preset debug` |
| Test | `ctest --preset debug` |
| Docker build+run | `docker-compose up --build` |
| Health check | `curl http://localhost:3000/health` |

---

## Prohibitions — NEVER Do These

1. **NEVER** put business logic in handlers — delegate to services
2. **NEVER** use raw `new`/`delete` — use smart pointers or stack allocation
3. **NEVER** use C-style casts — use `static_cast`, `reinterpret_cast`, etc.
4. **NEVER** use `using namespace std;` in headers — explicit `std::` prefix
5. **NEVER** expose internal error details in HTTP responses
6. **NEVER** use mutable global state — pass dependencies explicitly
7. **NEVER** skip `#pragma once` in headers
8. **NEVER** use `printf` — use spdlog for all logging
9. **NEVER** add `.cc` files without updating `CMakeLists.txt`
10. **NEVER** use compiler-specific extensions — standard C++20 only
