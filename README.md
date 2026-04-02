# C++ Backend Template

C++20 backend service template built on **Google Cloud Functions Framework**. Layered architecture
(Handlers → Services → Core), structured logging with spdlog, JSON responses with nlohmann/json,
GoogleTest test suite, and a minimal scratch-based Docker image for Cloud Run deployment.

## Features

- **C++20** — modern standard with concepts, ranges, std::format support
- **Layered architecture** — Router → Handlers → Services → Config
- **Google Cloud Functions Framework** — HTTP server for Cloud Run
- **spdlog** — fast structured logging
- **nlohmann/json** — intuitive JSON serialization
- **GoogleTest** — unit tests with app_lib linkage
- **CMake presets** — debug/release/test configurations
- **clang-format + clang-tidy** — automated formatting and static analysis
- **Multi-stage Docker** — Alpine build → scratch runtime (minimal image size)
- **CI/CD** — GitHub Actions (build, format check, test, docker build)

## Quick Start

### Docker (recommended)

```bash
docker-compose up --build
curl http://localhost:3000/health
curl http://localhost:3000/hello?name=World
```

### Local Build (requires vcpkg)

```bash
export VCPKG_ROOT=/path/to/vcpkg
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

## Architecture

```
src/
├── main.cc                     # Entry — init, wire services, register routes
├── config.h                    # Env config reader (TARGET, PORT, LOG_LEVEL)
├── router.h / router.cc        # HTTP method+path → handler dispatch
├── handlers/
│   ├── hello_handler.h         # GET /hello → JSON greeting
│   └── health_handler.h        # GET /health → {"status":"ok"}
└── services/
    ├── greeting_service.h      # Interface
    └── greeting_service.cc     # Business logic
tests/
├── greeting_service_test.cc
└── router_test.cc
```

## API Endpoints

| Method | Path | Description |
|--------|------|-------------|
| GET | `/health` | Health check — `{"status":"ok"}` |
| GET | `/hello?name=X` | Greeting — `{"greeting":"Hello X"}` |

## Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `TARGET` | `World` | Default greeting target |
| `PORT` | `8080` | Server listen port |
| `LOG_LEVEL` | `info` | spdlog level (trace/debug/info/warn/error) |

## Development

```bash
# Format
find src tests -name '*.cc' -o -name '*.h' | xargs clang-format -i

# Static analysis
clang-tidy src/*.cc src/services/*.cc -- -std=c++20

# Run tests
cmake --preset debug && cmake --build --preset debug && ctest --preset debug
```

## Adding a Handler

1. Create `src/handlers/<name>_handler.h`
2. If needed, create a service in `src/services/`
3. Register route in `src/main.cc`
4. Add `.cc` files to `src/CMakeLists.txt`
5. Write tests in `tests/`

See [.github/copilot-instructions.md](.github/copilot-instructions.md) for detailed architecture docs.

## License

[Apache-2.0](LICENSE)