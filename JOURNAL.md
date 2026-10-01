# Ap0ught/animmerger fork journal

Working notes so a future session starts from the findings instead of
rediscovering them. Newest entry at the bottom. Times UTC.

This file records the *fork's* maintenance history — triage of the automated
pull requests, build breakage, the test suite and CI. It is not part of upstream
`bisqwit/animmerger`, and nothing in it is a statement about that project's
history.

Clone used for the work: `/tmp/opencode/animmerger` — **a scratch clone under
`/tmp`; re-clone if it is gone.** Remotes: `origin` = `ap0ught/animmerger`,
`upstream` = `bisqwit/animmerger`, `iki` = a local mirror of
`git://bisqwit.iki.fi/animmerger.git`, `bisqwit-anim` = the GitHub upstream.

---

## 2026-09-30 — triage the three Copilot PRs, fix the broken build, add tests and CI

Branch `fix/build-and-docs`, 6 commits, PR
**https://github.com/ap0ught/animmerger/pull/10**. As of this entry the PR is
**open and unmerged**, and the three Copilot PRs it replaces (#5, #7, #9) were
**closed unmerged** at `2026-09-30T18:16:12Z`–`18:16:15Z`, roughly two minutes
after PR #10 was opened. Nothing was merged from any of the four.

### Where things stood

Branch `fix/build-and-docs` at `78d7bcf` ("Fix the build workflow:
`matrix.compiler` is an object, not a string"), 6 commits ahead of
`origin/master` (`03de0f6`), 0 behind. CI green on both matrix jobs:

```
$ gh pr checks 10 --repo ap0ught/animmerger
clang  pass  52s  .../actions/runs/36757844138/job/110032518656
gcc    pass  42s  .../actions/runs/36757844138/job/110032519111
```

The whole tree built and the suite passed when re-checked during this
documentation pass:

```
$ make            # exit 0, ~1m17s, 1987 warning lines
$ make check      # 29 passed, 0 failed
```

### Findings

**Blocker — `master` does not compile at all.** Verified in a throwaway
worktree at `origin/master`:

```
$ git worktree add /tmp/opencode/probe/master-wt origin/master
$ cd /tmp/opencode/probe/master-wt && make
pixel.cc:166:46: error: narrowing conversion of '2' from 'long unsigned int' to 'bool' [-Wnarrowing]
... 98 such errors, all at pixel.cc:166 ...
MAKE_EXIT=2
```

All 98 are the same defect. `MakeMethodCaller` (defined `pixel.cc:143`) used
`PixelMetaInfo<T>::Traits & (1u << pm_##name##Pixel)` as a non-type template
argument of type `bool`. `Traits` is wider than `bool`, so that is a narrowing
conversion, which is ill-formed in a template argument list — GCC reports it as
an **error**, not a warning, so `make` fails outright. Fixed in `9ab8329` by
comparing instead of narrowing: `(PixelMetaInfo<T>::Traits & (1u << pm_##name##Pixel)) != 0`.

Upstream `bisqwit/animmerger#2` works around the same error by adding
`-Wno-narrowing` to `CXXFLAGS`, which silences the whole warning class
project-wide. Fixing the one expression is better.

**The build requirements were wrong in four files.** They said libpng (the
Makefile links `-lgd`), OpenMP optional (`-fopenmp` is unconditional and
`alloc/FSBAllocator.hh` is compiled with
`FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OPENMP`), and C++11 (`-std=gnu++1z` is
C++17). Files: `README.md`, `DOCUMENTATION.md`, `COPILOT_README.md`,
`copilot-docs/workflow.md`. Corrected in `f1e49dd`. Note the pkg-config name
for libgd is `gdlib`, not `libgd`.

**There was no `.gitignore` at all**, which is why PR #5 committed 36.6 MiB of
build output — `animmerger` (12,343,232 bytes), ten `.o` files, `.depend`, and
1386 lines of generated `doc/README.html`. The binary blobs alone total
38,399,872 bytes (36.62 MiB). Added in `d217855`.

**There was no `make clean` target**, so the only way to empty a dirty tree was
`git clean -xdf`, which also destroys untracked source. Added in `76b490b` with
an explicit generated-file list.

**`copilot-docs` documented a `make test` target that has never existed** (three
sites: `workflow.md` Build-and-Test, Best-Practices, and Testing Workflow). The
real target is `make check`. Corrected during this documentation pass.

**Every `make CXXFLAGS=…` in the docs was a broken build command.** A
command-line `CXXFLAGS` replaces the Makefile's assignments outright,
`-std=gnu++1z` included, so `make CXXFLAGS="-g -O0"` fails to compile:

```
$ make clean && make CXXFLAGS="-g -O0"
range.hh:47:23: error: no class template named 'rebind' in 'class std::allocator<...>'
rangemap.hh:45:65: error: using invalid field 'rangemap<...>::const_iterator::i'
make: *** [<builtin>: main.o] Error 1     (MAKE_EXIT=2)

$ make clean && make CXXFLAGS="-std=gnu++1z -fopenmp -g -O0"   # 0 errors
```

This was in `workflow.md` (debug and profiling recipes, twice more in
troubleshooting), `common-issues.md` (twice), and `README.md`/`DOCUMENTATION.md`
now carry the warning. Worth remembering this trap generally: `CXXFLAGS +=` in a
Makefile plus `make CXXFLAGS=…` is a silent footgun.

**animmerger exits 0 on several genuine error paths.** Measured directly:

| input | message | exit |
|---|---|---|
| file that is not an image | `has unrecognized image type, ignoring file` | **0** |
| missing input file | `No such file or directory` | **0** |
| unknown `--deltae` method | (warns) | **0** |
| `-g=never` | `Invalid parameter to --gif: =never` | **1** |

Anything scripting animmerger cannot trust its exit status. Deliberately not
fixed — changing exit codes is a behaviour change that does not belong in a
build-fix PR. Pinned by `input/unreadable-file-warns`,
`input/missing-file-warns` and `colordiff/unknown-method-is-rejected` so a
future change has to be deliberate.

**`animmerger_nes` is an unfulfilled target, not a vestigial one.** The Makefile
builds it with `-DNESmode=1`, but `NESmode` appears nowhere in the current
sources — the only occurrence in the whole tree is `Makefile:144` itself. So
`make animmerger_nes` produces a binary identical to plain `animmerger`. The
implementation exists only on the `WIP_nesmode` branch (commit `2f512f6`,
2012-04-14, touching exactly `Makefile`/`canvas.cc`/`palette.cc`, where
`canvas.cc` carries five `NESmode` references — a `#ifndef`/`# define` pair at
lines 18–19 plus three `#if` reads). By contrast `animmerger_cga16` **is** live:
`CGA16mode` defaults to 0 at `canvas.cc:19` and is read at
`canvas.cc:786,889,903`.

**The GitHub mirror of upstream is one commit behind Joel's own git.** `iki.fi`
runs a live git daemon (verified: `git ls-remote git://bisqwit.iki.fi/animmerger.git`
answers, exit 0). It was GitHub that disabled `git://`, not iki.fi.

```
upstream/master (GitHub):  6fccae2  "Update README.md"
iki.fi        master:      e88e939  "Fix Q option error checking"
ahead/behind (github-only / iki-only): 0 / 1
```

`e88e939` is a real fix — the old check rejected any `-Q R<GxB` palette with
fewer than two levels per channel; the new one drops that per-channel floor and
instead rejects the case that actually yields a degenerate palette:

```diff
-  if(r_dim < 2 || g_dim < 2 || b_dim < 2
-  || r_dim > 256 || g_dim > 256 || b_dim > 256)
+  if(r_dim > 256 || g_dim > 256 || b_dim > 256
+  || r_dim*g_dim*b_dim*i_dim <= 1)
```

It also swaps a dead `advsys.net/ken/utils.htm` (pngout) link for
`https://github.com/google/zopfli`. **Deliberately left out of PR #10** and
posted as a comment on issue #4 instead: choosing which upstream to trust is the
maintainer's call, not something to slip into a build-fix PR. Recommendation
given: take the commit.

**Four behaviours where the manual reads differently from the code.** Found
while writing assertions; all four are now documented in `README.md`
("Known quirks") and `DOCUMENTATION.md`, and each is pinned by a test.

1. **The colour list in `-m x,y,w,h,C1,C2` is a filter, not a fill colour.**
   `mask.cc:565` gates on `a.colors.empty() || a.colors.find(...) != end()`.
   Measured on a 16×16 rect, left half red, right half blue:
   `-m0,0,16,16` blanks everything; `-m0,0,16,16,FF0000` blanks **only the red
   half**; `-m0,0,16,16,00FF00` blanks **nothing**. To blank a whole rectangle
   you pass no list at all.
2. **`-u censor` and `-u hole` differ in alpha, not RGB.** Measured from the
   output PNGs: `censor` leaves `rgba(0,0,0,255)`, `hole` leaves
   `rgba(0,0,0,0)`. `mask.cc:572` does `pixels[p+x] |= 0xFF000000u`, and libgd
   stores alpha *reversed*, so that bit pattern is fully transparent. A tool
   reporting libgd's 0–127 scale calls the same pixel "alpha 127" — the two
   conventions describe one value and it is worth not conflating them. An
   RGB-only comparison cannot tell the modes apart.
3. **An animated method writes GIF even when `-o` ends in `.png`.** `-pc`/`-po`/`-pv`
   are animated, so `--gif=auto` picks GIF and `-o out.png` yields
   `out-0000.gif`. Escapes are `%3$s` in the output template and `--gif=never`
   to force PNG.
4. **`%04d` expands in output filenames only.** A `%03d` in an *input* path is a
   literal filename: `frames/f%03d.png: No such file or directory`, exit 0.
   Multi-frame input must be spelled out with a shell glob.

Related, and corrected during this pass: **`-g=never` does not work** — it hands
`=never` (leading `=` included) to the parser, matches nothing, and aborts with
exit 1 while naming `--gif` in a message you did not type. Only `--gif=never`
works. (The session notes this was first recorded as "produces no file at all";
that is wrong, and the table above is the measured behaviour.)

### Ruled out

**"Merge upstream into the fork."** A no-op. `upstream/master` is already fully
contained:

```
$ git rev-list --left-right --count origin/master...upstream/master
19	0
$ git merge-base --is-ancestor upstream/master origin/master   # exit 0
```

19 ahead, 0 behind. Issue #4 was written on the premise that upstream had moved
on; it had not. Correcting that was part of the job.

**A rename-induced 404 hunt.** The URL-liveness audit came back clean: only
`bisqwit/animmerger` is referenced anywhere (`README.md`,
`DOCUMENTATION.md`), it exists and has 24 distinct tags, and there are no
`releases/download/` URLs in the tree at all — so neither upstream's zero GitHub
releases nor the fork's zero tags breaks anything. **No blanket owner rename
ever happened in this fork**, which is the opposite of the situation the URL
skill is written for. Do not re-run this expecting a find.

(Tag count: `git ls-remote --tags upstream` lists 48 ref lines, but 24 of those
are the `^{}` peeled companions of annotated tags, so there are 24 distinct
tags. iki.fi lists the same 48 refs.)

**The "integer overflow" commit in PR #5.** Dropped. Its entire change is

```cpp
-elements.reserve(DitherMatrixWidth * DitherMatrixHeight);
+// Cast to size_t to prevent overflow
+elements.reserve(static_cast<std::size_t>(DitherMatrixWidth) * DitherMatrixHeight);
```

which is a no-op: `reserve()` already takes `size_type`, and the fill loop
immediately recomputes the same count. The 32-bit multiply that would actually
matter is `unsigned matrix_size = Height * Width;` at `dither.cc:72`, and the
commit never touched it — `dither.cc` is byte-identical to upstream PR #2. The
commit subject is misleading rather than merely wrong: a cast *is* present, it
just cannot do what the comment claims. (Earlier notes put this at
`dither.cc:93` as `DitherMatrixWidth * DitherMatrixHeight`; line 93 is actually
`CreateDispersedDitheringMatrix()`'s return statement. The globals are read at
`dither.cc:95`.)

**A test suite that cannot fail.** Two mutations survived and were investigated
rather than papered over. Replacing `GetMostUsed()` with `GetLeastUsed()` is an
**equivalent mutant** on that fixture — it still returns the background for
every pixel — so the suite is not at fault. But an earlier version of the actor
test *did* have a real gap: it only asserted the block was absent, which a
degenerate flat output passes trivially. It was strengthened to also assert the
background survives everywhere.

### The three automated PRs, and where PR #5 came from

All three were authored by `app/copilot-swe-agent`, all `[WIP]` drafts, all
advertising work that was not in them.

| PR | Title | Advertised | Reality |
|----|-------|-----------|---------|
| #5 | Fix and merge issues from forked repo | +1505 / −5 | ~80 lines of code byte-identical to `bisqwit/animmerger#2`, plus 36.6 MB of build artifacts |
| #7 | Create test cases and update C code with documentation | +1430 / −0 | **zero test cases**; 1386 lines of generated `doc/README.html`, 44 lines of `.depend`, two "Initial plan" commits |
| #9 | Create workflow to build code with dependencies | +0 / −0 | **completely empty** — `changedFiles: 0`, one "Initial plan" commit |

PR #5's provenance was established by blob hash rather than by reading the
diff, compared against the head of `bisqwit/animmerger#2` ("Add feature for the
user to supply their own dithering matrices as images", by Kagamiin, still open):

```
canvas.cc   IDENTICAL  682c8ba6 = 682c8ba6
canvas.hh   IDENTICAL  695f21d7 = 695f21d7
dither.cc   IDENTICAL  6173b8b3 = 6173b8b3
dither.hh   IDENTICAL  05d6926a = 05d6926a
main.cc     DIFFERS    fork=2a926e45  upstream=7e334d47
Makefile    DIFFERS    fork=35ac54dc  upstream=9c81b9b2
pixel.cc    DIFFERS    fork=7c11ad46  upstream=fb6ad5ce
```

The three deltas are all improvements over upstream PR #2 and belong as a review
on that PR, not in this fork:

1. **A real TOCTOU fix** — upstream does `access(arg, R_OK)` then `fopen`
   (check-then-use); the fork opens once and uses the handle. This was the one
   commit subject whose diff actually matched it.
2. **`fclose()` added** on both paths; upstream never closes the `FILE*`.
   Immaterial for a one-shot CLI (the OS closes at exit) but correct.
3. **`pixel.cc` fixed in place** instead of upstream's blanket `-Wno-narrowing`.

### What the test suite and CI are

`tests/test_animmerger.cc` (787 lines), built and run by `make check`. It links
**libgd only** — already a hard dependency — using it to synthesise fixtures and
inspect output, and it shells out to the real `./animmerger`, so assertions cover
the command-line surface rather than internal functions. Scratch files go to
`tests/out/`. **29 tests**, green on g++ 16.2.1 and clang++ 22.1.8.

Coverage: `mostused`, `average` (incl. `--yuv`), `actionavg`, all six
colour-compare methods plus rejection of an unknown one, all four quantisers
against `-Q<n>,4`, dithering, masking, frame accounting, output naming, bad
input, and `--help`.

`.github/workflows/build.yml` is a gcc/clang matrix with `fail-fast: false` (a
GCC-only break and a Clang-only break are different bugs). It installs `make` +
`libgd-dev`, runs `make`, runs `make check`, then runs `./animmerger --version`
and `--longhelp` because a build that links but cannot start is not a build. It
uploads the binary as an artifact and the logs on failure.

**No warnings-as-errors gate, on purpose.** The tree emits ~2,000 warnings, the
overwhelming majority `regparm attribute ignored` from the `FasterPixelMethod`
typedefs — a 32-bit x86 attribute that x86-64 silently drops. Adding that many
suppressions to get a clean log would bury the errors that matter. `make`
already exits non-zero on a hard error, which is the signal worth keeping.

> The exact counts in the workflow comment (~2,000 total, 1,670 `regparm`) were
> measured on the CI toolchain. Re-measured on g++ 16.2.1 locally: **1,987**
> warnings, of which **1,319** `regparm attribute ignored` and **468**
> `always_inline function might not be inlinable`. Different compiler, different
> counts — the point stands, the numbers are a snapshot.

### The CI failure worth remembering

The first run of the new workflow **failed without creating a single job** —
`gh api …/actions/runs/36757236393/jobs` returns `total_count: 0`. The
run-level message is only "This run likely failed because of a workflow file
issue", naming neither line nor cause. The cause was `name: ${{ matrix.compiler }}`,
which interpolates an entire matrix *entry* — an object. GitHub rejects it with
"object, array, and null values should not be evaluated in template". Fixed to
`${{ matrix.compiler.name }}` in `78d7bcf`.

**actionlint is what turns that into a usable error, and it is not wired into
this repo.** Installing actionlint 1.7.7 and re-linting the bad form reproduces
GitHub's exact wording and exits 1:

```
$ /tmp/opencode/actionlint /tmp/opencode/probe/build-matrixobject.yml
build-matrixobject.yml:17:11: object, array, and null values should not be
  evaluated in template with ${{ }} but evaluating the value of type
  {cc: string; name: string} [expression]
EXIT=1

$ /tmp/opencode/actionlint .github/workflows/build.yml .github/workflows/publish-docs.yml
EXIT=0
```

Without actionlint in CI, the next workflow edit of this class will again fail
with only the useless run-level message. Adding an actionlint step to the
workflow is the fix; it is the executor's file, not this pass's.

### Open

- **PR #10 is open and unmerged.** Needs review and merge.
- **animmerger exits 0 on error paths** — measured and pinned above, deliberately
  unfixed. Any change to exit codes should be its own decision.
- **The three good fixes from PR #5 belong on `bisqwit/animmerger#2`** as a
  review comment, not as a merge into this fork.
- **`e88e939`** (the iki.fi-only `-Q` fix) is recommended but not taken.
- **`animmerger_nes`** stays inert until `WIP_nesmode` is merged or the target
  is deleted as dead. Either is defensible; leaving it silently unfulfilled is not.
- **The fork publishes nothing**: 0 tags, 0 releases. Nothing breaks today, and
  upstream has no GitHub releases either, so the real download page remains
  `bisqwit.iki.fi/source/animmerger.html`.
- **actionlint is not in CI.** See above.
- **The Zelda 1 question from the start of the session was answered but never
  acted on** — see below.

### Context: Zelda 1 level-map extraction (research only, no code written)

The session opened with the user asking whether animmerger could extract Zelda 1
level maps. The answer is that it is close to ideal, for three specific reasons:
LoZ 1's viewport scrolls in small tile steps (inside animmerger's ≤16px,
≤4px-preferred alignment budget), there is a single background layer so the
parallax caveat that ruins SMW/Metroid titles does not apply, and dungeon rooms
and overworld screens are contiguous in one global map coordinate space.

```bash
animmerger -pm frames/*.png -o overworld.png            # overworld: no HUD to mask
animmerger -pm frames/*.png -m0,224,256,16 -o dungeon.png  # mask the dungeon item row
animmerger --gif -pc frames/*.png                        # animated, fixed background
```

Caveats: capture every frame (Link moves ≤2px/frame); sprites get baked in if
they sit still; screen-state jumps (level-select, death, stair transitions)
throw off the aligner; you only get rooms you actually walked; one wrong offset
shifts the whole map. Note that `ian-albert.com/games/legend_of_zelda_maps/`
already hosts complete Quest-1 maps with layer separations, and per that page
**Quest 2 is not mapped anywhere** — which is the interesting gap animmerger
could actually fill.

This is unverified against real Zelda 1 capture data. Do not treat it as tested.
