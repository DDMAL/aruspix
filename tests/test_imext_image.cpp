// Tier 2 — leaf functions in imext.cpp that operate on imImage buffers.
//
// Each test builds a tiny synthetic image, runs the function, and pins
// down the observed output so the OpenCV port can be checked against
// the same expectations.

#include <cstring>

#include <doctest/doctest.h>

#include <im.h>
#include <im_image.h>

#include "imext.h"

namespace {

// Helper: fill a GRAY/IM_BYTE image's first plane from a row-major buffer.
void fill_gray(imImage *img, const unsigned char *src) {
    std::memcpy(img->data[0], src, img->count);
}

// Helper: build a labeled USHORT image (background=0, regions=1..N).
imImage *make_labeled(int w, int h, const unsigned short *labels) {
    imImage *img = imImageCreate(w, h, IM_GRAY, IM_USHORT);
    REQUIRE(img != nullptr);
    std::memcpy(img->data[0], labels, img->count * sizeof(unsigned short));
    return img;
}

}  // namespace

// ---------------------------------------------------------------------------
// imSetData — paste `selection` into `image` at (pos_x, pos_y) with clipping
// ---------------------------------------------------------------------------

TEST_CASE("imSetData: paste fully inside the destination") {
    imImage *dst = imImageCreate(4, 4, IM_GRAY, IM_BYTE);
    imImage *sel = imImageCreate(2, 2, IM_GRAY, IM_BYTE);
    REQUIRE(dst);
    REQUIRE(sel);

    std::memset(dst->data[0], 0, dst->count);
    unsigned char selbuf[4] = {1, 2, 3, 4};
    fill_gray(sel, selbuf);

    imSetData(dst, sel, /*pos_x=*/1, /*pos_y=*/1);

    const unsigned char expected[16] = {
        0, 0, 0, 0,
        0, 1, 2, 0,
        0, 3, 4, 0,
        0, 0, 0, 0,
    };
    auto *out = static_cast<unsigned char *>(dst->data[0]);
    for (int i = 0; i < 16; ++i) CHECK(out[i] == expected[i]);

    imImageDestroy(dst);
    imImageDestroy(sel);
}

TEST_CASE("imSetData: negative offset clips top-left") {
    imImage *dst = imImageCreate(3, 3, IM_GRAY, IM_BYTE);
    imImage *sel = imImageCreate(2, 2, IM_GRAY, IM_BYTE);
    std::memset(dst->data[0], 0, dst->count);
    unsigned char selbuf[4] = {1, 2, 3, 4};
    fill_gray(sel, selbuf);

    // Paste at (-1,-1): only sel[1][1] (=4) lands at dst[0][0].
    imSetData(dst, sel, -1, -1);

    auto *out = static_cast<unsigned char *>(dst->data[0]);
    CHECK(out[0] == 4);  // dst(0,0)
    CHECK(out[1] == 0);
    CHECK(out[3] == 0);

    imImageDestroy(dst);
    imImageDestroy(sel);
}

TEST_CASE("imSetData: offset entirely outside the destination is a no-op") {
    imImage *dst = imImageCreate(3, 3, IM_GRAY, IM_BYTE);
    imImage *sel = imImageCreate(2, 2, IM_GRAY, IM_BYTE);
    std::memset(dst->data[0], 0, dst->count);
    unsigned char selbuf[4] = {9, 9, 9, 9};
    fill_gray(sel, selbuf);

    imSetData(dst, sel, 99, 99);

    auto *out = static_cast<unsigned char *>(dst->data[0]);
    for (int i = 0; i < 9; ++i) CHECK(out[i] == 0);

    imImageDestroy(dst);
    imImageDestroy(sel);
}

// ---------------------------------------------------------------------------
// imProcessSafeCrop — clip a (pos_x,pos_y,width,height) crop to the image
// ---------------------------------------------------------------------------

TEST_CASE("imProcessSafeCrop: in-bounds crop is unchanged") {
    imImage *img = imImageCreate(10, 10, IM_GRAY, IM_BYTE);
    int x = 2, y = 3, w = 4, h = 5;
    CHECK(imProcessSafeCrop(img, &w, &h, &x, &y) == true);
    CHECK(x == 2);
    CHECK(y == 3);
    CHECK(w == 4);
    CHECK(h == 5);
    imImageDestroy(img);
}

TEST_CASE("imProcessSafeCrop: clips against negative origin and oversize") {
    imImage *img = imImageCreate(10, 10, IM_GRAY, IM_BYTE);
    int x = -2, y = -3, w = 20, h = 30;
    CHECK(imProcessSafeCrop(img, &w, &h, &x, &y) == true);
    CHECK(x == 0);
    CHECK(y == 0);
    CHECK(w == 10);
    CHECK(h == 10);
    imImageDestroy(img);
}

TEST_CASE("imProcessSafeCrop: returns false when origin is outside the image") {
    imImage *img = imImageCreate(10, 10, IM_GRAY, IM_BYTE);
    int x = 20, y = 5, w = 4, h = 4;
    CHECK(imProcessSafeCrop(img, &w, &h, &x, &y) == false);
    imImageDestroy(img);
}

TEST_CASE("imProcessSafeCrop: returns false when clipped size collapses") {
    imImage *img = imImageCreate(10, 10, IM_GRAY, IM_BYTE);
    int x = -5, y = 0, w = 4, h = 4;  // x=-5 + w=4 → w becomes -1 → false
    CHECK(imProcessSafeCrop(img, &w, &h, &x, &y) == false);
    imImageDestroy(img);
}

