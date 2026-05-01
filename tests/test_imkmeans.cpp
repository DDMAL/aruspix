// Tier 3b — kmeans clustering on a raw byte buffer.
//
// kmeans() mutates the input buffer (it shifts so that min becomes 1
// before histogramming, and never restores it). Tests must keep a
// snapshot if they want to assert on the input afterwards.

#include <cstdlib>
#include <cstring>

#include <doctest/doctest.h>

#include <im_util.h>  // imbyte

#include "imkmeans.h"

TEST_CASE("kmeans: two well-separated values get two distinct labels and means") {
    imbyte im[8] = {10, 10, 10, 10, 200, 200, 200, 200};
    imbyte mask[8] = {0};

    double *mu = kmeans(im, /*imsize=*/8, mask, /*k=*/2);
    REQUIRE(mu != nullptr);

    // The two halves of the buffer should land in different clusters.
    // Which numeric label corresponds to which cluster is an
    // implementation detail; assert that the labels split cleanly.
    CHECK(mask[0] == mask[1]);
    CHECK(mask[0] == mask[2]);
    CHECK(mask[0] == mask[3]);
    CHECK(mask[4] == mask[5]);
    CHECK(mask[4] == mask[6]);
    CHECK(mask[4] == mask[7]);
    CHECK(mask[0] != mask[4]);

    // Each label must be 1 or 2 (kmeans uses 1-based labels).
    CHECK((mask[0] == 1 || mask[0] == 2));
    CHECK((mask[4] == 1 || mask[4] == 2));

    // Means must be close to the input values, in some order.
    double m_lo = mu[0] < mu[1] ? mu[0] : mu[1];
    double m_hi = mu[0] < mu[1] ? mu[1] : mu[0];
    CHECK(m_lo == doctest::Approx(10.0).epsilon(0.05));
    CHECK(m_hi == doctest::Approx(200.0).epsilon(0.05));

    free(mu);
}

TEST_CASE("kmeans: k=3 produces three labels on three-mode input") {
    imbyte im[12] = {10, 10, 10, 10,
                     100, 100, 100, 100,
                     200, 200, 200, 200};
    imbyte mask[12] = {0};

    double *mu = kmeans(im, /*imsize=*/12, mask, /*k=*/3);
    REQUIRE(mu != nullptr);

    // Three groups, each internally consistent.
    CHECK(mask[0] == mask[3]);
    CHECK(mask[4] == mask[7]);
    CHECK(mask[8] == mask[11]);

    // The three labels are pairwise distinct.
    CHECK(mask[0] != mask[4]);
    CHECK(mask[4] != mask[8]);
    CHECK(mask[0] != mask[8]);

    free(mu);
}
