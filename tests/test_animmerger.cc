// Regression tests for animmerger.
//
// Links libgd (already a hard dependency of the project) so it can both
// synthesise input fixtures and inspect output, and shells out to the real
// ./animmerger binary so every assertion exercises the actual command-line
// surface rather than an internal API.
//
// Build and run:  make check

#include <gd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <set>

namespace {

int g_pass = 0;
int g_fail = 0;
std::string g_case;

void BeginCase(const char* name)
{
    g_case = name;
}

void Fail(const std::string& detail)
{
    ++g_fail;
    std::fprintf(stderr, "FAIL %s: %s\n", g_case.c_str(), detail.c_str());
}

void Pass()
{
    ++g_pass;
    std::printf("ok   %s\n", g_case.c_str());
}

#define CHECK(cond, msg) do { if(!(cond)) { Fail(msg); return; } } while(0)
#define CHECK_MSG(cond, msg) do { if(!(cond)) { Fail(msg); return; } } while(0)

// ---------------------------------------------------------------- fixtures

// All scratch files live under tests/out/ so a test run never litters the
// source tree. animmerger is invoked from the repository root (it has to be,
// the harness shells out to ./animmerger), so every path handed to it has to
// be spelled out relative to that root.
const char* OutDir()
{
    const char* d = "tests/out";
    std::string cmd = std::string("mkdir -p ") + d;
    if(std::system(cmd.c_str()) != 0) std::fprintf(stderr, "cannot create %s\n", d);
    return d;
}

std::string Path(const std::string& name)
{
    return std::string(OutDir()) + "/" + name;
}

// animmerger's %04d convention expands in OUTPUT filenames only -- an input
// path containing %03d is treated as a literal filename and fails to open.
// Multi-frame tests therefore have to spell the input list out. 'prefix' is a
// bare fixture name; Path() adds the scratch directory.
std::string InputList(const std::string& prefix, int count)
{
    std::string list;
    for(int i = 0; i < count; ++i)
    {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%s-%03d.png", prefix.c_str(), i);
        if(i) list += " ";
        list += Path(buf);
    }
    return list;
}

gdImagePtr MakeImage(int w, int h)
{
    gdImagePtr im = gdImageCreateTrueColor(w, h);
    gdImageFilledRectangle(im, 0, 0, w - 1, h - 1, gdTrueColor(0, 0, 0));
    return im;
}

void SavePng(gdImagePtr im, const std::string& name)
{
    FILE* fp = std::fopen(Path(name).c_str(), "wb");
    if(!fp) { Fail("cannot open " + name + " for writing"); return; }
    gdImagePng(im, fp);
    std::fclose(fp);
}

gdImagePtr LoadPng(const std::string& name)
{
    FILE* fp = std::fopen(Path(name).c_str(), "rb");
    if(!fp) return nullptr;
    gdImagePtr im = gdImageCreateFromPng(fp);
    std::fclose(fp);
    return im;
}

// Loads an image from an explicit path rather than from the scratch dir, for
// the one test that asserts where the default output name lands.
gdImagePtr LoadPngAt(const std::string& fullpath)
{
    FILE* fp = std::fopen(fullpath.c_str(), "rb");
    if(!fp) return nullptr;
    gdImagePtr im = gdImageCreateFromPng(fp);
    std::fclose(fp);
    return im;
}

gdImagePtr LoadGifAt(const std::string& fullpath)
{
    FILE* fp = std::fopen(fullpath.c_str(), "rb");
    if(!fp) return nullptr;
    gdImagePtr im = gdImageCreateFromGif(fp);
    std::fclose(fp);
    return im;
}

// gdImageDestroy dereferences its argument without a null check, and LoadPng
// returns null on a missing or unreadable file, which several tests assert.
void Destroy(gdImagePtr im)
{
    if(im) gdImageDestroy(im);
}

// Run animmerger. Returns the exit status, and captures merged stdout+stderr
// into g_lastOutput so a failing test can print it.
//
// The argument string is handed to /bin/sh, not to execve. That matters:
// animmerger's output-name template contains printf positional specifiers, so
// '%3$s' reaches the shell as a parameter expansion of the non-existent
// positional parameter 's' and collapses to '%3'. An output template must
// therefore be single-quoted at every call site -- see
// TestAnimatedOutputFormatSelection. Silently losing '$' is the kind of bug an
// assertion of the form "the file is absent" cannot detect, because a missing
// file also passes that.
std::string g_lastOutput;
int Run(const std::string& args)
{
    std::string logfile = Path("cmd.log");
    std::string cmd = "./animmerger " + args + " > " + logfile + " 2>&1";
    int status = std::system(cmd.c_str());
    g_lastOutput.clear();
    FILE* fp = std::fopen(logfile.c_str(), "rb");
    if(fp)
    {
        char buf[512];
        size_t n;
        while((n = std::fread(buf, 1, sizeof(buf), fp)) > 0) g_lastOutput.append(buf, n);
        std::fclose(fp);
    }
    return status;
}

int CountDistinctColors(gdImagePtr im)
{
    std::set<unsigned> seen;
    for(int y = 0; y < gdImageSY(im); ++y)
        for(int x = 0; x < gdImageSX(im); ++x)
            seen.insert(gdImageGetTrueColorPixel(im, x, y));
    return (int)seen.size();
}

bool SameImage(gdImagePtr a, gdImagePtr b)
{
    if(!a || !b) return false;
    if(gdImageSX(a) != gdImageSX(b) || gdImageSY(a) != gdImageSY(b)) return false;
    for(int y = 0; y < gdImageSY(a); ++y)
        for(int x = 0; x < gdImageSX(a); ++x)
            if(gdImageGetTrueColorPixel(a, x, y) != gdImageGetTrueColorPixel(b, x, y)) return false;
    return true;
}

// ---------------------------------------------------------------- fixtures

// 64x64 grey gradient. Deterministic, smooth, and dithering-visible.
void BuildGradient(const std::string& name)
{
    gdImagePtr im = MakeImage(64, 64);
    for(int y = 0; y < 64; ++y)
        for(int x = 0; x < 64; ++x)
            gdImageSetPixel(im, x, y, gdTrueColor(x * 4 % 256, y * 4 % 256, (x + y) * 2 % 256));
    SavePng(im, name);
    Destroy(im);
}

// N copies of one frame: a flat mid-grey field. Used to prove that an
// averaging or most-used method reproduces the input exactly when nothing moves.
void BuildFlat(const std::string& name, int copies, unsigned r, unsigned g, unsigned b)
{
    for(int i = 0; i < copies; ++i)
    {
        gdImagePtr im = MakeImage(32, 32);
        gdImageFilledRectangle(im, 0, 0, 31, 31, gdTrueColor(r, g, b));
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%s-%03d.png", name.c_str(), i);
        SavePng(im, buf);
        Destroy(im);
    }
}

// A static background with a bright block that walks across it. The block is
// the "actor" that most-used is supposed to discard.
void BuildMovingBlock(const std::string& name, int frames)
{
    const int BGSIDE = 32;
    gdImagePtr bg = MakeImage(BGSIDE, BGSIDE);
    gdImageFilledRectangle(bg, 0, 0, 31, 31, gdTrueColor(20, 40, 60));

    for(int i = 0; i < frames; ++i)
    {
        gdImagePtr im = gdImageClone(bg);
        // Park the block on each 8x8 cell in turn so every cell is occupied
        // exactly once: the actor is never the most-used pixel anywhere.
        int cx = (i % 4) * 8, cy = (i / 4) * 8;
        gdImageFilledRectangle(im, cx, cy, cx + 7, cy + 7, gdTrueColor(240, 240, 240));
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%s-%03d.png", name.c_str(), i);
        SavePng(im, buf);
        Destroy(im);
    }
    Destroy(bg);
}

// ---------------------------------------------------------------- tests

// MOSTUSED over identical frames must reproduce the input exactly. If it does
// not, the background-extraction path that every level map depends on is wrong.
void TestMostUsedIdenticalFrames()
{
    BeginCase("mostused/identical-frames-reproduce-input");
    BuildFlat("flat", 4, 90, 140, 200);
    CHECK(Run("--noalign -pm " + Path("flat-000.png") + " " + Path("flat-001.png") + " " +
              Path("flat-002.png") + " " + Path("flat-003.png") +
              " -o " + Path("out-mostused.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);

    gdImagePtr in = LoadPng("flat-000.png");
    gdImagePtr out = LoadPng("out-mostused.png");
    CHECK(in && out, "could not load input or output png");
    CHECK(SameImage(in, out), "mostused of 4 identical frames != the input frame");
    Destroy(in);
    Destroy(out);
    Pass();
}

// MOSTUSED must drop an actor that moves, and keep the background it moved over.
void TestMostUsedDiscardsMovingActor()
{
    BeginCase("mostused/discards-moving-actor");
    BuildMovingBlock("actor", 16);
    CHECK(Run("--noalign -pm " + InputList("actor", 16) +
              " -o " + Path("out-nomostused.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);

    gdImagePtr out = LoadPng("out-nomostused.png");
    CHECK(out, "could not load output png");
    // Assert both directions. Checking only that the actor is absent is not
    // enough: a background extractor that collapses to a single flat colour
    // also has no actor in it, and that is just as broken.
    const int BACKGROUND = gdTrueColor(20, 40, 60);
    int wrongPixel = 0, firstX = -1, firstY = -1;
    for(int y = 0; y < gdImageSY(out); ++y)
        for(int x = 0; x < gdImageSX(out); ++x)
        {
            int px = gdImageGetTrueColorPixel(out, x, y);
            if(px != BACKGROUND && px != gdTrueColor(240, 240, 240))
            {
                if(firstX < 0) { firstX = x; firstY = y; }
                ++wrongPixel;
            }
        }
    CHECK_MSG(wrongPixel == 0,
              "mostused invented a colour that was in no input frame");
    // Every one of the 16 possible block positions was occupied for exactly
    // one frame, so the white block can never win a most-used vote.
    for(int cy = 0; cy < 32; cy += 8)
        for(int cx = 0; cx < 32; cx += 8)
        {
            unsigned px = gdImageGetTrueColorPixel(out, cx, cy);
            char msg[128];
            std::snprintf(msg, sizeof(msg), "actor leaked into background at (%d,%d): got rgb(%u,%u,%u)",
                          cx, cy, gdTrueColorGetRed(px), gdTrueColorGetGreen(px), gdTrueColorGetBlue(px));
            CHECK_MSG(px != gdTrueColor(240, 240, 240), msg);
        }
    // And the background itself must have survived everywhere.
    int survivors = 0;
    for(int y = 0; y < gdImageSY(out); ++y)
        for(int x = 0; x < gdImageSX(out); ++x)
            if(gdImageGetTrueColorPixel(out, x, y) == BACKGROUND) ++survivors;
    CHECK_MSG(survivors == gdImageSX(out) * gdImageSY(out),
              "mostused did not return the background everywhere: only "
              "the actor colour survived, so the extraction collapsed");
    Destroy(out);
    Pass();
}

// AVERAGE over two frames must blend them. Black+white lands near mid-grey;
// a copy-the-first-frame implementation would give pure black and pass a
// naive "not all black" check, so assert the actual magnitude.
void TestAverageBlends()
{
    BeginCase("average/blends-two-frames");
    BuildFlat("black", 1, 0, 0, 0);
    BuildFlat("white", 1, 255, 255, 255);
    CHECK(Run("--noalign -pa " + Path("black-000.png") + " " + Path("white-000.png") +
              " -o " + Path("out-average.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);

    gdImagePtr out = LoadPng("out-average.png");
    CHECK(out, "could not load output png");
    unsigned px = gdImageGetTrueColorPixel(out, 16, 16);
    int lum = (int)(gdTrueColorGetRed(px) * 299 + gdTrueColorGetGreen(px) * 587 + gdTrueColorGetBlue(px) * 114) / 1000;
    char msg[128];
    std::snprintf(msg, sizeof(msg), "black+white averaged to luma %d, expected near 127", lum);
    CHECK_MSG(lum >= 120 && lum <= 135, msg);
    Destroy(out);
    Pass();
}

// --yuv must not change a grey blend: black+white is the same grey in either
// colourspace. Guards the YUV code path against a sign or channel-swap error.
void TestAverageYuvAgreesOnGrey()
{
    BeginCase("average/yuv-agrees-on-grey");
    CHECK(Run("--noalign --yuv -pa " + Path("black-000.png") + " " + Path("white-000.png") +
              " -o " + Path("out-average-yuv.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);

    gdImagePtr rgb = LoadPng("out-average.png");
    gdImagePtr yuv = LoadPng("out-average-yuv.png");
    CHECK(rgb && yuv, "could not load one of the outputs");
    // Averaging black and white is the same neutral grey in either colourspace,
    // up to rounding: the two paths round 127.5 differently (RGB gives 128, YUV
    // gives 127). Anything beyond that would be a real channel or sign error.
    int maxDelta = 0;
    for(int y = 0; y < gdImageSY(rgb); ++y)
        for(int x = 0; x < gdImageSX(rgb); ++x)
        {
            int r0 = gdTrueColorGetRed(gdImageGetTrueColorPixel(rgb, x, y));
            int g0 = gdTrueColorGetGreen(gdImageGetTrueColorPixel(rgb, x, y));
            int b0 = gdTrueColorGetBlue(gdImageGetTrueColorPixel(rgb, x, y));
            int r1 = gdTrueColorGetRed(gdImageGetTrueColorPixel(yuv, x, y));
            int g1 = gdTrueColorGetGreen(gdImageGetTrueColorPixel(yuv, x, y));
            int b1 = gdTrueColorGetBlue(gdImageGetTrueColorPixel(yuv, x, y));
            int d = std::max(std::max(std::abs(r0 - r1), std::abs(g0 - g1)), std::abs(b0 - b1));
            if(d > maxDelta) maxDelta = d;
        }
    char msg[128];
    std::snprintf(msg, sizeof(msg), "--yuv differs from RGB by %d per channel on a neutral grey; expected at most rounding", maxDelta);
    CHECK_MSG(maxDelta <= 1, msg);
    Destroy(rgb);
    Destroy(yuv);
    Pass();
}

// AVERAGE must keep a static background at full strength while blurring only
// the actor. A plain mean of a background that is present in every frame would
// wash the background out; ACTIONAVG is the method that must not.
void TestActionAvgKeepsBackgroundSolid()
{
    BeginCase("actionavg/keeps-background-solid");
    BuildMovingBlock("actionbg", 16);
    CHECK(Run("--noalign -pt " + InputList("actionbg", 16) +
              " -o " + Path("out-actionavg.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);

    gdImagePtr out = LoadPng("out-actionavg.png");
    CHECK(out, "could not load output png");
    // Pick cells the actor never occupied... the actor covers all 16 cells, so
    // instead assert the whole field is a uniform non-black colour, i.e. the
    // background survived instead of being averaged into the actor.
    int first = gdImageGetTrueColorPixel(out, 0, 0);
    bool uniform = true;
    for(int y = 0; y < gdImageSY(out) && uniform; ++y)
        for(int x = 0; x < gdImageSX(out); ++x)
            if(gdImageGetTrueColorPixel(out, x, y) != first) { uniform = false; break; }
    CHECK_MSG(uniform, "actionavg left a non-uniform field: background was not held solid");
    CHECK_MSG(first != gdTrueColor(0, 0, 0), "actionavg produced a fully black field");
    Destroy(out);
    Pass();
}

// Every colour-compare method must be accepted and produce an image.
void TestColorCompareMethods()
{
    static const char* methods[] = { "rgb", "cie76", "cie94", "ciede2000", "cmc", "bfd" };
    for(const char* m : methods)
    {
        BeginCase((std::string("colordiff/") + m + "-accepted").c_str());
        BuildGradient("cm-" + std::string(m) + "-in.png");
        CHECK(Run(std::string("--noalign --deltae=") + m + " -Qd,16 " + Path(std::string("cm-") + m + "-in.png") +
                  " -o " + Path(std::string("cm-") + m + ".png")) == 0,
              std::string("animmerger rejected --deltae=") + m + ": " + g_lastOutput);
        gdImagePtr out = LoadPng(std::string("cm-") + m + ".png");
        CHECK_MSG(out != nullptr, std::string("no output produced for --deltae=") + m);
        Destroy(out);
        Pass();
    }
}

// Palette reduction must honour the requested colour count.
void TestQuantizeRespectsColorCount()
{
    static const char* qmethods[] = { "m", "d", "b", "q" };
    for(const char* q : qmethods)
    {
        BeginCase((std::string("quantize/") + q + "-respects-color-count").c_str());
        BuildGradient("q-" + std::string(q) + "-in.png");
        CHECK(Run(std::string("--noalign -Q") + q + ",4 " + Path(std::string("q-") + q + "-in.png") +
                  " -o " + Path(std::string("q-") + q + ".png")) == 0,
              std::string("animmerger failed for -Q") + q + ",4: " + g_lastOutput);
        gdImagePtr out = LoadPng(std::string("q-") + q + ".png");
        CHECK(out, "could not load quantized output");
        char msg[128];
        std::snprintf(msg, sizeof(msg), "-Q%s,4 produced %d distinct colors", q, CountDistinctColors(out));
        CHECK_MSG(CountDistinctColors(out) <= 4, msg);
        Destroy(out);
        Pass();
    }
}

// A non-power-of-two dither matrix is allowed but must warn; a power of two
// must not. This is the documented contract of --dithmatrix.
void TestDitherMatrixPowerOfTwoWarning()
{
    BeginCase("dithermatrix/warns-only-for-non-power-of-two");

    CHECK(Run("--noalign --dm 3x3 " + Path("flat-000.png") + " -o " + Path("out-dm3.png")) == 0,
          "animmerger exited non-zero on --dm 3x3: " + g_lastOutput);
    CHECK_MSG(g_lastOutput.find("powers of two") != std::string::npos,
              "expected a power-of-two warning for 3x3, got: " + g_lastOutput);

    CHECK(Run("--noalign --dm 4x4 " + Path("flat-000.png") + " -o " + Path("out-dm4.png")) == 0,
          "animmerger exited non-zero on --dm 4x4: " + g_lastOutput);
    CHECK_MSG(g_lastOutput.find("powers of two") == std::string::npos,
              "unexpected power-of-two warning for 4x4: " + g_lastOutput);

    Pass();
}

// The dither matrix must actually reach the renderer: two different matrix
// sizes must dither a gradient into two different images. A run where the
// --dm value is parsed but ignored would pass a smoke test and fail this.
void TestDitherMatrixChangesOutput()
{
    BeginCase("dithermatrix/changes-rendered-output");
    BuildGradient("dm-in.png");
    CHECK(Run("--noalign -Qd,4 --dm 4x4 " + Path("dm-in.png") + " -o " + Path("out-dm-a.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);
    CHECK(Run("--noalign -Qd,4 --dm 8x8 " + Path("dm-in.png") + " -o " + Path("out-dm-b.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);

    gdImagePtr a = LoadPng("out-dm-a.png");
    gdImagePtr b = LoadPng("out-dm-b.png");
    CHECK(a && b, "could not load one of the outputs");
    CHECK_MSG(!SameImage(a, b), "--dm 4x4 and --dm 8x8 produced identical images");
    Destroy(a);
    Destroy(b);
    Pass();
}

// CENSOR masking (selected with -u censor) fills the rectangle with opaque
// black. Note that the trailing colour list after the geometry is a *filter*
// on which colours count as maskable, not a fill colour -- with a list, only
// pixels matching one of those colours are removed and the rest of the
// rectangle keeps its original content. Passing no list is what blanks the
// whole rectangle.
void TestMaskCensorBlanksRegion()
{
    BeginCase("mask/censor-blanks-region");
    BuildGradient("mask-in.png");
    CHECK(Run("--noalign -pm " + Path("mask-in.png") +
              " -u censor -m0,0,16,16 -o " + Path("out-mask.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);

    gdImagePtr out = LoadPng("out-mask.png");
    CHECK(out, "could not load masked output");
    const int OPAQUE_BLACK = gdTrueColorAlpha(0, 0, 0, 0);
    bool blank = true;
    char detail[192] = "";
    for(int y = 0; y < 16 && blank; ++y)
        for(int x = 0; x < 16; ++x)
        {
            int px = gdImageGetTrueColorPixel(out, x, y);
            if(px != OPAQUE_BLACK)
            {
                std::snprintf(detail, sizeof(detail),
                              "censor mask left rgb(%d,%d,%d) alpha=%d at (%d,%d)",
                              gdTrueColorGetRed(px), gdTrueColorGetGreen(px), gdTrueColorGetBlue(px),
                              gdTrueColorGetAlpha(px), x, y);
                blank = false;
                break;
            }
        }
    CHECK_MSG(blank, std::string("censor mask did not blank the rectangle: ") + detail);

    // Nothing outside the mask may move.
    bool outsideIntact = true;
    for(int y = 16; y < gdImageSY(out) && outsideIntact; ++y)
        for(int x = 0; x < gdImageSX(out); ++x)
        {
            int want = gdTrueColor(x * 4 % 256, y * 4 % 256, (x + y) * 2 % 256);
            if(gdImageGetTrueColorPixel(out, x, y) != want) { outsideIntact = false; break; }
        }
    CHECK_MSG(outsideIntact, "censor masking changed pixels below the masked rectangle");
    Destroy(out);
    Pass();
}

// HOLE masking (the default) must mark the rectangle transparent instead,
// which is a different result from censor -- if both modes produced the same
// bytes then -u would be doing nothing.
void TestMaskHoleDiffersFromCensor()
{
    BeginCase("mask/hole-differs-from-censor");
    BuildGradient("hole2-in.png");
    CHECK(Run("--noalign -pm " + Path("hole2-in.png") +
              " -u censor -m0,0,16,16 -o " + Path("out-censor.png")) == 0,
          "animmerger exited non-zero on the censor control: " + g_lastOutput);
    CHECK(Run("--noalign -pm " + Path("hole2-in.png") +
              " -u hole -m0,0,16,16 -o " + Path("out-holemode.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);

    gdImagePtr censor = LoadPng("out-censor.png");
    gdImagePtr hole = LoadPng("out-holemode.png");
    CHECK(censor && hole, "could not load one of the outputs");
    const int OPAQUE_BLACK = gdTrueColorAlpha(0, 0, 0, 0);
    int holeAlpha = gdTrueColorGetAlpha(gdImageGetTrueColorPixel(hole, 0, 0));
    char msg[192];
    std::snprintf(msg, sizeof(msg),
                  "-u hole and -u censor produced the same masked pixel (alpha=%d, rgb=%d,%d,%d)",
                  holeAlpha,
                  gdTrueColorGetRed(gdImageGetTrueColorPixel(hole, 0, 0)),
                  gdTrueColorGetGreen(gdImageGetTrueColorPixel(hole, 0, 0)),
                  gdTrueColorGetBlue(gdImageGetTrueColorPixel(hole, 0, 0)));
    CHECK_MSG(gdImageGetTrueColorPixel(hole, 0, 0) != OPAQUE_BLACK, msg);
    std::snprintf(msg, sizeof(msg),
                  "-u hole left the rectangle fully opaque (alpha=%d); a hole must carry transparency", holeAlpha);
    CHECK_MSG(holeAlpha != 0, msg);
    Destroy(censor);
    Destroy(hole);
    Pass();
}

// A mask must not disturb anything outside its rectangle. Compare against an
// unmasked control run of the same command.
void TestMaskLeavesSurroundingsIntact()
{
    BeginCase("mask/leaves-surroundings-intact");
    BuildGradient("hole-in.png");
    CHECK(Run("--noalign -pm " + Path("hole-in.png") +
              " -m8,8,8,8 -o " + Path("out-hole.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);
    CHECK(Run("--noalign -pm " + Path("hole-in.png") +
              " -o " + Path("out-nohole.png")) == 0,
          "animmerger exited non-zero on the unmasked control: " + g_lastOutput);

    gdImagePtr masked = LoadPng("out-hole.png");
    gdImagePtr control = LoadPng("out-nohole.png");
    CHECK(masked && control, "could not load one of the outputs");
    bool ok = true;
    for(int y = 0; y < gdImageSY(control) && ok; ++y)
        for(int x = 0; x < gdImageSX(control); ++x)
        {
            if(x >= 8 && x < 16 && y >= 8 && y < 16) continue; // the masked rectangle
            if(gdImageGetTrueColorPixel(masked, x, y) != gdImageGetTrueColorPixel(control, x, y)) { ok = false; break; }
        }
    CHECK_MSG(ok, "masking changed pixels outside the masked rectangle");
    Destroy(masked);
    Destroy(control);
    Pass();
}

// CHANGELOG must emit exactly one output frame per input frame. This is the
// frame-accounting that makes the fixed-background GIF pipeline work.
void TestChangeLogFrameCount()
{
    BeginCase("changelog/one-output-frame-per-input");
    BuildMovingBlock("cl", 7);
    std::string args = "--noalign --gif=never -pc " + InputList("cl", 7) + " -o " + Path("cl-out-%04d.png");
    CHECK(Run(args) == 0, "animmerger exited non-zero: " + g_lastOutput);

    int frames = 0;
    for(int i = 0; ; ++i)
    {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "cl-out-%04d.png", i);
        gdImagePtr im = LoadPng(buf);
        if(!im) break;
        Destroy(im);
        ++frames;
    }
    char msg[128];
    std::snprintf(msg, sizeof(msg), "expected 7 output frames from 7 inputs, found %d", frames);
    CHECK_MSG(frames == 7, msg);
    Pass();
}

// LOOPINGLOG must reuse frames, so N inputs must yield fewer than N outputs.
void TestLoopingLogReusesFrames()
{
    BeginCase("loopinglog/reuses-frames");
    BuildMovingBlock("ll", 24);
    CHECK(Run("--noalign --gif=never -l4 -po " + InputList("ll", 24) + " -o " + Path("ll-out-%04d.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);

    int frames = 0;
    for(int i = 0; ; ++i)
    {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "ll-out-%04d.png", i);
        gdImagePtr im = LoadPng(buf);
        if(!im) break;
        Destroy(im);
        ++frames;
    }
    char msg[128];
    std::snprintf(msg, sizeof(msg), "24 inputs produced %d frames; loopinglog reused nothing", frames);
    CHECK_MSG(frames > 0 && frames < 24, msg);
    Pass();
}

// The output-name template must be honoured verbatim, since the GIF pipeline
// depends on a predictable numbered pattern.
void TestOutputTemplate()
{
    BeginCase("output/template-is-honoured");
    BuildFlat("tpl", 2, 10, 20, 30);
    CHECK(Run("--noalign -pm " + Path("tpl-000.png") + " " + Path("tpl-001.png") +
              " -o " + Path("custom-name-%04d.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);
    CHECK_MSG(LoadPng("custom-name-0000.png") != nullptr,
              "output template custom-name-%04d.png was not honoured");

    // Clean up so the glob below is unambiguous.
    gdImagePtr im = LoadPng("custom-name-0000.png");
    Destroy(im);
    std::remove(Path("custom-name-0000.png").c_str());
    Pass();
}

// An animated method defaults to GIF output. Both escapes have to keep
// working, and neither is tested with a custom name template: see
// TestOutputTemplateShapes below for why that matters.
void TestAnimatedOutputFormatSelection()
{
    BeginCase("output/animated-defaults-to-gif");

    // With -o omitted, an animated run writes the documented default name with
    // a .gif extension -- the whole reason the GIF pipeline globs tile-*.gif.
    BuildMovingBlock("fmt", 3);
    CHECK(Run("--noalign -pc " + InputList("fmt", 3)) == 0,
          "animmerger exited non-zero: " + g_lastOutput);
    gdImagePtr gif = LoadGifAt("tile-0000.gif");
    CHECK_MSG(gif != nullptr,
              "an animated run with -o omitted did not write a loadable tile-0000.gif");
    Destroy(gif);
    for(int i = 0; i < 3; ++i)
    {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "tile-%04d.gif", i);
        std::remove(buf);
    }

    // The other escape: force PNG so the output is actually a PNG.
    BuildMovingBlock("fnp", 3);
    CHECK(Run("--noalign --gif=never -pc " + InputList("fnp", 3) +
              " -o " + Path("fnp-%04d.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);
    gdImagePtr png = LoadPng("fnp-0000.png");
    CHECK_MSG(png != nullptr,
              "--gif=never did not produce a loadable PNG for an animated run");
    Destroy(png);
    Pass();
}

// animmerger hands the -o value straight to snprintf as a format string.
// Whether glibc's fortified printf rejects a template therefore depends on how
// animmerger was compiled, not on the code: _FORTIFY_SOURCE is on by default
// on Debian and Ubuntu and off by default on Arch. So assert the invariant
// that holds either way, and report the fortify-dependent part without failing.
void TestOutputTemplateShapes()
{
    BeginCase("output/default-shaped-template-always-works");

    // animmerger's own default shape is what a user gets by default, so it must
    // work under any build configuration.
    BuildFlat("tplA", 1, 12, 34, 56);
    CHECK_MSG(Run("--noalign -pm " + Path("tplA-000.png") +
                  " -o '" + Path("tplA-%2$s-%1$04u.%3$s") + "'") == 0,
              "animmerger rejected a template shaped like its own default: " + g_lastOutput);
    Destroy(LoadPngAt(Path("tplA-tile-0000.png")));
    std::remove(Path("tplA-tile-0000.png").c_str());

    // Informational: a template whose first conversion is $1 aborts under
    // _FORTIFY_SOURCE with '*** invalid %N$ use detected ***' and no
    // diagnostic, but works on a build without fortify. animmerger should
    // validate the template and report it; today it core dumps. Tracked
    // upstream rather than asserted here, because the correct expectation
    // differs per build configuration.
    BuildFlat("tplB", 1, 12, 34, 56);
    int status = Run("--noalign -pm " + Path("tplB-000.png") +
                     " -o '" + Path("tplB-%04d.%3$s") + "'");
    std::printf("     note: template starting at $1 %s in this build (%s fortify)\n",
                status == 0 ? "was accepted" : "aborted animmerger",
                status == 0 ? "no" : "yes");
    Pass();
}

// --deltae must reject a method name it does not know rather than silently
// falling back to RGB, because a typo in a dithering recipe is otherwise
// invisible: the image still renders, just with the wrong colour metric.
// Like the other error paths in 1.6.2 it reports the problem and still exits 0.
void TestUnknownColorCompareMethodIsRejected()
{
    BeginCase("colordiff/unknown-method-is-rejected");
    BuildGradient("bogus-in.png");
    int status = Run("--noalign --deltae=nosuchmetric -Qd,16 " + Path("bogus-in.png") +
                     " -o " + Path("bogus-out.png"));
    CHECK_MSG(g_lastOutput.find("Unknown identifier") != std::string::npos
              || g_lastOutput.find("color difference formula") != std::string::npos,
              "an unknown --deltae method produced no error text: " + g_lastOutput);
    CHECK_MSG(status == 0, "exit status changed; animmerger 1.6.2 exits 0 after reporting this");
    Pass();
}

// The default output name is what a user gets when they omit -o. It is
// tile-NNNN.png, relative to the current directory, and it is the exact glob
// the documented GIF pipeline uses:
//   gifsicle -O2 -o out.gif -l0 -d3 tile-*.gif
// so if the default name changes, that documented command breaks too.
void TestDefaultOutputTemplate()
{
    BeginCase("output/default-name-is-tile-nnnn");
    BuildFlat("dt", 1, 70, 80, 90);
    CHECK(Run("--noalign --gif=never -pm " + Path("dt-000.png")) == 0,
          "animmerger exited non-zero: " + g_lastOutput);
    // Relative to the repo root, not to the input file's directory.
    gdImagePtr out = LoadPngAt("tile-0000.png");
    CHECK_MSG(out != nullptr,
              "omitting -o did not produce tile-0000.png in the working directory");
    Destroy(out);
    std::remove("tile-0000.png");
    Pass();
}

// Unusable input is reported but does NOT change the exit status: animmerger
// 1.6.2 warns, skips the file, and still exits 0. These tests pin that
// observed behaviour so a future change to it is a deliberate decision rather
// than an accident, and they assert the warning text actually appears -- the
// current state is "silent-ish", not "silent".
void TestUnreadableInputWarnsButExitsZero()
{
    BeginCase("input/unreadable-file-warns");
    FILE* fp = std::fopen(Path("garbage.png").c_str(), "wb");
    CHECK(fp != nullptr, "could not create the garbage fixture");
    const char junk[] = "this is definitely not a PNG file";
    std::fwrite(junk, 1, sizeof(junk) - 1, fp);
    std::fclose(fp);

    int status = Run("--noalign -pm " + Path("garbage.png") + " -o " + Path("out-garbage.png"));
    CHECK_MSG(g_lastOutput.find("unrecognized image type") != std::string::npos,
              "a non-image input produced no 'unrecognized image type' warning: " + g_lastOutput);
    gdImagePtr im = LoadPng("out-garbage.png");
    CHECK_MSG(im == nullptr, "animmerger produced an output png from a non-image input");
    Destroy(im);
    // Known upstream behaviour, asserted so it cannot change unnoticed.
    CHECK_MSG(status == 0, "exit status changed; animmerger 1.6.2 exits 0 here");
    Pass();
}

void TestMissingInputWarnsButExitsZero()
{
    BeginCase("input/missing-file-warns");
    int status = Run("--noalign -pm " + Path("does-not-exist.png") + " -o " + Path("out-missing.png"));
    CHECK_MSG(g_lastOutput.find("No such file or directory") != std::string::npos,
              "a missing input produced no 'No such file or directory' message: " + g_lastOutput);
    gdImagePtr im = LoadPng("out-missing.png");
    CHECK_MSG(im == nullptr, "animmerger produced an output png for a nonexistent input");
    Destroy(im);
    CHECK_MSG(status == 0, "exit status changed; animmerger 1.6.2 exits 0 here");
    Pass();
}

// --help and --longhelp must both succeed, and --longhelp must document the
// dithering options the manual claims exist.
void TestHelpText()
{
    BeginCase("help/exits-zero-and-documents-dithering");
    CHECK(Run("--help") == 0, "--help exited non-zero: " + g_lastOutput);
    CHECK_MSG(g_lastOutput.find("animmerger") != std::string::npos,
              "--help did not print the usage banner");
    CHECK(Run("--longhelp") == 0, "--longhelp exited non-zero: " + g_lastOutput);
    // The dithering options are documented at verbosity 2, so --help alone
    // will not mention them.
    CHECK_MSG(g_lastOutput.find("dithmatrix") != std::string::npos,
              "--longhelp does not mention --dithmatrix");
    CHECK_MSG(g_lastOutput.find("gamma") != std::string::npos,
              "--longhelp does not mention --gamma");
    Pass();
}

} // namespace

int main()
{
    std::setvbuf(stdout, nullptr, _IOLBF, 0);
    std::printf("animmerger regression tests\n");

    TestMostUsedIdenticalFrames();
    TestMostUsedDiscardsMovingActor();
    TestAverageBlends();
    TestAverageYuvAgreesOnGrey();
    TestActionAvgKeepsBackgroundSolid();
    TestColorCompareMethods();
    TestUnknownColorCompareMethodIsRejected();
    TestQuantizeRespectsColorCount();
    TestDitherMatrixPowerOfTwoWarning();
    TestDitherMatrixChangesOutput();
    TestMaskCensorBlanksRegion();
    TestMaskHoleDiffersFromCensor();
    TestMaskLeavesSurroundingsIntact();
    TestChangeLogFrameCount();
    TestLoopingLogReusesFrames();
    TestOutputTemplate();
    TestDefaultOutputTemplate();
    TestAnimatedOutputFormatSelection();
    TestOutputTemplateShapes();
    TestUnreadableInputWarnsButExitsZero();
    TestMissingInputWarnsButExitsZero();
    TestHelpText();

    std::printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail ? 1 : 0;
}
