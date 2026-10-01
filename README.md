# Animmerger

Animmerger stitches 2D images together, forming either a static image or an animation, while attempting to preserve a global frame of reference (static background).

**📖 Full HTML Documentation:** [https://ap0ught.github.io/animmerger/](https://ap0ught.github.io/animmerger/)  
**📚 Additional Docs:** [DOCUMENTATION.md](DOCUMENTATION.md) | [EXAMPLES.md](EXAMPLES.md)

## Features

- **Advanced Image Stitching:** Combines multiple 2D images into panoramic stills or animations
- **Motion Detection:** Automatically detects and compensates for camera movement
- **Multiple Pixel Methods:** Static methods (AVERAGE, MOSTUSED, FIRST, LAST, etc.) and animated methods (CHANGELOG, LOOPINGLOG, etc.)
- **Masking Capabilities:** Remove HUDs, logos, and unwanted overlays
- **Extensive Dithering:** Floyd-Steinberg, ordered dithering, and more
- **Color Quantization:** Multiple algorithms including Median-cut, Diversity, and NeuQuant
- **Perceptual Color Comparison:** Supports RGB, CIE76, CIE94, CIEDE2000, CMC, and BFD
- **Infinite 2D Canvas:** Simulates an infinite canvas that extends in all directions

## Quick Start

### Build

```bash
make
```

### Basic Usage

```bash
# Extract background from animation frames
animmerger -pm frames/*.png -o background.png

# Create motion blur effect
animmerger -pa frames/*.png -o motion_blur.png

# Generate compact animation
animmerger --gif -pc frames/*.png
```

## Common Use Cases

### Extract Static Background

Extract the background from a scrolling scene (e.g., video game footage):

```bash
animmerger -pm screenshots/*.png -o level_map.png
```

### Remove HUD Elements

Remove heads-up display (HUD) elements while processing:

```bash
# Remove a HUD at position 0,8 with size 256x16 and specific colors
animmerger -pm frames/*.png -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF
```

### Create Motion Trails

Generate an image showing all movement paths:

```bash
animmerger -pa frames/*.png -o motion_trails.png
```

### Generate Compact Animation

Create a space-efficient animation with a fixed background:

```bash
animmerger --gif -pc frames/*.png
gifsicle -O2 -o output.gif -l0 -d3 tile-*.gif
```

## Pixel Methods

### Static Methods (Single Frame Output)

- **AVERAGE** (`-pa`) - Motion blur effect averaging all frames
- **MOSTUSED** (`-pm`) - Extracts the most common pixel values (background)
- **ACTIONAVG** (`-pt`) - Average with separate background tracking
- **FIRST** (`-pf`) - Shows first appearance of each pixel
- **LAST** (`-pl`) - Shows last appearance of each pixel
- **SOLID** (`-pO`) - Finds longest consecutive sameness
- **FIRSTNMOST** (`-pF`) - Most common of first N pixels
- **LASTNMOST** (`-pL`) - Most common of last N pixels

### Animated Methods (Animation Output)

- **CHANGELOG** (`-pc`) - Full animation with fixed background
- **LOOPINGLOG** (`-s` or `-po`) - Optimized for looping animations
- **LOOPINGAVG** (`-pv`) - Looping with motion blur and averaging

## Documentation

- **[DOCUMENTATION.md](DOCUMENTATION.md)** - Complete documentation with all features, methods, and examples
- **[Official Website](http://bisqwit.iki.fi/source/animmerger.html)** - Original documentation source
- **[doc/AddingPixelMethods.txt](doc/AddingPixelMethods.txt)** - Guide for developers adding new pixel methods
- **[JOURNAL.md](JOURNAL.md)** - Fork maintenance history: findings, ruled-out hypotheses, and open items

## Requirements

- C++ compiler with C++17 support (the Makefile passes `-std=gnu++1z`)
- Make
- **libgd** (`-lgd`) — animmerger reads and writes images through libgd, not libpng
- **OpenMP** (required, not optional — the Makefile passes `-fopenmp` unconditionally and defines the thread-safe OpenMP paths in `alloc/FSBAllocator.hh`)
- gifsicle (optional, for GIF optimization)
- php (optional, only to regenerate `doc/README.html` via `doc/docmaker.php`)

On Arch/CachyOS: `pacman -S gd`. On Debian/Ubuntu: `apt-get install libgd-dev`.

## Building

```bash
make            # produces ./animmerger
make check      # builds and runs the test suite (see tests/)
make clean      # removes build output; the generated-file list is explicit
```

`.github/workflows/build.yml` builds this on both GCC and Clang on every pull
request, runs `make check`, and then runs `./animmerger --version` and
`--longhelp` — because a binary that links but cannot start is not a build.

> If you override `CXXFLAGS` on the `make` command line it replaces the
> Makefile's flags outright, including `-std=gnu++1z`. `make CXXFLAGS="-g -O0"`
> therefore does not compile; pass the standard through too:
> `make CXXFLAGS="-std=gnu++1z -fopenmp -g -O0"`.

## Testing

`tests/test_animmerger.cc` is a self-contained C++ harness. It links libgd —
already a hard dependency — and uses it to synthesise fixtures and inspect
output, and it shells out to the real `./animmerger` so the assertions cover the
actual command-line surface rather than internal functions. Scratch files go to
`tests/out/`.

```bash
make check                                  # build + run everything
make clean-tests                            # drop the harness, keep ./animmerger
./tests/test_animmerger                     # run an already-built suite
```

It was validated by mutation testing: deliberately injected regressions
(`--dm` parsed then discarded, the pow2 dither warning suppressed, `-u censor`
silently degraded to `-u hole`, the unreadable-input warning suppressed, the
default output template losing its frame numbering, `--gif=never` ignored) were
each caught by a named test.

## Examples

### Example 1: Video Game Level Mapping

```bash
# Capture frames from emulator
# frames/0001.png, frames/0002.png, etc.

# Generate level map
animmerger -pm frames/*.png -o mario_level_8-2.png
```

### Example 2: Remove Logo/Watermark

```bash
# Use interpolation masking to remove watermark
animmerger -pm frames/*.png --mask-interpolate 10,10,100,50 -o clean.png
```

### Example 3: Create Animated Sprite Sheet

```bash
# Generate fixed-background animation
animmerger --gif -pc sprites/*.png
gifsicle -O2 -o sprite_animation.gif -l0 -d5 tile-*.gif
```

## Advanced Options

- `--yuv` - Calculate averages in YUV colorspace instead of RGB
- `--motionblur N` (or `-B N`) - Add motion blur (N = 1-16)
- `--gamma VALUE` - Apply gamma correction
- `-f N` - Set first/last count for FIRSTNMOST/LASTNMOST methods
- `-m x,y,w,h,colors...` - Mask region with specific parameters

## Contributing

See `COPILOT_README.md` and `copilot-docs/` for development documentation.

## Known quirks

Four behaviours where the option syntax reads one way and the code does another.
All four are pinned by tests in `tests/`, so none can change silently.

**The colour list in `-m x,y,w,h,C1,C2` is a filter, not a fill colour.** It
lists colours to *remove*; everything else in the rectangle is left alone.
`mask.cc:565` gates on `a.colors.empty() || a.colors.find(...) != end()`, so:

| command | effect on a 16×16 rect, left half red, right half blue |
|---|---|
| `-m0,0,16,16` | whole rectangle blanked |
| `-m0,0,16,16,FF0000` | only the red half blanked; blue survives |
| `-m0,0,16,16,00FF00` | nothing blanked — no pixel matches |

To blank an entire rectangle, pass **no** colour list. To blank only particular
colours, list exactly those and nothing else.

**`-u censor` and `-u hole` differ in alpha, not in RGB.** Both write black. In
the PNG file that is `rgba(0,0,0,255)` for `censor` and `rgba(0,0,0,0)` for
`hole` — i.e. `mask.cc:572` does `pixels[p+x] |= 0xFF000000`, which is libgd's
*reversed* alpha, so `hole` is fully transparent. (libgd's own alpha range is
0–127 with 127 meaning transparent; a tool reporting that scale will say
"alpha 127" for the same pixel.) An RGB-only comparison cannot tell the two
modes apart — compare the alpha channel.

**An animated method writes GIF even when the output name ends in `.png`.**
`-pc`/`-po`/`-pv` are animated, so the default `--gif=auto` picks GIF and
`-o out.png` yields `out-0000.gif`. Use the `%3$s` escape, which expands to the
real extension, and `--gif=never` to force PNG:

```bash
animmerger -pc frames/*.png -o 'out-%04d.%3$s'      # GIF, extension follows
animmerger -pc frames/*.png -o 'out-%04d.png' --gif=never   # PNG
```

Note the spelling: **`--gif=never` works, `-g=never` does not.** The short form
passes `=never` (leading `=` included) as the argument, matches no valid value,
and aborts with `Invalid parameter to --gif: =never` and exit 1 — while naming
`--gif` in a message you did not type.

**`%04d` expands in output filenames only.** A `%03d` inside an *input* path is
a literal filename, so it fails to open (and, see below, exits 0). Multi-frame
input must be spelled out with a shell glob:

```bash
animmerger -pm frames/*.png -o out.png      # correct
animmerger -pm 'frames/f%03d.png' ...      # looks for a file literally named "f%03d.png"
```

### Exit codes are not trustworthy

animmerger exits **0** on several genuine error paths: a file that is not an
image, a missing input file, and an unknown `--deltae` colour-compare method all
print a warning and still report success. Anything scripting animmerger must not
rely on its exit status to detect these; check that the expected output file
exists. Three tests (`input/unreadable-file-warns`, `input/missing-file-warns`,
`colordiff/unknown-method-is-rejected`) pin this current behaviour on purpose,
so that changing it later is a deliberate act rather than an accident.

## Upstream and syncing

This is a fork of [bisqwit/animmerger](https://github.com/bisqwit/animmerger).
Two things are worth knowing before any sync:

- **`upstream/master` is already fully contained here.** There is nothing to
  merge; `git merge-base --is-ancestor upstream/master master` succeeds and the
  fork is 19 commits ahead, 0 behind. Requests to "merge upstream in" have
  historically been based on that false premise.
- **Joel also maintains `git://bisqwit.iki.fi/animmerger.git`, which is alive
  and one commit ahead of the GitHub mirror.** The missing commit (`e88e939`,
  "Fix Q option error checking") is a real fix: the old `-Q` validation rejected
  any palette with fewer than two levels per channel, while the new one rejects
  the case that actually produces a degenerate palette. Taking it is the
  maintainer's call — see `JOURNAL.md`.

Do not "fix" the `git://bisqwit.iki.fi/animmerger.git` URL in `progdesc.php:6`.
iki.fi runs its own git daemon; it was GitHub that disabled the `git://`
protocol.

## License

See the `COPYING` file for license information.

## Links

- **Official Website:** http://bisqwit.iki.fi/source/animmerger.html
- **GitHub (Official):** https://github.com/bisqwit/animmerger
- **gifsicle:** http://www.lcdf.org/gifsicle/
