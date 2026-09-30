# Animmerger - Complete Documentation

This document contains the complete documentation for Animmerger, an advanced tool for stitching 2D images together into either static images or animations.

> **Note:** This documentation is derived from the official website at http://bisqwit.iki.fi/source/animmerger.html

## Table of Contents

1. [Purpose](#purpose)
2. [Pixel Methods](#pixel-methods)
   - [Static Methods](#static-methods)
   - [Animated Methods](#animated-methods)
3. [Masking Methods](#masking-methods)
4. [Color Quantization](#color-quantization)
5. [Dithering](#dithering)
6. [Color Compare Methods](#color-compare-methods)
7. [Transformation](#transformation)
8. [Caveats](#caveats)
9. [Usage](#usage)
10. [Exit Codes](#exit-codes)
11. [Requirements](#requirements)
12. [License](#license)

---

## Purpose

Animmerger stitches 2D images together into either a static image or an animation, while attempting to preserve a global frame of reference (static background).

That is, for a movie that follows an actor around (and the background scrolls to follow them), it creates a movie that has a fixed background, and the camera moves around in the scene.

It does this with:
- A motion detection algorithm
- A set of different pixel methods
- A simulated infinite 2D canvas — a 2D canvas that extends infinitely to all four directions (well, as infinite as 32-bit integers can get…)

---

## Pixel Methods

Animmerger supports various pixel methods that determine how pixels from multiple frames are combined. These methods are divided into **static methods** (produce a single frame) and **animated methods** (produce animations).

### Static Methods

Static methods reduce all input frames into a single output frame.

#### AVERAGE (option: `-pa`)

The "average" method produces a "motion blur" effect of the entire input, reducing it into a single frame.

**Features:**
- Creates motion blur effect showing traces of all animated actors
- Fast-moving objects appear as faint traces

**Usage:**
```bash
animmerger -pa input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
```

**Alternative:** `tinyaverage` (option `-pA`) - Requires less memory but is less accurate.

**Note:** Add `--yuv` option to calculate color averages through YUV colorspace instead of RGB (not supported by tinyaverage).

#### ACTIONAVG (option: `-pt`)

The "actionavg" method attempts to fix the faintness problem with the "average" method by keeping track separately of the background (using the "mostused" method) and adding it only once to the average of moving actors.

**Usage:**
```bash
animmerger -pt input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
```

**Note:** Add `--yuv` option for YUV colorspace calculations.

#### MOSTUSED (option: `-pm`)

The "most used" method produces what might be the background image for the entire animation.

**Features:**
- Produces background image
- Records actors that sit in a location for a long time
- Removes anything that wanders around

**Usage:**
```bash
animmerger -pm input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
```

#### LAST (option: `-pl`)

The "last" method is a simpler implementation of the MostUsed method, simply recording the last pixel value in any location.

**Usage:**
```bash
animmerger -pl input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
```

#### FIRST (option: `-pf`)

The "first" method is analogous to "last". It shows whatever first appeared in a particular pixel location.

**Note:** Moving objects may appear distorted (similar to moving paper during scanning).

**Usage:**
```bash
animmerger -pf input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
```

#### SOLID (option: `-pO`)

The "solid" method is an experimental light-weight replacement to the "mostused" method. It simply ignores anything that moves and retains whatever stays still for the longest time.

Unlike "mostused", it does not sum separate appearances together; it only finds the maximum length of consecutive sameness.

**Usage:**
```bash
animmerger -pO input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
```

#### FIRSTNMOST (option: `-pF`)

The "firstnmost" method chooses the most common pixel of first N pixel values. Set N with the `--firstlast` (`-f`) option.

**Behavior:**
- If N is 0: gets last uncommon pixel
- If N is negative: uses least common values rather than most common

**Usage:**
```bash
# Most common of first 4
animmerger -pF -f4 input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF

# First uncommon
animmerger -pF -f0 input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF

# Least common of first 10
animmerger -pF -f-10 input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
```

#### LASTNMOST (option: `-pL`)

The "lastnmost" method is analogous to "firstnmost"; it chooses the most common pixel of last N pixel values.

**Usage:**
```bash
# Most common of last 10
animmerger -pL -f10 input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
```

#### LEASTUSED (option: `-pe`)

The "least used" method is analogous to "most used". It can be used to find graphical artifacts and teleporting actors, but for the most part, the output is not very useful.

**Usage:**
```bash
animmerger -pe input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
```

### Animated Methods

Animated methods produce output animations.

#### CHANGELOG (option: `-pc`)

The "changelog" method records the entire animation. It takes considerably less disk space (and is faster to load) than the original animation, because the background does not scroll.

**Features:**
- Records full animation with fixed background
- Significantly smaller file size
- Exteriors colored as in MostUsed pixel method

**Usage:**
```bash
rm tile-*.png tile-*.gif
animmerger --gif -pc input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
gifsicle -O2 -o output.gif -l0 -d3 tile-*.gif
```

**Motion Blur Option:** Add `--motionblur N` (or `-B N`) where N is 1-16 to create motion blur effect.

#### LOOPINGLOG (option: `-s` or `-po`)

The "loopinglog" method records the entire animation but reuses existing frames. Use the `-l` option to set the loop length in frames. Also called "loopinglast" mode.

**Features:**
- Reuses existing frames for looping
- Smaller value creates shorter animation with more "lemmings" effect
- Supports motion blur with `--motionblur` (`-B`) option

**Usage:**
```bash
# Create 10-frame loop
animmerger --gif -l10 -po input/*.png
gifsicle -O2 -o loop.gif -l0 -d3 tile-*.gif

# With motion blur
animmerger --gif -l10 -B4 -po input/*.png
```

#### LOOPINGAVG (option: `-pv`)

The "loopingavg" method combines the "loopinglog" and "actionavg" methods. Use the `-l` option to set the loop length in frames.

**Features:**
- Overlapping action is averaged rather than choosing one pixel
- Looks slightly better than loopinglog
- May require GIF palette reduction
- Only uses pixel colors present in original images

**Usage:**
```bash
# Create 10-frame averaged loop
animmerger --gif -l10 -pv input/*.png
gifsicle -O2 -o avg_loop.gif -l0 -d3 tile-*.gif
```

**Motion Blur:** Add `--motionblur` (`-B`) option for motion blur effect.

### Method Summary

| Method | Option | Type | Description |
|--------|--------|------|-------------|
| AVERAGE | `-pa` | Static | Motion blur of entire input |
| TINYAVERAGE | `-pA` | Static | Memory-efficient average |
| ACTIONAVG | `-pt` | Static | Average with separate background |
| MOSTUSED | `-pm` | Static | Background extraction |
| LAST | `-pl` | Static | Last pixel value |
| FIRST | `-pf` | Static | First pixel value |
| SOLID | `-pO` | Static | Longest consecutive sameness |
| FIRSTNMOST | `-pF` | Static | Most common of first N |
| LASTNMOST | `-pL` | Static | Most common of last N |
| LEASTUSED | `-pe` | Static | Least used pixels |
| CHANGELOG | `-pc` | Animated | Full animation with fixed background |
| LOOPINGLOG | `-s` or `-po` | Animated | Looping optimized changelog |
| LOOPINGAVG | `-pv` | Animated | Looping with averaging |

---

## Masking Methods

Masking methods allow selective removal or modification of pixels in specific regions.

### BLACK/BLANK/CENSOR

Removes specified regions by making them black, transparent, or censored.

**Parameters:** `-m x,y,width,height,color1,color2,...`

**Example:**
```bash
# Remove HUD at position 0,8 with size 256x16
animmerger -pm input/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
```

> **The colour list is a filter, not a fill colour.** `color1,color2,...` lists
> the colours to *remove*. Pixels inside the rectangle that do not match one of
> them are left untouched. `mask.cc:565` gates the whole operation on
> `a.colors.empty() || a.colors.find(...) != a.colors.end()`, so:
>
> | command | effect on a 16x16 rect whose left half is red and right half blue |
> |---|---|
> | `-m0,0,16,16` | entire rectangle blanked |
> | `-m0,0,16,16,FF0000` | only the red half blanked; blue survives |
> | `-m0,0,16,16,00FF00` | nothing blanked — no pixel matches |
>
> To blank a whole rectangle, pass no colour list at all.

### HOLE/ALPHA/TRANSPARENT

Creates transparent holes in specified regions.

> **`censor` and `hole` differ in alpha, not RGB.** Both write black. Read from
> the output PNG, `censor` leaves `rgba(0,0,0,255)` and `hole` leaves
> `rgba(0,0,0,0)`; `mask.cc:572` sets `pixels[p+x] |= 0xFF000000u`, and because
> libgd stores alpha reversed (0 = opaque, 127 = transparent) that makes the
> pixel fully transparent. A tool reporting libgd's scale will call that same
> pixel "alpha 127". An RGB-only comparison cannot distinguish the two modes —
> you must compare the alpha channel.

### DELOGO/BLUR/INTERPOLATE

Removes logos or overlays by interpolating from surrounding pixels.

### PATTERN/EXTRAPOLATE

Fills regions using pattern detection and extrapolation.

---

## Color Quantization

Color quantization methods reduce the number of colors in the output image to create an optimized palette.

### Median-cut (Heckbert)

Classic median-cut algorithm for palette generation.

**Option:** Default or specify with quantization settings

### Diversity

Selects colors based on diversity in the color space.

### Blend-diversity

Combines diversity selection with color blending.

### NeuQuant

Neural network-based quantization algorithm.

**Features:**
- High quality results
- Good for photographic images
- Slower than other methods

---

## Dithering

Dithering improves visual quality when reducing colors by spreading quantization error to neighboring pixels.

### Gamma Correction

Apply gamma correction during dithering for perceptually accurate color reduction.

**Option:** `--gamma VALUE`

### Dither Error Spread Factor

Controls how much quantization error spreads to neighboring pixels.

**Range:** 0.0 to 1.0
**Default:** Depends on dithering method

### Dither Matrix Size

Size of the dithering matrix (for ordered dithering).

**Options:** 2, 4, 8, 16, etc.

### Dither Candidate Count

Number of candidate colors to consider when dithering.

**Higher values:** Better quality but slower

### Dither Contrast Limiter

Limits contrast in dithering to avoid harsh artifacts.

---

## Color Compare Methods

Different color comparison methods for finding the nearest color in the palette.

### RGB

Simple Euclidean distance in RGB space.

**Formula:** `√((R₁-R₂)² + (G₁-G₂)² + (B₁-B₂)²)`

### CIE76

CIE 1976 color difference (ΔE*).

**Formula:** Distance in L*a*b* color space

### CIE94

CIE 1994 color difference formula.

**Features:**
- Improved perceptual uniformity
- Weighted differences for lightness, chroma, hue

### CIEDE2000

CIE 2000 color difference formula (ΔE₀₀).

**Features:**
- Most perceptually accurate
- Complex formula with multiple corrections
- Best for critical color matching

### CMC l:c

CMC (l:c) color difference formula.

**Parameters:**
- l: lightness weight
- c: chroma weight

### BFD l:c

British Flag Digitiser color difference formula.

### Illuminants

Different illuminant settings affect color comparison:
- **D65:** Daylight (6500K)
- **D50:** Horizon daylight (5000K)
- **A:** Incandescent (2856K)
- **C:** Average daylight

---

## Transformation

Mathematical pixel transformations can be applied before merging.

**Options include:**
- Rotation
- Scaling
- Translation
- Color space conversions

---

## Caveats

### Parallax Motion

**Issue:** Layers with different scroll speeds (parallax) can cause problems.

**Solution:** 
- Process each layer separately
- Use appropriate masking
- Adjust motion detection sensitivity

### Flashes, Fog and Transparent Layers

**Issue:** Temporary effects like flashes or fog can interfere with background detection.

**Solutions:**
- Use appropriate pixel methods (e.g., MOSTUSED instead of AVERAGE)
- Mask affected regions
- Pre-process to remove effects

---

## Usage

### Basic Command Structure

```bash
animmerger [options] input_files...
```

### Common Options

- `-pa` - Use AVERAGE method
- `-pm` - Use MOSTUSED method
- `-pc` - Use CHANGELOG method (animated)
- `-m x,y,w,h,colors...` - Mask region
- `--gif` - Output as GIF
- `--yuv` - Use YUV colorspace
- `--motionblur N` (or `-B N`) - Apply motion blur (1-16)
- `-f N` - Set first/last count for FIRSTNMOST/LASTNMOST
- `--gamma VALUE` - Set gamma correction

### Example Workflows

#### Extract Background from Animation

```bash
animmerger -pm frames/*.png -o background.png
```

#### Create Motion Blur Effect

```bash
animmerger -pa frames/*.png -o motion_blur.png
```

#### Generate Compact Animation

```bash
animmerger --gif -pc frames/*.png
gifsicle -O2 -o output.gif -l0 -d3 tile-*.gif
```

> **An animated method writes GIF even when `-o` ends in `.png`.** `-pc`, `-po`
> and `-pv` are animated, so the default `--gif=auto` selects GIF and `-o
> out.png` produces `out-0000.gif`. Use the `%3$s` escape, which expands to the
> extension actually written, or force PNG with `--gif=never`:
>
> ```bash
> animmerger -pc frames/*.png -o 'out-%04d.%3$s'             # GIF
> animmerger -pc frames/*.png -o 'out-%04d.png' --gif=never  # PNG
> ```
>
> The spelling matters: `--gif=never` works, `-g=never` does not. The short form
> hands `=never` — leading `=` included — to the parser, matches no valid value
> and aborts with exit 1 and `Invalid parameter to --gif: =never`, naming the
> long option you did not type.
>
> **Format escapes apply to output filenames only.** `%04d` and `%3$s` are
> substituted when animmerger writes output. A `%03d` in an *input* path is a
> literal filename and fails to open, so multi-frame input has to be spelled
> out by the shell.

#### Remove HUD and Extract Background

```bash
animmerger -pm frames/*.png -m0,8,256,16,FFFFFF,000000,FF0000 -o clean_background.png
```

> The colour list here is a filter — only those three colours are removed, and
> the rest of the rectangle is left as it was. Omit the list entirely to blank
> the whole rectangle. See [BLACK/BLANK/CENSOR](#blackblankcensor).

---

## Exit Codes

animmerger exits **0** on several genuine error paths: a file that is not an
image, a missing input file, and an unknown `--deltae` colour-compare method
each print a warning to stderr and still report success. Scripts must not treat
exit status as proof the run worked; check that the expected output file exists.
The behaviour is pinned by tests in `tests/` so that changing it is deliberate.

---

## Requirements

### Build Requirements

- C++ compiler with C++17 support (the Makefile passes `-std=gnu++1z`)
- Make
- OpenMP (required — the Makefile passes `-fopenmp` unconditionally)

### Runtime Requirements

- libgd (`-lgd`) — animmerger reads and writes images through libgd, not libpng
- For GIF output: GIF library or external tools like gifsicle

### Optional Tools

- **gifsicle** - For GIF optimization
- **imagemagick** - For format conversions

### Building and Testing

```bash
make          # produces ./animmerger
make check    # builds and runs the regression suite in tests/
make clean    # removes build output
```

On Arch/CachyOS install libgd with `pacman -S gd`; on Debian/Ubuntu,
`apt-get install libgd-dev`. Note the pkg-config name is `gdlib`, not `libgd`.

The suite in `tests/` links libgd and shells out to the real binary, so it
depends on nothing beyond what the main program already needs. `.github/workflows/build.yml`
runs `make` and `make check` on both GCC and Clang for every pull request.

> Overriding `CXXFLAGS` on the `make` command line replaces the Makefile's flags
> outright, `-std=gnu++1z` included, so `make CXXFLAGS="-g -O0"` does not
> compile. Pass the standard through: `make CXXFLAGS="-std=gnu++1z -fopenmp -g -O0"`.

---

## License

See the `COPYING` file for license information.

---

## See Also

- **Official Website:** http://bisqwit.iki.fi/source/animmerger.html
- **GitHub Repository:** https://github.com/bisqwit/animmerger
- **gifsicle:** http://www.lcdf.org/gifsicle/
- **JOURNAL.md:** fork maintenance history and open items

---

*This documentation is maintained to mirror the content from the official Animmerger website, making it accessible directly from the repository.*
