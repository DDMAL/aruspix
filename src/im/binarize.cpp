#include "binarize.h"

#include <algorithm>

#include <opencv2/imgproc.hpp>

#include "thresholds.h"

namespace ax {

namespace {

// Cap on the small-element pruning area, mirroring TP_MAX_SMALL_ELEMENT
// in impage.cpp. Anything below this gets removed; the threshold is
// computed from staff-line geometry but capped.
constexpr int kMaxSmallElement = 100;

// Run the chosen threshold method. Foreground is 1, background is 0.
// Input is assumed dark-foreground / bright-background so all methods
// are invoked with white_is_255=false.
bool run_threshold(const cv::Mat &src8u, cv::Mat &dst_bin,
                   BinarizationMethod method, int sauvola_region) {
    switch (method) {
        case BinarizationMethod::Sauvola:
            return ax::sauvola_threshold(src8u, dst_bin, sauvola_region,
                                         0.5f, 128, 20, 150,
                                         /*white_is_255=*/false) != 0;
        case BinarizationMethod::Brink2Classes:
            ax::brink2_classes_threshold(src8u, dst_bin,
                                         /*white_is_255=*/false,
                                         BRINK_AND_PENDOCK);
            return true;
        case BinarizationMethod::Brink3Classes:
            ax::brink3_classes_threshold(src8u, dst_bin,
                                         /*white_is_255=*/false,
                                         BRINK_AND_PENDOCK);
            return true;
        case BinarizationMethod::FixedAt127:
            cv::threshold(src8u, dst_bin, 127, 1, cv::THRESH_BINARY_INV);
            return true;
    }
    return false;
}

}  // namespace

bool binarize_and_clean(const cv::Mat &src_gray, cv::Mat &dst_binary,
                        const BinarizeAndCleanParams &params) {
    if (src_gray.empty() || src_gray.type() != CV_8UC1) return false;

    // 1. 3x3 median filter on the grayscale input.
    cv::Mat filtered;
    cv::medianBlur(src_gray, filtered, 3);

    // 2. Threshold to binary (values 0/1).
    cv::Mat binary;
    if (!run_threshold(filtered, binary, params.method,
                       params.sauvola_region_size)) {
        return false;
    }

    // 3. If foreground is the majority, invert. The pipeline downstream
    //    assumes foreground is the minority class (ink on white page).
    cv::Scalar mean_s, stddev_s;
    cv::meanStdDev(binary, mean_s, stddev_s);
    double mean = mean_s[0];
    double stddev = stddev_s[0];
    if (mean > 0.5) {
        // Bitwise not on a 0/1 image gives 254/255 on uchar; flip
        // semantically by subtracting from 1.
        cv::subtract(cv::Scalar(1), binary, binary);
        cv::meanStdDev(binary, mean_s, stddev_s);
        mean = mean_s[0];
        stddev = stddev_s[0];
    }

    // 4. Compute area-pruning threshold from staff geometry, mirroring
    //    the formula in the original BinarizeAndClean.
    double ink_factor =
        params.line_width != 0
            ? static_cast<double>(params.space_width) / params.line_width
            : 1.0;
    if (ink_factor == 0.0) ink_factor = 1.0;
    double mean_factor = 1.0 - mean;
    if (mean_factor == 0.0) mean_factor = 1.0;
    double threshold_d =
        10.0 * std::pow(1.0 / ink_factor + 1.0 / mean_factor, 2);
    int area_threshold = std::min(static_cast<int>(threshold_d), kMaxSmallElement);

    // 5. Add a 1-pixel border so connected components touching the
    //    edge don't get clipped during morphology / labeling.
    cv::Mat bordered;
    cv::copyMakeBorder(binary, bordered, 1, 1, 1, 1,
                       cv::BORDER_CONSTANT, cv::Scalar(0));

    // 6. Conditional morphological opening to clean up speckle.
    if (stddev > 0.0 && (1.0 - mean) / stddev < 2.0) {
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,
                                                   cv::Size(3, 3));
        cv::morphologyEx(bordered, bordered, cv::MORPH_OPEN, kernel);
    }

    // 7. Remove small connected components by area (8-connectivity).
    cv::Mat labels, stats, centroids;
    int n = cv::connectedComponentsWithStats(bordered, labels, stats,
                                             centroids, 8, CV_32S);
    // Label 0 is the background; iterate over foreground labels.
    for (int label = 1; label < n; ++label) {
        int area = stats.at<int>(label, cv::CC_STAT_AREA);
        if (area < area_threshold) {
            // Zero out this component.
            cv::Mat mask = (labels == label);
            bordered.setTo(0, mask);
        }
    }

    // 8. Strip the 1-pixel border back off.
    dst_binary = bordered(cv::Rect(1, 1, src_gray.cols, src_gray.rows)).clone();
    return true;
}

}  // namespace ax
