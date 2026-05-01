#include "binarize.h"

#include <algorithm>
#include <cstring>

#include <opencv2/imgproc.hpp>

#include <im.h>
#include <im_image.h>

#include "imext.h"

namespace ax {

namespace {

// Cap on the small-element pruning area, mirroring TP_MAX_SMALL_ELEMENT
// in impage.cpp. Anything below this gets removed; the threshold is
// computed from staff-line geometry but capped.
constexpr int kMaxSmallElement = 100;

// Run the chosen threshold method. The migrated im* threshold
// implementations already use OpenCV internally, but they keep the
// imImage* surface for compatibility with the rest of the codebase
// and the doctest suite. We allocate a pair of imImages, copy the
// cv::Mat data through them, and copy the result back. Cheap on
// page-sized images and avoids duplicating the algorithms here.
bool run_threshold(const cv::Mat &src8u, cv::Mat &dst_bin,
                   BinarizationMethod method, int sauvola_region) {
    imImage *src_im = imImageCreate(src8u.cols, src8u.rows, IM_GRAY, IM_BYTE);
    imImage *dst_im = imImageCreate(src8u.cols, src8u.rows, IM_BINARY, IM_BYTE);
    if (!src_im || !dst_im) {
        if (src_im) imImageDestroy(src_im);
        if (dst_im) imImageDestroy(dst_im);
        return false;
    }
    std::memcpy(src_im->data[0], src8u.data, src_im->count);

    bool ok = true;
    switch (method) {
        case BinarizationMethod::Sauvola:
            ok = imProcessSauvolaThreshold(src_im, dst_im, sauvola_region,
                                           0.5f, 128, 20, 150,
                                           /*white_is_255=*/false) != 0;
            break;
        case BinarizationMethod::Brink2Classes:
            imProcessBrink2ClassesThreshold(src_im, dst_im,
                                            /*white_is_255=*/false,
                                            BRINK_AND_PENDOCK);
            break;
        case BinarizationMethod::Brink3Classes:
            imProcessBrink3ClassesThreshold(src_im, dst_im,
                                            /*white_is_255=*/false,
                                            BRINK_AND_PENDOCK);
            break;
        case BinarizationMethod::FixedAt127: {
            // Fallback: cv::threshold with THRESH_BINARY_INV gives the
            // same convention as the rest (foreground=1).
            cv::Mat src_view(src_im->height, src_im->width, CV_8UC1,
                             src_im->data[0]);
            cv::Mat dst_view(dst_im->height, dst_im->width, CV_8UC1,
                             dst_im->data[0]);
            cv::threshold(src_view, dst_view, 127, 1, cv::THRESH_BINARY_INV);
            break;
        }
    }

    dst_bin.create(src8u.rows, src8u.cols, CV_8UC1);
    std::memcpy(dst_bin.data, dst_im->data[0], dst_im->count);

    imImageDestroy(src_im);
    imImageDestroy(dst_im);
    return ok;
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
