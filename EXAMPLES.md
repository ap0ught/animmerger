# Animmerger Examples

This document provides practical examples of using Animmerger for various tasks.

## Table of Contents

1. [Video Game Level Mapping](#video-game-level-mapping)
2. [Motion Blur Effects](#motion-blur-effects)
3. [HUD and Overlay Removal](#hud-and-overlay-removal)
4. [Animation Optimization](#animation-optimization)
5. [Background Extraction](#background-extraction)
6. [Sprite and Asset Processing](#sprite-and-asset-processing)

---

## Video Game Level Mapping

### Basic Level Map from Emulator Screenshots

Capture frames from an emulator and create a complete level map:

```bash
# Assuming you have frames: frame0001.png, frame0002.png, etc.
animmerger -pm frames/*.png -o level_map.png
```

### Remove HUD While Mapping

Remove the HUD overlay while extracting the background:

```bash
# HUD at top: position (0,8), size 256x16
# Specify colors to remove: black, orange, gold, blue, brown, white
animmerger -pm frames/*.png \
  -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF \
  -o clean_level_map.png
```

### Super Mario Bros. Example

```bash
# Extract SMB level with HUD removal
animmerger -pm smb_frames/*.png \
  -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF \
  -o mario_world_8-2.png
```

---

## Motion Blur Effects

### Simple Motion Blur

Create a motion blur effect showing all movement:

```bash
animmerger -pa input/*.png -o motion_blur.png
```

### Action-Focused Motion Blur

Use ACTIONAVG to keep the background clear while blurring moving objects:

```bash
animmerger -pt input/*.png -o action_blur.png
```

### YUV Colorspace Blur

Calculate averages in YUV colorspace for more perceptually accurate results:

```bash
animmerger -pa --yuv input/*.png -o yuv_blur.png
```

### Memory-Efficient Blur

For large image sets, use tinyaverage:

```bash
animmerger -A input/*.png -o tiny_blur.png
```

---

## HUD and Overlay Removal

### Remove Simple Rectangular HUD

```bash
# Remove HUD at position (0, 0) with size 320x40
animmerger -pm frames/*.png -m0,0,320,40 -o no_hud.png
```

### Remove HUD with Specific Colors

Only remove specific colors within the HUD region:

```bash
# Remove white text and specific UI colors
animmerger -pm frames/*.png \
  -m0,8,256,16,FFFFFF,FF0000,00FF00,0000FF \
  -o selective_removal.png
```

### Remove Multiple Overlays

Specify multiple mask regions:

```bash
# Remove top HUD and bottom status bar
animmerger -pm frames/*.png \
  -m0,0,320,40,FFFFFF,000000 \
  -m0,440,320,40,FFFFFF,000000 \
  -o double_removal.png
```

### Logo/Watermark Removal with Interpolation

Use interpolation masking for better results:

```bash
animmerger -pm frames/*.png \
  --mask-interpolate 10,10,100,50 \
  -o delogo.png
```

---

## Animation Optimization

### Create Space-Efficient Animation

Convert a series of frames to a compact animation with fixed background:

```bash
# Generate tiles
rm tile-*.png tile-*.gif
animmerger --gif -pc frames/*.png \
  -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF

# Optimize with gifsicle
gifsicle -O2 -o optimized.gif -l0 -d3 tile-*.gif
```

### Looping Animation

Create a perfectly looping animation:

```bash
animmerger --gif -pl frames/*.png
gifsicle -O2 -o loop.gif -l0 -d3 tile-*.gif
```

### Animation with Motion Blur

Add motion blur to the animation:

```bash
# Blur level 4 (1-16 scale)
animmerger --gif -pc --blur 4 frames/*.png
gifsicle -O2 -o blur_animation.gif -l0 -d3 tile-*.gif
```

### Looping Animation with Motion Blur

```bash
animmerger --gif -pv --blur 8 frames/*.png
gifsicle -O2 -o blur_loop.gif -l0 -d3 tile-*.gif
```

---

## Background Extraction

### Extract Most Common Pixels (Background)

```bash
animmerger -pm frames/*.png -o background.png
```

### Extract First Appearance

Get whatever first appeared at each location (useful for scrolling reveals):

```bash
animmerger -pf frames/*.png -o first_reveal.png
```

### Extract Last Appearance

Get the final state at each location:

```bash
animmerger -pl frames/*.png -o final_state.png
```

### Solid Background (Lightweight)

Use the SOLID method for a faster, lighter alternative to MOSTUSED:

```bash
animmerger -pO frames/*.png -o solid_bg.png
```

### Most Common of First N Frames

Extract background based only on the first N pixel appearances:

```bash
# Use first 10 appearances of each pixel
animmerger -pF -f10 frames/*.png -o first10_bg.png
```

### Most Common of Last N Frames

Extract background based on the last N pixel appearances:

```bash
# Use last 10 appearances of each pixel
animmerger -pL -f10 frames/*.png -o last10_bg.png
```

---

## Sprite and Asset Processing

### Extract Static Sprites

Find static elements (enemies, objects that don't move):

```bash
animmerger -pm sprite_frames/*.png -o static_sprites.png
```

### Find Moving Elements

Use LEASTUSED to identify moving/animated elements:

```bash
animmerger -pe frames/*.png -o moving_elements.png
```

### Sprite Sheet with Fixed Background

```bash
# Create animated sprite sheet
animmerger --gif -pc sprite_frames/*.png

# Combine into sprite sheet
gifsicle -o sprite_sheet.gif -l0 -d5 tile-*.gif
```

---

## Advanced Techniques

### Parallax Layer Separation

For scenes with parallax scrolling, process each layer separately:

```bash
# Process foreground layer
animmerger -pm foreground/*.png -o fg_layer.png

# Process background layer  
animmerger -pm background/*.png -o bg_layer.png

# Combine layers externally (using imagemagick, etc.)
```

### High-Quality Color Quantization

Use NeuQuant for best quality when reducing colors:

```bash
animmerger -pm frames/*.png --quantize neuquant --colors 256 -o quantized.png
```

### Gamma-Corrected Processing

Apply gamma correction for perceptually accurate color processing:

```bash
animmerger -pm frames/*.png --gamma 2.2 -o gamma_corrected.png
```

### Perceptual Color Matching

Use CIEDE2000 for most accurate color comparison:

```bash
animmerger -pm frames/*.png --color-compare ciede2000 -o perceptual.png
```

### Advanced Dithering

Fine-tune dithering parameters:

```bash
animmerger -pm frames/*.png \
  --colors 16 \
  --dither-error 0.8 \
  --dither-matrix 8 \
  --dither-candidates 8 \
  --gamma 2.2 \
  -o dithered.png
```

---

## Batch Processing

### Process Multiple Animations

```bash
#!/bin/bash
for dir in level*/; do
  echo "Processing $dir"
  animmerger -pm "$dir"/*.png -o "output/${dir%/}_map.png"
done
```

### Different Methods on Same Input

```bash
#!/bin/bash
methods=("pm" "pa" "pf" "pl" "pO")
names=("mostused" "average" "first" "last" "solid")

for i in "${!methods[@]}"; do
  animmerger -"${methods[$i]}" frames/*.png -o "method_${names[$i]}.png"
done
```

### Batch HUD Removal

```bash
#!/bin/bash
# Process multiple game recordings with same HUD
for recording in recording_*/frames; do
  output="${recording%/frames}_map.png"
  animmerger -pm "$recording"/*.png \
    -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF \
    -o "$output"
done
```

---

## Troubleshooting Examples

### Low Memory Situation

Use tinyaverage or solid methods:

```bash
# Instead of -pa (average)
animmerger -A frames/*.png -o output.png

# Or use solid
animmerger -pO frames/*.png -o output.png
```

### Dealing with Flashing Effects

Use MOSTUSED or SOLID to ignore temporary flashes:

```bash
animmerger -pm frames/*.png -o no_flash_bg.png
```

### Handling Transparent Layers

Process with alpha channel support:

```bash
animmerger -pm frames/*.png --alpha -o transparent_bg.png
```

### Very Large Animations

Process in chunks and combine:

```bash
# Process first half
animmerger -pm frames/0*.png frames/1*.png -o part1.png

# Process second half
animmerger -pm frames/2*.png frames/3*.png -o part2.png

# Combine externally
```

---

## Performance Tips

1. **Use appropriate pixel methods:** SOLID and LAST are faster than MOSTUSED
2. **Limit input files:** Process in batches if you have thousands of frames
3. **Use tinyaverage:** For memory-constrained systems
4. **Enable OpenMP:** Compile with OpenMP support for parallel processing
5. **Reduce color depth early:** If you don't need full color, quantize during processing

---

## Real-World Use Cases

### Case Study 1: NES Game Mapping

```bash
# Mapping Super Mario Bros levels
# Captured 500+ frames from emulator

animmerger -pm smb_8-2/*.png \
  -m0,8,256,16,020202,A64010,D09030,006E84,511800,FFFFFF \
  -o smb_8-2_complete_map.png

# Result: Complete level map, 724x224 pixels
# Original: 500 frames × 256×224 = 28.4 MB
# Result: Single PNG = 47 KB
```

### Case Study 2: Security Camera Analysis

```bash
# Extract static background from security footage
# Remove people and moving objects

animmerger -pm security_footage/*.png -o empty_scene.png

# Find all movement paths
animmerger -pa security_footage/*.png -o movement_trails.png
```

### Case Study 3: Time-Lapse Background

```bash
# Create clean background from time-lapse photos
# Remove people/cars passing through

animmerger -pm timelapse/*.jpg -o clean_background.jpg

# Show all activity as motion blur
animmerger -pt timelapse/*.jpg -o activity_map.jpg
```

---

*For more detailed information about options and methods, see [DOCUMENTATION.md](DOCUMENTATION.md).*
