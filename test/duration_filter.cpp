/*
 * duration_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday August 4, 2021
 *
 * Distributed under terms of the MIT license.
 */

#include "moonlight/test.h"
#include "timefilter/range_filter.h"
#include "timefilter/duration_filter.h"
#include <csignal>
#include <iostream>

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter duration_filter tests")
    .die_on_signal(SIGSEGV)
    .test("next_range() for duration filter", [&]() {
        Datetime dtA(1988, Month::June, 8, 12, 00);
        Datetime dtPivot(1980, Month::January, 1);

        auto filterA = RangeFilter::create(dtA, dtA + seconds(1));
        auto rangeA = filterA->next_range(dtPivot);
        std::cout << "rangeA = " << rangeA.value() << ", " << rangeA->duration() << std::endl;
        assert_equal(rangeA->duration(), seconds(1));

        auto filterB = DurationFilter::create(filterA, days(1));
        auto rangeB = filterB->next_range(dtPivot);
        std::cout << "rangeB = " << rangeB.value() << ", " << rangeB->duration() << std::endl;
        assert_equal(rangeB->duration(), days(1));
    })
    .test("prev_range() for duration filter", [&]() {
        Datetime dtA(1988, Month::June, 8, 12, 00);
        Datetime dtPivot(2525, Month::January, 1);

        auto filterA = RangeFilter::create(dtA, dtA + seconds(1));
        auto rangeA = filterA->prev_range(dtPivot);
        std::cout << "rangeA = " << rangeA.value() << ", " << rangeA->duration() << std::endl;
        assert_equal(rangeA->duration(), seconds(1));

        auto filterB = DurationFilter::create(filterA, days(1));
        auto rangeB = filterB->prev_range(dtPivot);
        std::cout << "rangeB = " << rangeB.value() << ", " << rangeB->duration() << std::endl;
        assert_equal(rangeB->duration(), days(1));
    })
    .run();
}
