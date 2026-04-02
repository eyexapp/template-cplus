# Architecture — C++ Backend Template

## Overview

C++20 backend service built on **Google Cloud Functions Framework**. Layered into
Handlers → Services → Core, with a Router dispatching HTTP requests to the correct handler.
Packaged as a minimal scratch-based Docker image for Cloud Run deployment.

## Directory Structure

```
src/
├── main.cc                  # Entry point — init spdlog, create services, register routes, gcf::Run
├── config.h                 # Header-only env config reader (TARGET, PORT, LOG_LEVEL)
├── router.h / router.cc     # Path+Method → Handler dispatch map
├── handlers/
│   ├── hello_handler.h      # GET /hello → JSON greeting (uses GreetingService)
│   └── health_handler.h     # GET /health → {"status":"ok"}
└── services/
    ├── greeting_service.h   # Interface: greet(name) → string
    └── greeting_service.cc  # Business logic implementation
tests/
├── greeting_service_test.cc # Unit tests for GreetingService
└── router_test.cc           # Unit tests for Router dispatch
```

## Key Patterns

### Router Pattern
Cloud Functions Framework accepts a single handler function. The `Router` class wraps this
by mapping `"METHOD /path"` keys to handler functions. Routes are registered in `main.cc`:
```cpp
router.get("/health", app::handlers::health);
router.get("/hello", [&service](auto const& req) { return app::handlers::hello(req, service); });
```

### Layers
- **Handlers** — Parse HTTP request, call services, return JSON response. No business logic.
- **Services** — Pure business logic. No HTTP awareness. Easy to test.
- **Config** — Reads environment variables with defaults. Header-only, no external files.

### Testing via app_lib
`src/` compiles as a STATIC library (`app_lib`). Both `main.cc` and test executables link to it,
so all code is testable without HTTP overhead.

## Build Commands

```bash
# With CMake presets (requires VCPKG_ROOT env)
cmake --preset debug && cmake --build --preset debug
ctest --preset debug

# Docker
docker-compose up --build
curl http://localhost:3000/health
curl http://localhost:3000/hello?name=World
```

## Adding a New Handler

1. Create `src/handlers/<name>_handler.h` with a function matching `gcf::HttpResponse(gcf::HttpRequest const&)`
2. If it needs business logic, create a service in `src/services/`
3. Register the route in `src/main.cc`: `router.get("/path", handler)`
4. Add `service.cc` to `src/CMakeLists.txt` `app_lib` sources (if applicable)
5. Write tests in `tests/<name>_test.cc` and add to `tests/CMakeLists.txt`

## Adding a New Service

1. Create `src/services/<name>_service.h` (interface) + `.cc` (implementation)
2. Add `.cc` to `app_lib` sources in `src/CMakeLists.txt`
3. Inject into handler via lambda capture in `main.cc`
4. Write tests in `tests/<name>_service_test.cc`

## Dependencies

| Library | Purpose | Header |
|---------|---------|--------|
| functions-framework-cpp | HTTP server (Cloud Functions) | `<google/cloud/functions/framework.h>` |
| spdlog | Structured logging | `<spdlog/spdlog.h>` |
| nlohmann/json | JSON serialization | `<nlohmann/json.hpp>` |
| GoogleTest | Unit testing | `<gtest/gtest.h>` |
