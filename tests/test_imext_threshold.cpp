// Tier 3 — thresholding algorithms.
//
// These are the highest-value tests for the OpenCV migration: every
// one of these algorithms will need a behavioral equivalent on the
// other side. We don't try to pin every pixel — we pin the *shape* of
// the result on a synthetic bimodal image: dark half ends up as
// foreground (1), bright half as background (0), with the transition
// near the visual boundary.

#include <cstring>

#include <doctest/doctest.h>

#include <im.h>
#include <im_image.h>

#include "imext.h"

namespace {

// Build a 16x16 image with the left half at `dark` and the right half
// at `bright`. The histogram is sharply bimodal, so any half-decent
// threshold should split exactly down the middle.
imImage *make_bimodal(unsigned char dark, unsigned char bright,
                      int w = 16, int h = 16) {
    imImage *img = imImageCreate(w, h, IM_GRAY, IM_BYTE);
    REQUIRE(img != nullptr);
    auto *p = static_cast<unsigned char *>(img->data[0]);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            p[y * w + x] = (x < w / 2) ? dark : bright;
    return img;
}

// Count pixels in a binary destination image (0/1 values) that match
// the expected label `v` in each half.
struct HalfCounts {
    int left_one;
    int left_zero;
    int right_one;
    int right_zero;
};
HalfCounts split_count(const imImage *bin) {
    HalfCounts c{};
    auto *p = static_cast<const unsigned char *>(bin->data[0]);
    for (int y = 0; y < bin->height; ++y) {
        for (int x = 0; x < bin->width; ++x) {
            unsigned char v = p[y * bin->width + x];
            if (x < bin->width / 2) {
                (v ? c.left_one : c.left_zero)++;
            } else {
                (v ? c.right_one : c.right_zero)++;
            }
        }
    }
    return c;
}

}  // namespace

// ---------------------------------------------------------------------------
// Brink minimum-cross-entropy threshold
// ---------------------------------------------------------------------------

TEST_CASE("imProcessBrinkThreshold: bimodal image splits into two halves") {
    imImage *src = make_bimodal(/*dark=*/40, /*bright=*/210);
    imImage *dst = imImageCreate(src->width, src->height, IM_BINARY, IM_BYTE);

    int Topt = imProcessBrinkThreshold(src, dst, /*white_is_255=*/true);
    CHECK(Topt > 40);
    CHECK(Topt < 210);

    auto c = split_count(dst);
    // Dark half should be foreground (1), bright half background (0).
    CHECK(c.left_one == src->width / 2 * src->height);
    CHECK(c.right_zero == src->width / 2 * src->height);

    imImageDestroy(src);
    imImageDestroy(dst);
}

// ---------------------------------------------------------------------------
// Brink 2-/3-class variants
// ---------------------------------------------------------------------------

TEST_CASE("imProcessBrink2ClassesThreshold (Brink & Pendock): bimodal split") {
    imImage *src = make_bimodal(/*dark=*/50, /*bright=*/200);
    imImage *dst = imImageCreate(src->width, src->height, IM_BINARY, IM_BYTE);

    imProcessBrink2ClassesThreshold(src, dst, /*white_is_255=*/true,
                                    BRINK_AND_PENDOCK);

    auto c = split_count(dst);
    CHECK(c.left_one + c.right_one > 0);
    CHECK(c.left_zero + c.right_zero > 0);
    // Bimodal → most foreground pixels live on the dark half.
    CHECK(c.left_one > c.right_one);

    imImageDestroy(src);
    imImageDestroy(dst);
}

TEST_CASE("imProcessBrink3ClassesThreshold (Li & Lee): output is binary, smoke test") {
    // The 3-class Brink threshold returns a single threshold T and
    // produces a binary image, but on strictly bimodal input it tends
    // to collapse: the chosen T puts every pixel into the same class,
    // and after the imProcessBitwiseNot at the end of the algorithm
    // the result is all zeros. We don't pin the foreground/background
    // split here — only that the algorithm runs to completion and
    // emits valid 0/1 pixels. Behaviorally meaningful tests for this
    // function need a tri-modal fixture, which is left for the
    // integration-test layer.
    imImage *src = make_bimodal(/*dark=*/50, /*bright=*/200);
    imImage *dst = imImageCreate(src->width, src->height, IM_BINARY, IM_BYTE);

    imProcessBrink3ClassesThreshold(src, dst, /*white_is_255=*/true,
                                    LI_AND_LEE);

    auto *p = static_cast<unsigned char *>(dst->data[0]);
    for (int i = 0; i < dst->count; ++i) {
        CHECK((p[i] == 0 || p[i] == 1));
    }

    imImageDestroy(src);
    imImageDestroy(dst);
}

