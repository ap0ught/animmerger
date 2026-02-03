# Copilot Documentation Index

Welcome to the GitHub Copilot documentation for Animmerger. This directory contains comprehensive documentation to help GitHub Copilot understand and work with the codebase effectively.

## Documentation Structure

### 📘 [COPILOT_README.md](../COPILOT_README.md)
**Main entry point** - Overview of the project, architecture, and quick reference for Copilot.

### 📁 copilot-docs/

Detailed documentation organized by topic:

#### 🏗️ [architecture.md](architecture.md)
- System architecture overview
- Component descriptions
- Data flow diagrams
- Performance characteristics
- Extension points

**When to use:** Understanding system design, planning major changes, adding new components

#### 🎨 [coding-patterns.md](coding-patterns.md)
- Template patterns and best practices
- Factory and registration patterns
- Memory management patterns
- Parallel processing patterns
- Code organization guidelines
- Common anti-patterns to avoid

**When to use:** Writing new code, refactoring, following project conventions

#### 📖 [api-reference.md](api-reference.md)
- Core API documentation
- Canvas, Pixel, Dithering, Quantization, Palette APIs
- Data structure APIs (KD-Tree, Binary Heap, Range types)
- Extension APIs
- Performance notes and thread safety

**When to use:** Using existing APIs, understanding interfaces, extending functionality

#### 🔧 [common-issues.md](common-issues.md)
- Build issues and solutions
- Runtime errors and debugging
- Development problems
- Platform-specific issues
- Prevention best practices

**When to use:** Troubleshooting errors, debugging problems, avoiding common mistakes

#### 🔄 [workflow.md](workflow.md)
- Development workflow
- Code modification workflows
- Testing procedures
- Debugging strategies
- Release process
- Best practices

**When to use:** Starting new tasks, following project processes, understanding development lifecycle

## Quick Reference

### Common Tasks

| Task | Documentation |
|------|--------------|
| Add new pixel method | [workflow.md](workflow.md#adding-a-new-pixel-method), [api-reference.md](api-reference.md#adding-new-pixel-methods) |
| Add dithering algorithm | [workflow.md](workflow.md#adding-a-new-dithering-algorithm) |
| Fix template errors | [common-issues.md](common-issues.md#issue-compilation-errors-with-template-instantiation) |
| Optimize performance | [common-issues.md](common-issues.md#issue-performance-degradation) |
| Understand architecture | [architecture.md](architecture.md) |
| Follow coding patterns | [coding-patterns.md](coding-patterns.md) |
| Debug runtime issues | [common-issues.md](common-issues.md#runtime-issues) |

### Key Concepts

- **Pixel Methods:** Template-based pixel processing system
- **Traits Pattern:** Compile-time configuration
- **Factory Pattern:** Component registration via macros
- **KD-Tree:** Fast spatial queries for color matching
- **OpenMP:** Parallel processing support
- **RAII:** Resource management pattern used throughout

### File Naming Conventions

- `.hh` - C++ header files
- `.cc` - C++ implementation files
- `.tcc` - Template implementation files
- `.php` - PHP documentation generation scripts

### Important Files

- `pixel.hh`, `pixel.cc` - Pixel processing system
- `canvas.hh`, `canvas.cc` - Image manipulation
- `dither.hh`, `dither.cc` - Dithering algorithms
- `quantize.hh`, `quantize.cc` - Color quantization
- `doc/AddingPixelMethods.txt` - Pixel method guide

## Usage Guide for Copilot

### When Starting a Task

1. Read the relevant documentation section(s)
2. Understand the existing patterns
3. Check common issues for potential problems
4. Follow the workflow for that type of task

### When Encountering Errors

1. Check [common-issues.md](common-issues.md) first
2. Review [api-reference.md](api-reference.md) for correct usage
3. Verify patterns in [coding-patterns.md](coding-patterns.md)

### When Adding Features

1. Review [architecture.md](architecture.md) for system design
2. Follow patterns in [coding-patterns.md](coding-patterns.md)
3. Use workflow from [workflow.md](workflow.md)
4. Check [api-reference.md](api-reference.md) for extension points

### When Refactoring

1. Understand current architecture in [architecture.md](architecture.md)
2. Follow patterns in [coding-patterns.md](coding-patterns.md)
3. Use refactoring workflow in [workflow.md](workflow.md)
4. Ensure tests pass (prevent regressions)

## Contributing to Documentation

This documentation should evolve as the codebase changes and new patterns emerge.

### When to Update

- **New features added** → Update relevant sections
- **Patterns discovered** → Add to coding-patterns.md
- **Issues encountered** → Add to common-issues.md
- **Architecture changes** → Update architecture.md
- **New workflows** → Add to workflow.md

### How to Update

1. Keep documentation accurate and up-to-date
2. Use clear examples and code snippets
3. Link related sections together
4. Include practical, actionable advice
5. Update the index when adding new sections

## Additional Resources

- **Project Website:** http://bisqwit.iki.fi/source/animmerger.html
- **Main README:** [../README.md](../README.md)
- **License:** [../COPYING](../COPYING)
- **Existing Docs:** [../doc/](../doc/)

## Document Version

- **Created:** 2026-02-03
- **Last Updated:** 2026-02-03
- **Status:** Initial version

---

**Note:** This documentation is specifically designed to help GitHub Copilot understand and work with the Animmerger codebase. It should be kept up-to-date as the project evolves.
