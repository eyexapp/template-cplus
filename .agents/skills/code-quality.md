---
name: code-quality
type: knowledge
version: 1.0.0
agent: CodeActAgent
triggers:
  - clean code
  - naming
  - modern c++
  - smart pointer
  - raii
  - concept
---

# Code Quality — C++20

## Naming Conventions

| Element | Convention | Example |
|---------|-----------|---------|
| Class | PascalCase | `UserService` |
| Function/Method | snake_case | `find_by_id()` |
| Variable | snake_case | `user_count` |
| Constant | kPascalCase | `kMaxRetries` |
| Macro | UPPER_SNAKE | `LOG_ERROR(...)` |
| Namespace | snake_case | `namespace user_service` |
| Header guard | UPPER_SNAKE_H | `USER_SERVICE_H` |
| File | snake_case | `user_service.cpp` |

## Modern C++20 Features

```cpp
// Concepts
template<typename T>
concept Serializable = requires(T t) {
    { t.to_json() } -> std::convertible_to<nlohmann::json>;
};

// Structured bindings
auto [name, email] = parse_user_data(json);

// std::optional
std::optional<User> find_by_id(const std::string& id);

// std::expected (C++23) or Result type
Result<User, AppError> create_user(CreateDTO dto);

// Ranges
auto active_users = users | std::views::filter([](const User& u) { return u.is_active; });
```

## Smart Pointers

```cpp
// Unique ownership
auto service = std::make_unique<UserService>(repo);

// Shared ownership (rare — prefer unique)
auto pool = std::make_shared<ConnectionPool>(config);

// NEVER raw new/delete
// User* u = new User();  // FORBIDDEN
```

## Logging — spdlog

```cpp
#include <spdlog/spdlog.h>

spdlog::info("User created: id={}, email={}", user.id, user.email);
spdlog::error("Failed to create user: {}", error.what());
```

## Error Handling

```cpp
// Custom error types
enum class ErrorCode { NotFound, Conflict, ValidationFailed, Internal };

class AppError : public std::exception {
    ErrorCode code_;
    std::string message_;
public:
    AppError(ErrorCode code, std::string msg) : code_(code), message_(std::move(msg)) {}
    ErrorCode code() const { return code_; }
    const char* what() const noexcept override { return message_.c_str(); }
};
```

## Rules

- No raw pointers for ownership — `unique_ptr`/`shared_ptr`.
- No C-style casts — use `static_cast`, `dynamic_cast`.
- RAII for all resources (files, connections, locks).
- `const` by default — mark mutable explicitly.
