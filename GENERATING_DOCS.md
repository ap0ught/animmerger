# Documentation Generation Process

**Update (2026-02-03):** 
- ✅ The original PHP-based documentation generator has been fixed for PHP 8+ compatibility! 
- ✅ GitHub Pages automation is now set up to auto-publish documentation on each release!

**View Live Documentation:** [https://ap0ught.github.io/animmerger/](https://ap0ught.github.io/animmerger/)

---

This document explains how the documentation in this repository is created, published, and how it can be updated in the future.

## Source of Documentation

The official Animmerger documentation at http://bisqwit.iki.fi/source/animmerger.html is generated from PHP source files in this repository:

### Source Files

1. **`progdesc.php`** (1,382 lines)
   - Main documentation source file
   - Contains all text content, examples, and descriptions
   - Organized as a PHP array with hierarchical keys
   - Includes HTML markup and embedded images (hosted externally)

2. **`doc/docmaker.php`**
   - Generic documentation generator script by Bisqwit
   - Processes progdesc.php to generate HTML documentation
   - Version 1.2.1

3. **`doc/document.php`**
   - Document formatting module
   - Handles HTML rendering and structure
   - Version 2.0.7

## Original Documentation Generation

The website HTML can now be generated using:

```bash
php doc/docmaker.php animmerger.tar.bz2 progdesc.php > animmerger.html
```

**Note:** The PHP 8+ compatibility issues have been fixed (as of 2026-02-03). The script now works with modern PHP versions.

**Optional Dependencies:**
- `/usr/local/bin/animmerger` - Used to extract command-line help text (falls back gracefully if missing)
- `/usr/local/bin/htmlrecode` - Used for character encoding conversion (falls back gracefully if missing)

Previous limitations (now resolved):
- ~~Uses PHP's `each()` function (deprecated in PHP 7.2+)~~ ✅ Fixed - now uses `foreach()`

## How Repository Documentation Was Created

Since the original PHP-based documentation generator has compatibility issues with modern PHP and requires external dependencies, the documentation for this repository was created by:

### Step 1: Analyze progdesc.php Structure

```bash
# Extract all section keys from progdesc.php
php -r '$text=array(); include "progdesc.php"; foreach(array_keys($text) as $k) echo "$k\n";'
```

This revealed the complete documentation structure with ~52 sections covering:
- Purpose and overview
- 13 pixel methods (static and animated)
- Masking methods
- Color quantization algorithms  
- Dithering options
- Color comparison methods
- Transformation capabilities
- Usage examples and caveats

### Step 2: Extract Content

The content was extracted manually from progdesc.php by:

1. **Reading key sections** of progdesc.php (viewing lines 1-100, 100-200, etc.)
2. **Identifying structure** - Each key in the `$text` array represents a documentation section
3. **Converting HTML to Markdown** - HTML markup was converted to equivalent markdown
4. **Removing external image dependencies** - Image references were noted but not embedded
5. **Extracting command examples** - Code blocks were preserved and formatted

### Step 3: Supplement with Web Research

Since direct access to the website wasn't available, additional information was gathered using web search to ensure completeness:

```bash
# Information gathered about:
# - Pixel methods and their behaviors
# - Command-line options and syntax
# - Color comparison methods (CIE76, CIEDE2000, etc.)
# - Real-world use cases and examples
```

### Step 4: Create Markdown Documentation

Three comprehensive markdown files were created:

1. **`DOCUMENTATION.md`** (476 lines)
   - Complete reference documentation
   - All features, methods, and options
   - Derived from progdesc.php content

2. **`README.md`** (Enhanced, 153 lines)
   - Quick start guide
   - Common use cases
   - Feature overview
   - Links to detailed docs

3. **`EXAMPLES.md`** (450 lines)
   - Practical examples and tutorials
   - Real-world use cases
   - Batch processing scripts
   - Troubleshooting tips

## Documentation Structure Mapping

### progdesc.php Array Keys → Documentation Sections

```
'1. Purpose' → DOCUMENTATION.md ## Purpose
'methods:1. Pixel methods' → DOCUMENTATION.md ## Pixel Methods
  'static:1.1. Static methods' → ### Static Methods
    'average:1.1.1. AVERAGE' → #### AVERAGE
    'mostused:1.1.1. MOSTUSED' → #### MOSTUSED
    [... etc ...]
  'animated:1.1. Animated methods' → ### Animated Methods
    'changelog:1.1.1. CHANGELOG' → #### CHANGELOG
    [... etc ...]
'maskmethods:1. Masking methods' → ## Masking Methods
'palettemethods:1. Color quantization methods' → ## Color Quantization
'dither:1. Dithering' → ## Dithering
'dither_compare:1. Color compare methods' → ## Color Compare Methods
'transformation:1. Transformation' → ## Transformation
'caveats:1. Caveats' → ## Caveats
'usage:1. Usage' → ## Usage
```

## Updating Documentation

### Option 1: Update Markdown Files Directly

For quick updates or corrections:

1. Edit the relevant markdown file (`DOCUMENTATION.md`, `README.md`, or `EXAMPLES.md`)
2. Commit changes
3. No build process required

### Option 2: Update from progdesc.php

If progdesc.php is updated upstream:

1. **Manual extraction** (recommended for now):
   ```bash
   # View specific sections
   grep -A 50 "'average:" progdesc.php
   
   # Extract section by section and update markdown files
   ```

2. **Custom extraction script** (future improvement):
   ```bash
   # Could create a PHP or Python script to automate conversion
   # php extract_docs.php > new_DOCUMENTATION.md
   ```

### Option 3: Fix Original Generator ✅ COMPLETED

**Status:** The original HTML generator has been fixed and is now fully functional with modern PHP.

**Changes Made:**
1. **Fixed PHP 8+ compatibility issues in `doc/document.php`:**
   - Replaced the first deprecated `each()` loop over `$text` with `foreach($text as $title => $content)`
   - Replaced the second deprecated `each()` loop over `$text` with `foreach($text as $title => $content)`
   - Removed unnecessary `reset($text)` calls that were only needed for `each()`

**How to Use:**
1. **Generate HTML documentation:**
   ```bash
   php doc/docmaker.php animmerger.tar.bz2 progdesc.php > animmerger.html
   ```

2. **Optional - Handle external dependencies (for full functionality):**
   ```bash
   # For help text generation: Build and install animmerger
   make
   sudo cp animmerger /usr/local/bin/
   
   # For character encoding: Install htmlrecode (optional)
   # The script will work without this, but won't perform character encoding conversion
   ```

**Notes:**
- The generator now works with PHP 8.3+ without any fatal errors
- Minor warnings about undefined optional variables (`$title_ext`, `$ozi_pre`, `$ozi_ext`) are expected and harmless
- These variables are only set by progdesc.php in specific cases for customization
- The missing `/usr/local/bin/animmerger` and `/usr/local/bin/htmlrecode` are optional dependencies
  - They only affect help text inclusion and character encoding, not core documentation generation

## GitHub Pages Automation 🚀

**Status:** Automated documentation publishing is now configured!

**Live Documentation:** [https://ap0ught.github.io/animmerger/](https://ap0ught.github.io/animmerger/)

### How It Works

The repository now includes a GitHub Actions workflow (`.github/workflows/publish-docs.yml`) that:

1. **Triggers automatically** when a new release is published
2. **Generates HTML documentation** using the fixed PHP generator:
   ```bash
   php doc/docmaker.php animmerger.tar.bz2 progdesc.php > index.html
   ```
3. **Publishes to GitHub Pages** automatically

### Publishing Process

When you create a new release on GitHub:

1. Go to the GitHub repository → Releases → "Draft a new release"
2. Create a tag (e.g., `v1.0.0`) and fill in release details
3. Click "Publish release"
4. The workflow automatically runs and publishes updated documentation to GitHub Pages
5. Documentation is available at: `https://ap0ught.github.io/animmerger/`

### Manual Trigger

You can also manually trigger documentation publishing:

1. Go to GitHub repository → Actions → "Publish Documentation to GitHub Pages"
2. Click "Run workflow"
3. Select the branch and click "Run workflow"

### Configuration Requirements

For the workflow to work properly, ensure:

1. **GitHub Pages is enabled** in repository settings:
   - Settings → Pages → Source: "GitHub Actions"

2. **Workflow permissions** are set correctly:
   - Settings → Actions → General → Workflow permissions: "Read and write permissions"

### Advantages

- ✅ **Always up-to-date:** Documentation automatically updates with each release
- ✅ **No manual steps:** Just publish a release, documentation follows
- ✅ **Version control:** Each release gets its documentation snapshot
- ✅ **Public access:** Documentation is publicly accessible via GitHub Pages
- ✅ **Free hosting:** No external hosting costs or dependencies

## Version Control

### Tracking Documentation Changes

- **progdesc.php** - Source of truth for official documentation
- **DOCUMENTATION.md, README.md, EXAMPLES.md** - Derived documentation in markdown
- Update markdown files when progdesc.php changes upstream
- Document any deviations or additions

### Synchronization

To check if progdesc.php has changed:

```bash
# Check last modification
git log -1 --format="%ai" progdesc.php

# Compare with last documentation update
git log -1 --format="%ai" DOCUMENTATION.md

# If progdesc.php is newer, consider updating markdown docs
```

## Advantages of Markdown Documentation

The markdown-based documentation has several advantages over the HTML generation:

1. **No build dependencies** - No PHP script execution needed
2. **Modern PHP compatible** - Not affected by deprecated functions
3. **Git-friendly** - Easy to diff and track changes
4. **GitHub rendering** - Automatically rendered on GitHub
5. **Portable** - Works everywhere markdown is supported
6. **Searchable** - Easy to grep and search
7. **No external dependencies** - Self-contained in repository

## Future Improvements

### Automated Conversion Tool

Create a script to automate progdesc.php → markdown conversion:

```python
#!/usr/bin/env python3
# convert_docs.py - Convert progdesc.php to markdown

import re
import sys

def parse_progdesc(filename):
    """Parse progdesc.php and extract documentation."""
    # Read PHP array
    # Convert HTML to markdown
    # Generate structured markdown
    pass

def generate_documentation_md(sections):
    """Generate DOCUMENTATION.md from parsed sections."""
    pass

if __name__ == '__main__':
    sections = parse_progdesc('progdesc.php')
    markdown = generate_documentation_md(sections)
    print(markdown)
```

### Documentation Tests

Add validation to ensure documentation stays in sync:

```bash
#!/bin/bash
# test_docs.sh - Validate documentation

# Check that all pixel methods from progdesc.php are documented
PROG_METHODS=$(grep -o "1.1.1. [A-Z]*'" progdesc.php | cut -d' ' -f2 | tr -d "'")
DOC_METHODS=$(grep "^#### " DOCUMENTATION.md | cut -d' ' -f2)

# Compare and report missing methods
```

## References

- **Original documentation source:** `progdesc.php`
- **Generated HTML:** http://bisqwit.iki.fi/source/animmerger.html  
- **Markdown documentation:** `DOCUMENTATION.md`, `README.md`, `EXAMPLES.md`
- **Generator scripts:** `doc/docmaker.php`, `doc/document.php`

---

*Last updated: 2026-02-03*
