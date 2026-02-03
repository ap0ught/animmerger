# Development Workflow

## Getting Started

### Initial Setup

```bash
# Clone repository
git clone <repository-url>
cd animmerger

# Build project
make

# Run basic test
./animmerger --help
```

### Development Environment

**Recommended:**
- C++ compiler with C++11 support (GCC 4.8+, Clang 3.4+)
- OpenMP support for parallelization
- Text editor with C++ syntax support
- Git for version control

**Optional:**
- gdb for debugging
- valgrind for memory checking
- gprof for profiling
- cppcheck for static analysis

---

## Daily Development Workflow

### 1. Create Feature Branch

```bash
git checkout -b feature/new-dithering-algorithm
```

### 2. Make Changes

**Typical workflow:**
1. Understand the existing code
2. Write tests (if applicable)
3. Implement feature
4. Test locally
5. Document changes

### 3. Build and Test

```bash
# Clean build
make clean
make

# Run tests
make test  # if test target exists

# Manual testing
./animmerger input.png output.png [options]
```

### 4. Debug Issues

```bash
# Build with debug symbols
make clean
make CXXFLAGS="-g -O0"

# Run with debugger
gdb ./animmerger
(gdb) run [args]

# Check for memory leaks
valgrind --leak-check=full ./animmerger [args]
```

### 5. Commit Changes

```bash
git add [files]
git commit -m "Add new dithering algorithm"
```

### 6. Push and Create PR

```bash
git push origin feature/new-dithering-algorithm
# Create pull request on GitHub
```

---

## Code Modification Workflows

### Adding a New Pixel Method

**Workflow:**

1. **Plan the method**
   - Determine what operation it performs
   - Identify required pixel data
   - Choose appropriate pixel types

2. **Create implementation file**
   ```bash
   touch pixels/newmethod.hh
   ```

3. **Implement the method**
   ```cpp
   // pixels/newmethod.hh
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

4. **Register in pixel.hh**
   ```cpp
   #define DefinePixelMethods \
       /* existing methods */ \
       PixelMethod(NEXT_ID, NewMethod)
   ```

5. **Update pixel.cc**
   ```cpp
   #include "pixels/newmethod.hh"
   
   #define DefinePixelClasses \
       /* existing classes */ \
       PixelClass(NewMethodImpl)
   ```

6. **Build and test**
   ```bash
   make clean && make
   ./animmerger [test with new method]
   ```

7. **Document**
   - Add to COPILOT_README.md
   - Update API documentation
   - Add usage examples

---

### Adding a New Dithering Algorithm

**Workflow:**

1. **Study existing algorithms**
   ```bash
   grep -r "dither" dither.cc dither.hh
   ```

2. **Implement function**
   ```cpp
   // dither.cc
   void ditherNewAlgorithm(Canvas& canvas, const Palette& palette) {
       // Implementation
   }
   ```

3. **Add declaration**
   ```cpp
   // dither.hh
   void ditherNewAlgorithm(Canvas& canvas, const Palette& palette);
   ```

4. **Integrate with main**
   - Add command-line option
   - Connect to processing pipeline

5. **Test thoroughly**
   - Various image sizes
   - Different color counts
   - Edge cases (1x1, large images)

6. **Benchmark**
   ```bash
   time ./animmerger input.png output.png --dither=newalgorithm
   ```

---

### Refactoring Code

**Workflow:**

1. **Ensure tests exist**
   - Write tests if missing
   - Verify tests pass

2. **Make small changes**
   - One refactoring at a time
   - Keep builds working

3. **Test after each change**
   ```bash
   make && make test
   ```

4. **Commit frequently**
   ```bash
   git commit -m "Extract method for color conversion"
   ```

5. **Review the diff**
   ```bash
   git diff HEAD~1
   ```

---

## Testing Workflow

### Manual Testing

```bash
# Basic functionality
./animmerger input.png output.png

# Specific features
./animmerger input.png output.png --dither=floyd --colors=256

# Edge cases
./animmerger tiny.png output.png
./animmerger huge.png output.png
./animmerger corrupt.png output.png
```

### Automated Testing

If test framework exists:
```bash
make test
```

### Visual Testing

```bash
# Generate test output
./animmerger test.png result.png [options]

# Compare with reference
diff result.png reference.png

# Visual inspection
# Open result.png in image viewer
```

### Performance Testing

```bash
# Time execution
time ./animmerger large.png output.png

# Profile
make clean
make CXXFLAGS="-pg -O2"
./animmerger input.png output.png
gprof ./animmerger > profile.txt
```

---

## Debugging Workflow

### Compile-Time Errors

1. **Read error message carefully**
2. **Check line number and file**
3. **Look for typos**
4. **Verify template instantiation**
5. **Check include files**

### Runtime Errors

1. **Build with debug symbols**
   ```bash
   make CXXFLAGS="-g -O0"
   ```

2. **Run with debugger**
   ```bash
   gdb ./animmerger
   (gdb) break main
   (gdb) run [args]
   (gdb) next
   (gdb) print variable
   ```

3. **Check memory issues**
   ```bash
   valgrind ./animmerger [args]
   ```

### Logic Errors

1. **Add debug prints**
   ```cpp
   std::cerr << "Debug: value=" << value << std::endl;
   ```

2. **Use assertions**
   ```cpp
   assert(condition && "Error message");
   ```

3. **Reduce test case**
   - Simplify input
   - Binary search for issue

---

## Documentation Workflow

### Code Documentation

1. **Comment complex algorithms**
2. **Document public APIs**
3. **Explain non-obvious choices**
4. **Keep comments up-to-date**

### External Documentation

1. **Update COPILOT_README.md**
2. **Update copilot-docs/ files**
3. **Add usage examples**
4. **Document breaking changes**

---

## Release Workflow

### Pre-Release Checklist

- [ ] All tests pass
- [ ] No memory leaks (valgrind)
- [ ] Documentation updated
- [ ] CHANGELOG updated
- [ ] Version number bumped

### Release Process

1. **Create release branch**
   ```bash
   git checkout -b release/v1.0.0
   ```

2. **Final testing**
3. **Tag release**
   ```bash
   git tag -a v1.0.0 -m "Release version 1.0.0"
   ```

4. **Push**
   ```bash
   git push origin release/v1.0.0
   git push origin v1.0.0
   ```

---

## Best Practices

### Code Quality

- Write clear, readable code
- Follow existing style conventions
- Use meaningful names
- Keep functions small and focused
- Avoid premature optimization

### Git Usage

- Commit often, push regularly
- Write descriptive commit messages
- Keep commits atomic
- Use branches for features
- Rebase to keep history clean

### Communication

- Document design decisions
- Explain complex code
- Update documentation
- Respond to code review feedback
- Help others understand your changes

### Continuous Improvement

- Learn from mistakes
- Refactor as you go
- Keep dependencies updated
- Monitor performance
- Stay current with C++ best practices
