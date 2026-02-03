# GitHub Copilot Documentation for Animmerger

This document contains lessons, patterns, and insights learned while working with the Animmerger codebase. It serves as a knowledge base for GitHub Copilot to better assist with development tasks.

## Project Overview

Animmerger is a tool that stitches 2D images together to form either static images or animations. It features an extensive set of dithering and quantizing algorithms, making it useful as a general-purpose quantizing and dithering tool for arbitrary images.

**Official Documentation:** http://bisqwit.iki.fi/source/animmerger.html

## Architecture and Key Components

### Core Components

1. **Pixel Processing** (`pixel.cc`, `pixel.hh`)
   - Defines pixel methods and implementations
   - Located in `pixels/` directory
   - See `doc/AddingPixelMethods.txt` for adding new pixel methods

2. **Canvas** (`canvas.cc`, `canvas.hh`)
   - Image canvas manipulation

3. **Alignment** (`align.cc`, `align.hh`)
   - Image alignment algorithms

4. **Dithering** (`dither.cc`, `dither.hh`)
   - Dithering algorithm implementations

5. **Quantization** (`quantize.cc`, `quantize.hh`)
   - Color quantization algorithms

6. **Palette** (`palette.cc`, `palette.hh`)
   - Palette management and operations

7. **Masking** (`mask.cc`, `mask.hh`)
   - Image masking functionality

### Data Structures

- **Binary Heap** (`binaryheap.hh`)
- **KD-Tree** (`kdtree.hh`)
- **Range Types** (`range.hh`, `range.tcc`)
- **Map Types** (`maptype.hh`, `rangemap.hh`, `rangemap.tcc`)
- **Set Types** (`settype.hh`)
- **Vector Types** (`vectype.hh`)

### Build System

- Uses **Makefile** for building
- `Makefile.sets` contains build configuration
- `depfun.mak` for dependency management
- OpenMP support (`openmp.hh`)

## Development Patterns

### Adding New Pixel Methods

From `doc/AddingPixelMethods.txt`:

1. **Add method number** in `pixel.hh` (`#define DefinePixelMethods`)
2. **Implement the method** in `pixels/`
   - Either add to existing class or create new class
   - Update traits for modified class
3. **If new class created**, update `pixel.cc`:
   - Add relevant include file
   - Update `#define DefinePixelClasses`

**Common Error:**
```
error: 'getname' is not a member of '<unnamed>::PixelMethodImplName<void>'
```
This means a PixelMethod has no PixelImpl that implements it.

### Code Organization

- **Header files** (`.hh`) contain class definitions and templates
- **Template implementations** (`.tcc`) for template classes
- **Implementation files** (`.cc`) for concrete implementations
- **Subdirectories** for organized components:
  - `pixels/` - Pixel method implementations
  - `doc/` - Documentation
  - `util/` - Utility functions
  - `alloc/` - Memory allocation utilities
  - `fparser/` - Formula parser

## Build and Development

### Building the Project

```bash
make
```

### Dependencies

- C++ compiler with C++11 support
- OpenMP for parallel processing

## Common Tasks

### Working with Images

- Image I/O and manipulation through Canvas class
- Support for various image formats
- Dithering and quantization for color reduction

### Performance Optimization

- OpenMP parallelization support
- KD-tree for spatial queries
- Binary heap for priority operations
- Range-based operations for efficiency

## Coding Conventions

- Use `.hh` extension for C++ headers
- Use `.cc` extension for C++ implementation files
- Template implementations in `.tcc` files
- Preprocessor macros for component definitions
- Traits-based template programming

## Testing and Validation

- Test images and validation scripts in appropriate directories
- PHP scripts for documentation generation (`doc/docmaker.php`, `doc/document.php`)
- Preset configurations in `presets.cc`

## External Resources

- **Project Website:** http://bisqwit.iki.fi/source/animmerger.html
- **License:** See `COPYING` file

## Lessons Learned

### Design Patterns

1. **Template-based polymorphism** for pixel methods
2. **Traits pattern** for compile-time configuration
3. **Factory pattern** for creating pixel implementations
4. **Preprocessor macros** for component registration

### Performance Considerations

1. Use OpenMP for parallel processing where appropriate
2. KD-trees for efficient spatial queries
3. Range-based operations minimize allocations
4. Template specialization for optimization

### Common Pitfalls

1. **Missing PixelImpl implementations** cause compilation errors
2. **Trait updates required** when adding methods to classes
3. **Include file registration** needed for new pixel classes

## Future Development Notes

- Consider modernizing build system (CMake?)
- Additional dithering algorithms can be added
- More quantization methods possible
- Documentation could be expanded

## Documentation Generation

### PHP-Based HTML Generator (Fixed for PHP 8+)

As of 2026-02-03, the original PHP-based documentation generator has been fixed for PHP 8+ compatibility:

**Location:** `doc/docmaker.php` and `doc/document.php`

**Issue Fixed:** Replaced deprecated `each()` function with `foreach()` in `doc/document.php` (lines 17 and 86)

**Usage:**
```bash
php doc/docmaker.php animmerger.tar.bz2 progdesc.php > animmerger.html
```

**Source Files:**
- `progdesc.php` - Main documentation source (1,382 lines) containing all text content as PHP array
- `doc/docmaker.php` - Generic documentation generator script (version 1.2.1)
- `doc/document.php` - Document formatting module (version 2.0.7, now PHP 8+ compatible)

**Generated Output:** HTML documentation matching the official website at http://bisqwit.iki.fi/source/animmerger.html

**Optional Dependencies:**
- `/usr/local/bin/animmerger` - For help text inclusion (gracefully degrades if missing)
- `/usr/local/bin/htmlrecode` - For character encoding (gracefully degrades if missing)

For more details, see `GENERATING_DOCS.md`.

---

*Last Updated: 2026-02-03*
*This document should be updated as new patterns and lessons are discovered.*
