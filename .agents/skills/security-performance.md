---
name: security-performance
type: knowledge
version: 1.0.0
agent: CodeActAgent
triggers:
  - security
  - performance
  - memory
  - buffer overflow
  - sanitizer
---

# Security & Performance — C++20

## Performance

### Memory Efficiency

- Stack allocation preferred over heap.
- `std::string_view` for non-owning string references.
- Move semantics: `std::move()` for transferring ownership.
- `constexpr` for compile-time computation.

### Optimization

```cpp
// String view — no copy
void process(std::string_view data) { ... }

// Reserve vector capacity
std::vector<User> users;
users.reserve(expected_count);

// Move instead of copy
auto result = std::move(heavy_object);
```

### Profiling

- `perf` on Linux for CPU profiling.
- `valgrind --tool=massif` for heap profiling.
- AddressSanitizer: `-fsanitize=address`.

## Security

### Memory Safety

- **Smart pointers only** — eliminates use-after-free, double-free.
- AddressSanitizer (`-fsanitize=address`) in debug builds.
- UndefinedBehaviorSanitizer (`-fsanitize=undefined`).
- **No `sprintf`, `strcpy`** — use `std::format`, `std::string`.

### Buffer Overflow Prevention

```cpp
// NEVER: char buf[256]; sprintf(buf, "%s", input);
// ALWAYS: use std::string or std::format
auto message = std::format("Hello, {}", name);
```

### Input Validation

```cpp
auto validate_dto(const nlohmann::json& j) -> Result<CreateDTO, AppError> {
    if (!j.contains("name") || !j["name"].is_string()) {
        return AppError{ErrorCode::ValidationFailed, "name is required"};
    }
    auto name = j["name"].get<std::string>();
    if (name.empty() || name.size() > 100) {
        return AppError{ErrorCode::ValidationFailed, "name must be 1-100 chars"};
    }
    return CreateDTO{name, j["email"].get<std::string>()};
}
```

### SQL/Injection Prevention

- Parameterized queries — never concatenate user input into queries.
- Validate and sanitize all external input.

### Build Hardening

```cmake
target_compile_options(my_function PRIVATE
    -Wall -Wextra -Wpedantic
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
)
```
