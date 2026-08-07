#ifndef AX_THRESHOLDS_H
#define AX_THRESHOLDS_H

// Algorithm variants shared by the Brink-based thresholds. Historically
// these were defined in imext.h; kept here so callers can use the ax::
// overloads without pulling in the imImage-facing surface.
enum {
    BRINK_AND_PENDOCK = 0,
    LI_AND_LEE
};

#ifdef __cplusplus

#include <opencv2/core.hpp>

namespace ax {

// Adaptive Sauvola threshold. Writes a 0/1 (or 0/255) binary image
// to `dst`. Regions where `src` < lower_bound become foreground (1);
// regions >= upper_bound become background (0); otherwise the pixel
// is compared to a per-pixel adaptive threshold computed from local
// mean and stddev over a `region_size`-sided square window.
//
// `src` must be 8-bit single-channel. `dst` is reshaped/allocated
// as needed to 8-bit single-channel.
// Returns 1 on success, 0 on invalid region_size.
int sauvola_threshold(const cv::Mat& src, cv::Mat& dst, int region_size,
                      float sensitivity = 0.5f, int dynamic_range = 128,
                      int lower_bound = 20, int upper_bound = 150,
                      bool white_is_255 = true);

// Brink cross-entropy threshold. Picks a single threshold value T that
// minimizes the cross-entropy between the foreground and background
// distributions of `src`. `src` 8-bit single-channel; `dst` 8-bit
// single-channel, values 0/1. Returns the chosen T.
int brink_threshold(const cv::Mat& src, cv::Mat& dst,
                    bool white_is_255 = true);

// Brink-Pendock / Li-Lee two-class KL-divergence threshold.
// `src` 8-bit single-channel; `dst` 8-bit single-channel, values 0/1.
// Returns the chosen threshold value T (0..255).
int brink2_classes_threshold(const cv::Mat& src, cv::Mat& dst,
                             bool white_is_255, int algorithm);

// Brink-Pendock / Li-Lee three-class KL-divergence threshold.
// Same shape as brink2_classes_threshold; picks the single threshold
// that partitions into three classes (foreground / midtone / background)
// and returns that threshold value.
int brink3_classes_threshold(const cv::Mat& src, cv::Mat& dst,
                             bool white_is_255, int algorithm);

}  // namespace ax

#endif  // __cplusplus

#endif  // AX_THRESHOLDS_H
