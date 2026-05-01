// Test for the cv::Mat-native binarize_and_clean free function
// extracted from ImPage::BinarizeAndClean.
//
// We don't test exact pixel-level output (the underlying threshold
// algorithms have their own behavioral tests and the wrapper
// composes morphology + connected-component pruning on top). Instead
// we test the *contract* of the pipeline:
//   - input is grayscale, output is strictly 0/1 binary
//   - foreground (the dark ink in the input) becomes 1, background 0
//   - isolated noise pixels below the area threshold get removed
//   - the output preserves the source dimensions

#include <doctest/doctest.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "binarize.h"

namespace {

// Build a synthetic page: bright background, a single solid dark
// rectangle as "ink", plus a handful of isolated dark pixels as
// noise. With small_element pruning enabled and an area threshold
// well above 1, the noise pixels should be removed.
cv::Mat make_synthetic_page(int rows, int cols, cv::Rect ink,
                            const std::vector<cv::Point> &noise,
                            unsigned char bg = 220, unsigned char fg = 40) {
    cv::Mat img(rows, cols, CV_8UC1, cv::Scalar(bg));
    img(ink).setTo(fg);
    for (const auto &p : noise) {
        if (p.x >= 0 && p.x < cols && p.y >= 0 && p.y < rows) {
            img.at<uchar>(p) = fg;
        }
    }
    return img;
}

}  // namespace

TEST_CASE("binarize_and_clean rejects empty / non-grayscale input") {
    ax::BinarizeAndCleanParams params;
    cv::Mat empty;
    cv::Mat dst;
    CHECK_FALSE(ax::binarize_and_clean(empty, dst, params));

    cv::Mat rgb(8, 8, CV_8UC3, cv::Scalar(0, 0, 0));
    CHECK_FALSE(ax::binarize_and_clean(rgb, dst, params));
}

TEST_CASE("binarize_and_clean: output is strictly binary (0 or 1)") {
    cv::Mat src = make_synthetic_page(64, 64, cv::Rect(8, 8, 32, 32), {});
    cv::Mat dst;
    ax::BinarizeAndCleanParams params;
    REQUIRE(ax::binarize_and_clean(src, dst, params));

    REQUIRE(dst.rows == src.rows);
    REQUIRE(dst.cols == src.cols);
    REQUIRE(dst.type() == CV_8UC1);
    for (int y = 0; y < dst.rows; ++y) {
        const uchar *row = dst.ptr<uchar>(y);
        for (int x = 0; x < dst.cols; ++x) {
            CHECK((row[x] == 0 || row[x] == 1));
        }
    }
}

TEST_CASE("binarize_and_clean: ink area becomes foreground (1), background stays 0") {
    cv::Rect ink(8, 8, 32, 32);
    cv::Mat src = make_synthetic_page(64, 64, ink, {});
    cv::Mat dst;
    ax::BinarizeAndCleanParams params;
    REQUIRE(ax::binarize_and_clean(src, dst, params));

    int ink_ones = 0, ink_total = 0;
    int bg_zeros = 0, bg_total = 0;
    for (int y = 0; y < dst.rows; ++y) {
        for (int x = 0; x < dst.cols; ++x) {
            bool inside = ink.contains(cv::Point(x, y));
            if (inside) {
                ink_total++;
                if (dst.at<uchar>(y, x) == 1) ink_ones++;
            } else {
                bg_total++;
                if (dst.at<uchar>(y, x) == 0) bg_zeros++;
            }
        }
    }
    // The ink rectangle should come out almost entirely as 1s; the
    // background should come out almost entirely as 0s. We allow a
    // small slack because morphological opening can erode the ink
    // border slightly.
    CHECK(ink_ones > ink_total * 0.9);
    CHECK(bg_zeros > bg_total * 0.95);
}

TEST_CASE("binarize_and_clean: isolated noise pixels are pruned by area filter") {
    // Big ink block plus a few stray foreground pixels far from it.
    // With space_width=10, line_width=1, ink_factor=10 → area threshold
    // is ~10 * (1/10 + 1/(1-mean))^2 → for a small ink block, plenty
    // big enough to wipe out single-pixel noise.
    cv::Rect ink(20, 20, 24, 24);
    std::vector<cv::Point> noise = {
        {2, 2}, {2, 60}, {60, 2}, {60, 60}, {5, 50},
    };
    cv::Mat src = make_synthetic_page(64, 64, ink, noise);
    cv::Mat dst;
    ax::BinarizeAndCleanParams params;
    params.space_width = 10;
    params.line_width = 1;
    REQUIRE(ax::binarize_and_clean(src, dst, params));

    for (const auto &p : noise) {
        CAPTURE(p.x);
        CAPTURE(p.y);
        CHECK(dst.at<uchar>(p) == 0);
    }
}

TEST_CASE("binarize_and_clean: Sauvola method produces a binary image") {
    cv::Rect ink(8, 8, 32, 32);
    cv::Mat src = make_synthetic_page(64, 64, ink, {});
    cv::Mat dst;
    ax::BinarizeAndCleanParams params;
    params.method = ax::BinarizationMethod::Sauvola;
    params.sauvola_region_size = 9;
    REQUIRE(ax::binarize_and_clean(src, dst, params));

    REQUIRE(dst.type() == CV_8UC1);
    for (int y = 0; y < dst.rows; ++y) {
        const uchar *row = dst.ptr<uchar>(y);
        for (int x = 0; x < dst.cols; ++x) {
            CHECK((row[x] == 0 || row[x] == 1));
        }
    }
}

TEST_CASE("binarize_and_clean: FixedAt127 fallback works") {
    cv::Mat src = make_synthetic_page(32, 32, cv::Rect(4, 4, 24, 24), {});
    cv::Mat dst;
    ax::BinarizeAndCleanParams params;
    params.method = ax::BinarizationMethod::FixedAt127;
    REQUIRE(ax::binarize_and_clean(src, dst, params));

    REQUIRE(dst.type() == CV_8UC1);
    int ones = cv::countNonZero(dst);
    int total = dst.rows * dst.cols;
    CHECK(ones > 0);
    CHECK(ones < total);  // not all-foreground
}