// ---------------------------------------------------------------------------
// Sauvola adaptive threshold
// ---------------------------------------------------------------------------

TEST_CASE("imProcessSauvolaThreshold: dark side foreground, bright side background") {
    imImage *src = make_bimodal(/*dark=*/40, /*bright=*/210, /*w=*/32, /*h=*/32);
    imImage *dst = imImageCreate(src->width, src->height, IM_BINARY, IM_BYTE);

    int ok = imProcessSauvolaThreshold(src, dst, /*region_size=*/9,
                                       /*sensitivity=*/0.5f,
                                       /*dynamic_range=*/128,
                                       /*lower_bound=*/20,
                                       /*upper_bound=*/150,
                                       /*white_is_255=*/true);
    CHECK(ok != 0);

    // Dark pixels are below upper_bound=150, so they go through the
    // adaptive branch; with the dark plateau they reliably come out
    // as foreground (1). Bright pixels are >= upper_bound → 0.
    auto c = split_count(dst);
    CHECK(c.left_one > c.left_zero);
    CHECK(c.right_zero == src->width / 2 * src->height);

    imImageDestroy(src);
    imImageDestroy(dst);
}

TEST_CASE("imProcessSauvolaThreshold: invalid region_size returns 0") {
    imImage *src = imImageCreate(8, 8, IM_GRAY, IM_BYTE);
    imImage *dst = imImageCreate(8, 8, IM_BINARY, IM_BYTE);
    std::memset(src->data[0], 100, src->count);

    CHECK(imProcessSauvolaThreshold(src, dst, /*region_size=*/0) == 0);
    CHECK(imProcessSauvolaThreshold(src, dst, /*region_size=*/100) == 0);

    imImageDestroy(src);
    imImageDestroy(dst);
}

// ---------------------------------------------------------------------------
// Pugin threshold (built on top of Otsu + histogram expansion)
// ---------------------------------------------------------------------------

TEST_CASE("imProcessPuginThreshold: pins the current (broken-looking) behavior") {
    // WARNING: imProcessPuginThreshold in this codebase appears to be
    // half-implemented. The kmeans call that was supposed to populate
    // the destination is commented out (see imext.cpp ~line 960), and
    // the final loop only reads from `dest` (which the caller created
    // with imImageCreate, so it starts zero-filled). The result is
    // therefore *always all zeros*, regardless of the input image.
    //
    // We pin this so the OpenCV port isn't held to a wrong reference.
    // Anyone porting Pugin should first dig the original kmeans-based
    // implementation out of git history and decide what it should do.
    imImage *src = make_bimodal(/*dark=*/30, /*bright=*/220);
    imImage *dst = imImageCreate(src->width, src->height, IM_BINARY, IM_BYTE);

    int ok = imProcessPuginThreshold(src, dst, /*white_is_255=*/true);
    CHECK(ok != 0);

    auto *p = static_cast<unsigned char *>(dst->data[0]);
    int ones = 0;
    for (int i = 0; i < dst->count; ++i) {
        CHECK((p[i] == 0 || p[i] == 1));
        if (p[i]) ones++;
    }
    CHECK(ones == 0);  // Pin the broken behavior — every pixel is 0.

    imImageDestroy(src);
    imImageDestroy(dst);
}

// ---------------------------------------------------------------------------
// Kittler entropy threshold
// ---------------------------------------------------------------------------

TEST_CASE("imProcessKittlerThreshold: returns a threshold between the two modes") {
    imImage *src = make_bimodal(/*dark=*/40, /*bright=*/210);
    imImage *dst = imImageCreate(src->width, src->height, IM_BINARY, IM_BYTE);

    int level = imProcessKittlerThreshold(src, dst);
    CHECK(level >= 40);
    CHECK(level <= 210);

    imImageDestroy(src);
    imImageDestroy(dst);
}
