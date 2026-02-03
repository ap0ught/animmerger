# Architecture Documentation

## System Architecture

Animmerger follows a modular architecture with clear separation of concerns:

```
┌─────────────────────────────────────────┐
│           Main Application              │
│              (main.cc)                  │
└────────────────┬────────────────────────┘
                 │
    ┌────────────┼────────────┐
    │            │            │
    ▼            ▼            ▼
┌─────────┐  ┌─────────┐  ┌──────────┐
│ Canvas  │  │ Pixel   │  │ Alignment│
│ System  │  │ System  │  │ System   │
└─────────┘  └─────────┘  └──────────┘
    │            │            │
    ▼            ▼            ▼
┌─────────┐  ┌─────────┐  ┌──────────┐
│ Dither  │  │Quantize │  │ Palette  │
└─────────┘  └─────────┘  └──────────┘
```

## Component Descriptions

### Canvas System
- **Purpose:** Image buffer management
- **Files:** `canvas.cc`, `canvas.hh`
- **Key Features:**
  - Image loading and saving
  - Buffer manipulation
  - Coordinate systems

### Pixel System
- **Purpose:** Pixel-level operations and method dispatch
- **Files:** `pixel.cc`, `pixel.hh`, `pixels/*`
- **Key Features:**
  - Template-based method selection
  - Runtime polymorphism via traits
  - Extensible method system

### Alignment System
- **Purpose:** Image alignment and stitching
- **Files:** `align.cc`, `align.hh`
- **Key Features:**
  - Image registration
  - Coordinate transformation
  - Multi-frame alignment

### Dithering System
- **Purpose:** Error diffusion and dithering algorithms
- **Files:** `dither.cc`, `dither.hh`
- **Key Features:**
  - Multiple dithering algorithms
  - Configurable error propagation
  - Ordered and error-diffusion dithering

### Quantization System
- **Purpose:** Color reduction and palette generation
- **Files:** `quantize.cc`, `quantize.hh`
- **Key Features:**
  - Median cut algorithm
  - Octree quantization
  - K-means clustering
  - KD-tree based optimization

### Palette System
- **Purpose:** Color palette management
- **Files:** `palette.cc`, `palette.hh`
- **Key Features:**
  - Palette creation
  - Color matching
  - Palette optimization

## Data Flow

### Image Processing Pipeline

1. **Input:** Load images via Canvas
2. **Alignment:** Align multiple images if needed
3. **Quantization:** Generate optimal palette
4. **Dithering:** Apply dithering algorithm
5. **Output:** Save processed image/animation

## Memory Management

- Custom allocation strategies in `alloc/`
- RAII principles for resource management
- Template-based memory optimization

## Concurrency

- OpenMP directives for parallelization
- Thread-safe data structures where needed
- Parallel quantization and dithering

## Extension Points

1. **New Pixel Methods:** Add to `pixels/` directory
2. **New Dithering Algorithms:** Extend dithering system
3. **New Quantization Methods:** Extend quantization system
4. **Custom Data Structures:** Add to appropriate header

## Performance Characteristics

- **KD-Tree:** O(log n) color lookups
- **Binary Heap:** O(log n) priority operations
- **Range Operations:** O(1) amortized access
- **Parallel Processing:** Linear speedup with cores
