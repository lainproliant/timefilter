/*
 * year_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Friday December 25, 2020
 *
 * Distributed under terms of the MIT license.
 */

#include "moonlight/test.h"
#include "timefilter/year_filter.h"
#include <csignal>
#include <iostream>

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter year_filter tests")
    .test("next_range()", [&]() {
        Datetime dt(1980, Month::April, 1);
        YearFilter filterA(1981);
        YearFilter filterB(1980);
        YearFilter filterC(1979);

        auto rangeA = filterA.next_range(dt);
        auto rangeB = filterB.next_range(dt);
        auto rangeC = filterC.next_range(dt);

        assert_true(rangeA.has_value());
        std::cout << "rangeA = " << *rangeA << std::endl;
        assert_equal(*rangeA, Range(Datetime(1981, Month::January, 1),
                                    Datetime(1982, Month::January, 1)));
        assert_false(rangeB.has_value());
        assert_false(rangeC.has_value());
    })
    .test("prev_range()", [&]() {
        Datetime dt(1980, Month::April, 1);
        YearFilter filterA(1981);
        YearFilter filterB(1980);
        YearFilter filterC(1979);

        auto rangeA = filterA.prev_range(dt);
        auto rangeB = filterB.prev_range(dt);
        auto rangeC = filterC.prev_range(dt);

        assert_false(rangeA.has_value());
        assert_true(rangeB.has_value());
        std::cout << "rangeB = " << *rangeB << std::endl;
        assert_equal(*rangeB, Range(Datetime(1980, Month::January, 1),
                                    Datetime(1981, Month::January, 1)));
        std::cout << "rangeC = " << *rangeC << std::endl;
        assert_equal(*rangeC, Range(Datetime(1979, Month::January, 1),
                                    Datetime(1980, Month::January, 1)));
    })
    .die_on_signal(SIGSEGV)
    .run();
}