// ---------------------------------------------------------------------------
// imAnalyzeProjectionH / imAnalyzeProjectionV — row / column sums
// ---------------------------------------------------------------------------

TEST_CASE("imAnalyzeProjectionH: sums each row of an IM_BYTE image") {
    imImage *img = imImageCreate(3, 2, IM_GRAY, IM_BYTE);
    const unsigned char buf[6] = {
        1, 2, 3,   // row 0 sum = 6
        4, 5, 6,   // row 1 sum = 15
    };
    fill_gray(img, buf);

    int hist[2] = {-1, -1};
    imAnalyzeProjectionH(img, hist);
    CHECK(hist[0] == 6);
    CHECK(hist[1] == 15);
    imImageDestroy(img);
}

TEST_CASE("imAnalyzeProjectionV: sums each column of an IM_BYTE image") {
    imImage *img = imImageCreate(3, 2, IM_GRAY, IM_BYTE);
    const unsigned char buf[6] = {
        1, 2, 3,
        4, 5, 6,
    };
    fill_gray(img, buf);

    int hist[3] = {-1, -1, -1};
    imAnalyzeProjectionV(img, hist);
    CHECK(hist[0] == 5);   // 1+4
    CHECK(hist[1] == 7);   // 2+5
    CHECK(hist[2] == 9);   // 3+6
    imImageDestroy(img);
}

// ---------------------------------------------------------------------------
// imAnalyzeBoundingBoxes — bounding boxes of a labeled USHORT image
// boxes layout per region: [xmin, xmax, ymin, ymax]
// ---------------------------------------------------------------------------

TEST_CASE("imAnalyzeBoundingBoxes: two disjoint rectangular regions") {
    // 5x4 image, label 1 at (0,0)-(1,1), label 2 at (3,2)-(4,3).
    const unsigned short labels[20] = {
        1, 1, 0, 0, 0,
        1, 1, 0, 0, 0,
        0, 0, 0, 2, 2,
        0, 0, 0, 2, 2,
    };
    imImage *img = make_labeled(5, 4, labels);

    int boxes[8] = {0};
    imAnalyzeBoundingBoxes(img, boxes, /*region_count=*/2);

    CHECK(boxes[0] == 0); CHECK(boxes[1] == 1);  // region 1 x range
    CHECK(boxes[2] == 0); CHECK(boxes[3] == 1);  // region 1 y range
    CHECK(boxes[4] == 3); CHECK(boxes[5] == 4);  // region 2 x range
    CHECK(boxes[6] == 2); CHECK(boxes[7] == 3);  // region 2 y range

    imImageDestroy(img);
}

// ---------------------------------------------------------------------------
// imAnalyzeClearMin — drop labeled regions whose bbox is too small
// ---------------------------------------------------------------------------

TEST_CASE("imAnalyzeClearMin: removes regions narrower or shorter than threshold") {
    // 6x4: region 1 is 1x1 (too small at threshold=2), region 2 is 3x3.
    const unsigned short labels[24] = {
        1, 0, 0, 0, 0, 0,
        0, 0, 2, 2, 2, 0,
        0, 0, 2, 2, 2, 0,
        0, 0, 2, 2, 2, 0,
    };
    imImage *img = make_labeled(6, 4, labels);

    imAnalyzeClearMin(img, /*region_count=*/2, /*threshold=*/2);

    auto *out = static_cast<unsigned short *>(img->data[0]);
    CHECK(out[0] == 0);          // region 1 cleared
    CHECK(out[6 + 2] == 2);      // region 2 preserved (one sample point)
    CHECK(out[3 * 6 + 4] == 2);  // region 2 still there

    imImageDestroy(img);
}

// ---------------------------------------------------------------------------
// imAnalyzeRuns — find peak / median run length
// ---------------------------------------------------------------------------

TEST_CASE("imAnalyzeRuns: vertical foreground runs of equal length") {
    // QUIRK: imAnalyzeRuns has an off-by-one in its transition handling.
    // When the buffer transitions to the target type, the transition
    // pixel itself doesn't get counted toward the run — run_val is reset
    // to 0 and only subsequent same-type pixels increment it. A K-pixel
    // foreground stripe is therefore reported as a length-(K-1) run.
    // We pin this behavior so the OpenCV port can match it (or
    // explicitly choose to fix it).
    //
    // 3x6 image, height=6: 3-pixel-tall foreground stripe at rows 1..3.
    // Each column → one run of recorded length 2.
    const unsigned char buf[18] = {
        0, 0, 0,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        0, 0, 0,
        0, 0, 0,
    };
    imImage *img = imImageCreate(3, 6, IM_GRAY, IM_BYTE);
    fill_gray(img, buf);

    int peak = -1, med = -1;
    imAnalyzeRuns(img, &peak, &med, /*type=*/1, /*vertical=*/true);
    CHECK(med == 2);
    CHECK(peak == 2);
    imImageDestroy(img);
}

TEST_CASE("imAnalyzeRuns: image with no qualifying runs reports zero") {
    // All zeros → no transitions, so neither type=0 nor type=1 runs
    // get flushed. Algorithm reports (0, 0).
    imImage *img = imImageCreate(3, 3, IM_GRAY, IM_BYTE);
    std::memset(img->data[0], 0, img->count);

    int peak = -1, med = -1;
    imAnalyzeRuns(img, &peak, &med, /*type=*/1, /*vertical=*/true);
    CHECK(peak == 0);
    CHECK(med == 0);
    imImageDestroy(img);
}
