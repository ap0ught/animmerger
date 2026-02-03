# Coding Patterns and Best Practices

## Template Patterns

### Traits-Based Configuration

```cpp
template<typename PixelType>
struct PixelTraits {
    static const bool hasAlpha = false;
    static const bool hasGamma = false;
    // ... more traits
};
```

**Usage:** Compile-time configuration of pixel behavior
**When to use:** Creating new pixel types or methods

### Template Specialization

Pattern used throughout for optimizing specific types:
```cpp
template<typename T>
void process(T value);

template<>
void process<int>(int value) {
    // Optimized version for int
}
```

### SFINAE (Substitution Failure Is Not An Error)

Used for conditional compilation based on type properties.

## Factory Pattern

### Pixel Method Factory

Located in `pixel.cc`:
```cpp
#define DefinePixelClasses \
    PixelClass(RGB) \
    PixelClass(YUV) \
    // ... more classes
```

**Purpose:** Register pixel implementations at compile time
**Extension:** Add new PixelClass entries when creating new implementations

## Macro-Based Registration

### Component Registration

```cpp
#define DefinePixelMethods \
    PixelMethod(1, FirstMethod) \
    PixelMethod(2, SecondMethod) \
    // ... more methods
```

**Rationale:** Centralized component management
**Maintenance:** Keep numbers unique and sequential

## Range-Based Operations

### Range Types

Pattern from `range.hh`:
```cpp
template<typename T>
class Range {
    T begin_, end_;
public:
    T begin() const { return begin_; }
    T end() const { return end_; }
};
```

**Benefits:**
- Type-safe iteration
- Compile-time optimization
- Clear semantics

## Error Handling

### Compile-Time Checks

Preferred over runtime checks where possible:
```cpp
static_assert(condition, "Error message");
```

### Template Instantiation Errors

Intentionally descriptive error messages via traits:
```cpp
template<typename T>
struct ErrorIfInvalid {
    static_assert(T::valid, "T must be valid");
};
```

## Memory Patterns

### Custom Allocators

Located in `alloc/`:
- Pool allocators for frequent small allocations
- Arena allocators for batch operations
- RAII wrappers for automatic cleanup

### Move Semantics

Prefer move over copy for large objects:
```cpp
Canvas result = std::move(temporary_canvas);
```

## Parallel Processing Patterns

### OpenMP Directives

```cpp
#pragma omp parallel for
for (int i = 0; i < n; ++i) {
    // Parallel work
}
```

**Guidelines:**
- Use for large loops
- Ensure thread safety
- Avoid overhead for small operations

### Thread-Local Storage

```cpp
#pragma omp threadprivate(variable)
```

## Optimization Patterns

### Loop Unrolling

Manually unrolled for critical paths:
```cpp
// Process 4 pixels at once
for (int i = 0; i < n; i += 4) {
    process(pixels[i]);
    process(pixels[i+1]);
    process(pixels[i+2]);
    process(pixels[i+3]);
}
```

### Inline Functions

Heavy use of `inline` for small, frequently-called functions.

### Branch Prediction Hints

Consider using `__builtin_expect` for hot paths.

## Code Organization Patterns

### Header Structure

```cpp
#ifndef HEADER_NAME_HH
#define HEADER_NAME_HH

// Forward declarations
class ClassName;

// Type definitions
using TypeAlias = OtherType;

// Class definition
class ClassName {
    // ...
};

// Inline implementations
inline void ClassName::inlineMethod() {
    // ...
}

#endif
```

### Implementation File Structure

```cpp
// Includes
#include "header.hh"

// Anonymous namespace for helpers
namespace {
    void helperFunction() {
        // ...
    }
}

// Public implementations
void ClassName::method() {
    // ...
}
```

## Common Anti-Patterns to Avoid

1. **Raw pointers** - Use RAII wrappers or smart pointers
2. **Manual memory management** - Prefer automatic storage
3. **Naked new/delete** - Use make_unique or custom allocators
4. **Excessive macros** - Only for registration and configuration
5. **Deep inheritance** - Prefer composition and templates

## Testing Patterns

- Unit tests for individual components
- Integration tests for pipelines
- Performance benchmarks for critical paths
- Visual validation for image output

## Documentation Patterns

- Header comments for public API
- Implementation comments for complex algorithms
- Inline comments for non-obvious code
- Separate documentation files for major features
