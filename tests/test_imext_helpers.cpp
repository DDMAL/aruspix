// Tier 1 — pure helpers from imext.cpp. No imImage involved.
//
// These are tiny and arguably trivial, but the test file is also a
// canary: if doctest + the build is wired correctly, this file alone
// will compile and run. Keep it cheap and deterministic.

#include <doctest/doctest.h>

#include "imext.h"

TEST_CASE("median: odd-length array returns middle element") {
    int a[] = {7, 3, 1, 9, 5};
    CHECK(median(a, 5, /*sort_array=*/false) == 5);
}

TEST_CASE("median: even-length array averages the two middle elements") {
    int a[] = {1, 2, 3, 4};
    CHECK(median(a, 4, /*sort_array=*/false) == 2);  // (2+3)/2 = 2 (int division)
}

TEST_CASE("median: sort_array=true sorts in place") {
    int a[] = {7, 3, 1, 9, 5};
    median(a, 5, /*sort_array=*/true);
    for (int i = 1; i < 5; ++i) {
        CHECK(a[i - 1] <= a[i]);
    }
}

TEST_CASE("median: sort_array=false leaves input untouched") {
    int a[] = {7, 3, 1, 9, 5};
    int snap[5];
    for (int i = 0; i < 5; ++i) snap[i] = a[i];
    median(a, 5, /*sort_array=*/false);
    for (int i = 0; i < 5; ++i) {
        CHECK(a[i] == snap[i]);
    }
}

TEST_CASE("median: empty / null input returns 0") {
    CHECK(median(nullptr, 0) == 0);
    int a[] = {42};
    CHECK(median(a, 0) == 0);
}

TEST_CASE("medianf: float variant") {
    float a[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    CHECK(medianf(a, 5, /*sort_array=*/false) == doctest::Approx(3.0f));
}

TEST_CASE("max_val: returns largest and reports its position") {
    int a[] = {1, 4, 2, 8, 3};
    int pos = -1;
    CHECK(max_val(a, 5, &pos) == 8);
    CHECK(pos == 3);
}

TEST_CASE("max_val: ties take the last index (>= comparison)") {
    int a[] = {5, 5, 5};
    int pos = -1;
    CHECK(max_val(a, 3, &pos) == 5);
    CHECK(pos == 2);  // documents current behavior
}

TEST_CASE("max_val: pos pointer is optional") {
    int a[] = {1, 4, 2, 8, 3};
    CHECK(max_val(a, 5, nullptr) == 8);
}

TEST_CASE("max_val: empty input returns 0 and sets pos to 0") {
    int pos = 99;
    CHECK(max_val(nullptr, 0, &pos) == 0);
    CHECK(pos == 0);
}

TEST_CASE("sum: adds elements") {
    int a[] = {1, 2, 3, 4, 5};
    CHECK(sum(a, 5) == 15);
}

TEST_CASE("sum: empty returns 0") {
    CHECK(sum(nullptr, 0) == 0);
}

TEST_CASE("count: counts non-zero elements") {
    int a[] = {0, 1, 0, 2, 3, 0};
    CHECK(count(a, 6) == 3);
}

TEST_CASE("count: empty returns 0") {
    CHECK(count(nullptr, 0) == 0);
}

TEST_CASE("corr: identical signals correlate at zero shift") {
    int a[] = {1, 0, 1, 0, 1, 0, 1, 0};
    int b[] = {1, 0, 1, 0, 1, 0, 1, 0};
    int dec = 999, max = -1;
    corr(a, b, /*size=*/8, /*win=*/3, &dec, &max);
    CHECK(dec == 0);
    CHECK(max == 4);  // sum of a[i]*b[i] for matching ones
}

TEST_CASE("corr: shifted signal reports the correct decalage") {
    // 'b' has its impulse 2 samples to the right of 'a's impulse.
    // corr scans 'a' across a window of [-win, +win); the maximum
    // overlap occurs when the window is shifted *left* by 2, so
    // dec = -2. (Sign convention: dec is the offset of 'a' relative
    // to 'b', not the other way around.)
    int b[] = {0, 0, 1, 1, 0, 0, 0, 0};
    int a[] = {1, 1, 0, 0, 0, 0, 0, 0};
    int dec = 999, max = -1;
    corr(a, b, /*size=*/8, /*win=*/3, &dec, &max);
    CHECK(dec == -2);
    CHECK(max == 2);
}

TEST_CASE("corr: zero-correlation case sets dec to 0") {
    int a[] = {0, 0, 0, 0};
    int b[] = {1, 1, 1, 1};
    int dec = 999, max = -1;
    corr(a, b, /*size=*/4, /*win=*/2, &dec, &max);
    CHECK(max == 0);
    CHECK(dec == 0);
}

TEST_CASE("alloc2DArray / free2DArray: round-trip writes survive") {
    double **m = alloc2DArray(3, 4);
    REQUIRE(m != nullptr);
    for (int i = 0; i < 3; ++i) {
        REQUIRE(m[i] != nullptr);
        for (int j = 0; j < 4; ++j) m[i][j] = i * 10.0 + j;
    }
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 4; ++j)
            CHECK(m[i][j] == doctest::Approx(i * 10.0 + j));
    free2DArray(m, 3);
}
