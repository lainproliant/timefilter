/*
 * duration_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday August 4, 2021
 *
 * Distributed under terms of the MIT license.
 */

#include <csignal>
#include <iostream>
#include "moonlight/test.h"
#include "timefilter/duration.h"
#include "timefilter/static_range.h"

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter duration_filter tests")
    .die_on_signal(SIGSEGV)
    .test("next_range() for duration filter", [&]() {
        Datetime dtA(1988, Month::June, 8, 12, 00);
        Datetime dtPivot(1980, Month::January, 1);

        auto filterA = StaticRangeFilter::create(dtA, seconds(1));
        auto rangeA = filterA->next_range(dtPivot);
        std::cout << "rangeA = " << rangeA.value() << ", " << rangeA->duration() << std::endl;
        ASSERT_EQUAL(rangeA->duration(), seconds(1));

        auto filterB = FilterDuration::create(filterA, days(1));
        auto rangeB = filterB->next_range(dtPivot);
        std::cout << "rangeB = " << rangeB.value() << ", " << rangeB->duration() << std::endl;
        ASSERT_EQUAL(rangeB->duration(), days(1));
    })
    .test("prev_range() for duration filter", [&]() {
        Datetime dtA(1988, Month::June, 8, 12, 00);
        Datetime dtPivot(2525, Month::January, 1);

        auto filterA = StaticRangeFilter::create(dtA, seconds(1));
        auto rangeA = filterA->prev_range(dtPivot);
        std::cout << "rangeA = " << rangeA.value() << ", " << rangeA->duration() << std::endl;
        ASSERT_EQUAL(rangeA->duration(), seconds(1));

        auto filterB = FilterDuration::create(filterA, days(1));
        auto rangeB = filterB->prev_range(dtPivot);
        std::cout << "rangeB = " << rangeB.value() << ", " << rangeB->duration() << std::endl;
        ASSERT_EQUAL(rangeB->duration(), days(1));
    })
    .run();
}
