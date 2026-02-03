# Animmerger

Animmerger stitches 2D images together, forming either a static image or an animation, while attempting to preserve a global frame of reference (static background).

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
- **LOOPINGLOG** - Optimized for looping animations
- **LOOPINGAVG** (`-pv`) - Looping with motion blur

## Documentation

- **[DOCUMENTATION.md](DOCUMENTATION.md)** - Complete documentation with all features, methods, and examples
- **[Official Website](http://bisqwit.iki.fi/source/animmerger.html)** - Original documentation source
- **[doc/AddingPixelMethods.txt](doc/AddingPixelMethods.txt)** - Guide for developers adding new pixel methods

## Requirements

- C++ compiler with C++11 support (GCC 4.8+, Clang 3.4+)
- Make
- PNG library (libpng)
- OpenMP (optional, for parallel processing)
- gifsicle (optional, for GIF optimization)

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
- `--blur N` - Add motion blur (N = 1-16)
- `--gamma VALUE` - Apply gamma correction
- `-f N` - Set first/last count for FIRSTNMOST/LASTNMOST methods
- `-m x,y,w,h,colors...` - Mask region with specific parameters

## Contributing

See `COPILOT_README.md` and `copilot-docs/` for development documentation.

## License

See the `COPYING` file for license information.

## Links

- **Official Website:** http://bisqwit.iki.fi/source/animmerger.html
- **GitHub (Official):** https://github.com/bisqwit/animmerger
- **gifsicle:** http://www.lcdf.org/gifsicle/
