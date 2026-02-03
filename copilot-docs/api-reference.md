# API Reference

## Core APIs

### Canvas API

#### Canvas Class
**File:** `canvas.hh`, `canvas.cc`

Primary interface for image manipulation.

**Key Methods:**
- `load(filename)` - Load image from file
- `save(filename)` - Save image to file
- `getWidth()`, `getHeight()` - Dimensions
- `getPixel(x, y)` - Get pixel value
- `setPixel(x, y, color)` - Set pixel value

### Pixel API

#### Pixel Method System
**File:** `pixel.hh`, `pixel.cc`, `pixels/*`

Template-based pixel processing system.

**Registration Macro:**
```cpp
#define DefinePixelMethods \
    PixelMethod(id, MethodName)
```

**Implementation Pattern:**
```cpp
template<typename PixelType>
class PixelMethodImpl {
    static void process(PixelType& pixel);
    static const char* getname();
};
```

### Dithering API

#### Dither Functions
**File:** `dither.hh`, `dither.cc`

Apply dithering algorithms to images.

**Common Dithering Methods:**
- Error diffusion (Floyd-Steinberg, etc.)
- Ordered dithering (Bayer, etc.)
- Random dithering

**Usage Pattern:**
```cpp
void applyDither(Canvas& canvas, const Palette& palette, DitherMethod method);
```

### Quantization API

#### Quantize Functions
**File:** `quantize.hh`, `quantize.cc`

Color quantization and palette generation.

**Methods:**
- `quantizeMedianCut()` - Median cut algorithm
- `quantizeOctree()` - Octree quantization
- `quantizeKMeans()` - K-means clustering

**Usage Pattern:**
```cpp
Palette generatePalette(const Canvas& canvas, int numColors, QuantizeMethod method);
```

### Palette API

#### Palette Class
**File:** `palette.hh`, `palette.cc`

Color palette management.

**Key Methods:**
- `addColor(color)` - Add color to palette
- `findNearest(color)` - Find closest color
- `size()` - Number of colors
- `getColor(index)` - Get color by index

### Alignment API

#### Align Functions
**File:** `align.hh`, `align.cc`

Image alignment and registration.

**Methods:**
- `alignImages()` - Align multiple images
- `computeOffset()` - Calculate image offset
- `stitchImages()` - Combine aligned images

## Data Structure APIs

### KD-Tree
**File:** `kdtree.hh`

K-dimensional tree for spatial queries.

**Template:**
```cpp
template<typename T, int Dimensions>
class KDTree {
    void insert(const T& point);
    T findNearest(const T& query);
};
```

**Use Case:** Fast color matching in quantization

### Binary Heap
**File:** `binaryheap.hh`

Priority queue implementation.

**Template:**
```cpp
template<typename T, typename Compare>
class BinaryHeap {
    void push(const T& value);
    T pop();
    const T& top() const;
};
```

**Use Case:** Priority-based processing

### Range Types
**Files:** `range.hh`, `range.tcc`, `rangemap.hh`, `rangemap.tcc`

Range-based containers and iterators.

**Range:**
```cpp
template<typename T>
class Range {
    T begin() const;
    T end() const;
    size_t size() const;
};
```

**RangeMap:**
```cpp
template<typename Key, typename Value>
class RangeMap {
    void insert(Key key, Value value);
    Value& operator[](Key key);
};
```

### Vector Types
**File:** `vectype.hh`

Type-safe vector operations.

**Common Types:**
- `Vec2<T>` - 2D vector
- `Vec3<T>` - 3D vector (for RGB)
- `Vec4<T>` - 4D vector (for RGBA)

## Utility APIs

### OpenMP Support
**File:** `openmp.hh`

Parallel processing utilities.

**Macros:**
- `OPENMP_ENABLED` - Check if OpenMP available
- `OMP_PARALLEL_FOR` - Parallel loop wrapper

### Type Utilities

#### MapType
**File:** `maptype.hh`

Type definitions for map containers.

#### SetType
**File:** `settype.hh`

Type definitions for set containers.

#### Types
**File:** `types.hh`

Common type definitions and aliases.

## Extension APIs

### Adding New Pixel Methods

**Step 1:** Define in `pixel.hh`
```cpp
#define DefinePixelMethods \
    /* existing methods */ \
    PixelMethod(NEW_ID, NewMethodName)
```

**Step 2:** Implement in `pixels/newmethod.hh`
```cpp
template<typename PixelType>
class NewMethodImpl {
    static void process(PixelType& pixel) {
        // Implementation
    }
    static const char* getname() {
        return "NewMethod";
    }
};
```

**Step 3:** Register in `pixel.cc`
```cpp
#include "pixels/newmethod.hh"

#define DefinePixelClasses \
    /* existing classes */ \
    PixelClass(NewMethodImpl)
```

### Adding New Dithering Algorithms

Extend dithering system with new algorithms:
```cpp
void ditherNewAlgorithm(Canvas& canvas, const Palette& palette) {
    // Implementation
}
```

### Adding New Quantization Methods

Extend quantization system:
```cpp
Palette quantizeNewMethod(const Canvas& canvas, int numColors) {
    // Implementation
    return palette;
}
```

## Error Handling

Most APIs use:
- Return values for success/failure
- Exceptions for critical errors
- Static assertions for compile-time checks

## Performance Notes

- **Canvas operations:** O(1) pixel access
- **KD-Tree queries:** O(log n) nearest neighbor
- **Dithering:** O(width × height) per image
- **Quantization:** Varies by algorithm (typically O(n log n))
- **Parallel operations:** Use OpenMP for large images

## Thread Safety

- **Canvas:** Not thread-safe, use separate instances
- **KD-Tree:** Thread-safe for reads after construction
- **Palette:** Thread-safe for reads after construction
- **Dithering:** Can parallelize per-row with care
- **Quantization:** Build KD-tree once, query in parallel
