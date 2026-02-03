# Common Issues and Solutions

## Build Issues

### Issue: Compilation Errors with Template Instantiation

**Error Message:**
```
pixel.cc: In instantiation of 'const<unnamed>::FactoryType <unnamed>::PixelImplCombFactory<void>::data':
pixel.cc:333:   instantiated from here
pixel.cc:312: error: 'getname' is not a member of '<unnamed>::PixelMethodImplName<void>'
pixel.cc:312: error: too many initializers for 'const<unnamed>::FactoryType'
```

**Cause:** A PixelMethod is defined but has no PixelImpl that implements it.

**Solution:**
1. Check `#define DefinePixelMethods` in `pixel.hh`
2. Ensure each method has a corresponding implementation
3. Verify `#define DefinePixelClasses` includes the implementation
4. Check that implementation class has `getname()` static method

**Prevention:** Always implement the method before defining it in DefinePixelMethods.

---

### Issue: Missing Include Files

**Error Message:**
```
fatal error: header.hh: No such file or directory
```

**Solution:**
1. Check include paths in Makefile
2. Verify file exists in expected location
3. Check for correct file extension (`.hh` not `.h`)
4. Ensure path is relative to project root

---

### Issue: OpenMP Not Available

**Symptoms:** Slow performance, single-threaded execution

**Solution:**
1. Install OpenMP library: `apt-get install libomp-dev`
2. Add OpenMP flags to Makefile: `-fopenmp`
3. Verify OpenMP support: `echo |cpp -fopenmp -dM |grep -i openmp`

---

## Runtime Issues

### Issue: Segmentation Fault During Image Processing

**Common Causes:**
1. Out-of-bounds array access
2. Invalid pointer dereference
3. Stack overflow from deep recursion
4. Memory corruption

**Debugging Steps:**
1. Run with valgrind: `valgrind --leak-check=full ./animmerger`
2. Enable debug symbols: `make CXXFLAGS="-g -O0"`
3. Use gdb: `gdb ./animmerger`
4. Check array bounds carefully
5. Verify memory allocation succeeded

---

### Issue: Incorrect Color Output

**Symptoms:** Colors don't match expected output

**Possible Causes:**
1. Wrong color space conversion
2. Palette generation issues
3. Dithering error accumulation
4. Pixel format mismatch

**Solutions:**
1. Verify color space (RGB vs YUV)
2. Check palette size and quality
3. Try different dithering algorithms
4. Validate input image format

---

### Issue: Memory Leaks

**Detection:**
```bash
valgrind --leak-check=full --show-leak-kinds=all ./animmerger [args]
```

**Common Sources:**
1. Missing destructors
2. Unreleased resources
3. Circular references
4. Forgotten cleanup in error paths

**Solutions:**
1. Use RAII for all resources
2. Implement proper destructors
3. Use smart pointers where appropriate
4. Check error paths for cleanup

---

## Development Issues

### Issue: Adding New Pixel Method Fails

**Steps to Verify:**

1. **Method number is unique:**
   ```cpp
   #define DefinePixelMethods \
       PixelMethod(1, Method1) \
       PixelMethod(2, Method2) \
       PixelMethod(3, NewMethod)  // Must be unique
   ```

2. **Implementation exists in `pixels/`:**
   ```cpp
   template<typename PixelType>
   class NewMethodImpl {
       static void process(PixelType& pixel);
       static const char* getname();
   };
   ```

3. **Traits updated if modifying existing class:**
   ```cpp
   template<>
   struct PixelTraits<MyPixelType> {
       static const bool supportsNewMethod = true;
   };
   ```

4. **Class registered in `pixel.cc`:**
   ```cpp
   #include "pixels/newmethod.hh"
   
   #define DefinePixelClasses \
       PixelClass(NewMethodImpl)
   ```

---

### Issue: Performance Degradation

**Symptoms:** Slow image processing

**Profiling:**
```bash
# Compile with profiling
make CXXFLAGS="-pg -O2"

# Run program
./animmerger [args]

# Analyze profile
gprof ./animmerger gmon.out > profile.txt
```

**Common Bottlenecks:**
1. Inefficient color matching (use KD-tree)
2. Unnecessary memory copies
3. Missing OpenMP parallelization
4. Inefficient dithering loops

**Solutions:**
1. Use KD-tree for O(log n) color lookup
2. Pass by reference/move semantics
3. Add `#pragma omp parallel for` to large loops
4. Optimize inner loops (unroll, vectorize)

---

### Issue: Test Failures

**Debugging Steps:**
1. Run single failing test
2. Check test expectations vs actual output
3. Verify input test data is correct
4. Compare with known good output
5. Check for floating-point precision issues

---

## Platform-Specific Issues

### Linux

**Issue:** Missing dependencies
```bash
sudo apt-get install build-essential libomp-dev
```

### macOS

**Issue:** Clang doesn't support OpenMP by default
```bash
brew install libomp
export LDFLAGS="-L/usr/local/opt/libomp/lib"
export CPPFLAGS="-I/usr/local/opt/libomp/include"
```

### Windows

**Issue:** MSVC compilation differences
- Use `/openmp` flag instead of `-fopenmp`
- Handle path separators correctly
- Use Windows-compatible headers

---

## Getting Help

1. **Check existing documentation:**
   - `README.md`
   - `doc/AddingPixelMethods.txt`
   - Project website: http://bisqwit.iki.fi/source/animmerger.html

2. **Enable verbose output:**
   - Add debug prints
   - Use logging framework
   - Check intermediate results

3. **Reduce test case:**
   - Simplify input
   - Isolate failing component
   - Binary search for issue

4. **Ask for help:**
   - Provide minimal reproducible example
   - Include error messages
   - Share environment details
   - Show what you've tried

---

## Prevention Best Practices

1. **Write tests first** - TDD helps catch issues early
2. **Use static analysis** - Tools like cppcheck, clang-tidy
3. **Code review** - Peer review catches many issues
4. **Document assumptions** - Make implicit knowledge explicit
5. **Validate inputs** - Check preconditions
6. **Handle errors gracefully** - Don't crash on bad input
7. **Use version control** - Git bisect to find regressions
8. **Profile regularly** - Don't guess at performance
9. **Test on multiple platforms** - Catch portability issues
10. **Keep dependencies updated** - Security and bug fixes
