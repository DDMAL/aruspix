#ifndef AX_BINARIZE_H
#define AX_BINARIZE_H

#include <opencv2/core.hpp>

namespace ax {

enum class BinarizationMethod {
    Brink2Classes,
    Brink3Classes,
    Sauvola,
    FixedAt127,
};

struct BinarizeAndCleanParams {
    BinarizationMethod method = BinarizationMethod::Brink2Classes;
    // Only used when method == Sauvola.
    int sauvola_region_size = 25;
    // Used to compute the small-element pruning threshold. These are
    // pixel measurements of staff-line widths and inter-line spacing,
    // which ImPage::FindStaves fills in before BinarizeAndClean runs.
    // Pass 1/1 if you don't have meaningful values — the threshold
    // formula degrades to a constant.
    int space_width = 1;
    int line_width = 1;
};

// Binarize a grayscale page image and remove small noise components.
//
// Pipeline (mirrors what ImPage::BinarizeAndClean used to do inline
// with IM calls):
//   1. 3x3 median filter on the grayscale source.
//   2. Threshold to a binary image using the chosen method. Inputs
//      are assumed dark-foreground / bright-background (typical
//      scanned page); the threshold methods are called with
//      white_is_255=false so foreground comes out as 1 in dst.
//   3. If the binary image's mean is > 0.5 (most pixels are
//      foreground), invert it. Convention: foreground is the
//      minority class (text/staves on a mostly-white page).
//   4. Conditionally morph-open with a 3x3 kernel when the image
//      is "noisy" by the (1-mean)/stddev < 2.0 heuristic.
//   5. Remove connected components below a computed area threshold
//      using 8-connectivity.
//
// src_gray: 8-bit single-channel.
// dst_binary: 8-bit single-channel, values 0 or 1 only. Reallocated.
// Returns false on invalid input; true otherwise.
bool binarize_and_clean(const cv::Mat &src_gray, cv::Mat &dst_binary,
                        const BinarizeAndCleanParams &params);

}  // namespace ax

#endif  // AX_BINARIZE_H
