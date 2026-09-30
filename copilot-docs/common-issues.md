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

**Note:** the Makefile already passes `-fopenmp` unconditionally, and
`alloc/FSBAllocator.hh` is compiled with
`FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OPENMP`. OpenMP is therefore a hard
requirement, not an optional extra — a missing OpenMP runtime is a build
failure, not a performance setting.

**Solution:**
1. Install OpenMP: `apt-get install libomp-dev` (Debian/Ubuntu) or `pacman -S
   gcc` (Arch, ships with the compiler)
2. Verify OpenMP support: `echo |cpp -fopenmp -dM |grep -i openmp`
3. On macOS with Apple clang, `-fopenmp` may be unavailable — install a real
   LLVM via Homebrew and put it first on `PATH`, or build without it after
   commenting out `-fopenmp` and the `FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OPENMP`
   define in the Makefile.

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
2. Enable debug symbols: `make CXXFLAGS="-std=gnu++1z -fopenmp -g -O0"`
   (a command-line `CXXFLAGS` replaces the Makefile's flags, so the C++17
   standard has to be repeated or the build fails)
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
make CXXFLAGS="-std=gnu++1z -fopenmp -pg -O2"

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

`make check` prints one `ok <name>` or `FAIL <name>: <reason>` line per test and
a final `N passed, M failed`. The harness takes no arguments, so there is no way
to run a single test — find the `FAIL` line and read the reason it prints.

**Debugging Steps:**
1. Locate the `FAIL <name>` line and read the message
2. Check test expectations vs actual output
3. Verify input test data is correct
4. Compare with known good output — fixtures the harness wrote are left in `tests/out/`
5. Check for floating-point precision issues

> If a test you expect to be load-bearing passes against obviously wrong code,
> the test is the problem. That is how the equivalent-mutant case was found:
> swapping `GetMostUsed()` for `GetLeastUsed()` still returned the background for
> every pixel of the fixture. Strengthen the assertion rather than accepting the
> green.

---

## Platform-Specific Issues

### Linux

**Issue:** Missing dependencies
```bash
sudo apt-get install build-essential libgd-dev
```

libgd is the one that matters — it is the only external library the binary
links (`-lgd`), and without it `make` fails at the link step. `libomp-dev` is
only needed where GCC's OpenMP runtime is packaged separately.

### macOS

**Issue:** Apple clang has no OpenMP support
```bash
brew install libomp
export LDFLAGS="-L/usr/local/opt/libomp/lib"
export CPPFLAGS="-I/usr/local/opt/libomp/include"
```
Remember that a command-line `CXXFLAGS` wipes `-std=gnu++1z` out of the
Makefile — see the build notes in `workflow.md`.

### Windows

**Issue:** MSVC compilation differences
- Use `/openmp` flag instead of `-fopenmp`
- Use `/std:c++17` — the Makefile's `-std=gnu++1z` is GCC/Clang syntax
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
