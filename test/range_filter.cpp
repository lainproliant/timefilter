/*
 * range_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday January 4, 2021
 *
 * Distributed under terms of the MIT license.
 */

#include "moonlight/test.h"
#include "timefilter/range_filter.h"
#include <csignal>
#include <iostream>

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter range_filter tests")
    .test("next_range()", [&]() {
        Datetime dtA(1989, Month::June, 8);
        Datetime dtB(1990, Month::June, 8);
        Datetime dtC(1991, Month::June, 8);
        Range range = Range(Datetime(1990, Month::January, 1),
                            Datetime(1991, Month::December, 1));

        auto filter = RangeFilter(range);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;
        std::cout << "dtC = " << dtC << std::endl;

        auto rangeA = filter.next_range(dtA);
        auto rangeB = filter.next_range(dtB);
        auto rangeC = filter.next_range(dtC);

        std::cout << "rangeA.has_value() = " << rangeA.has_value() << std::endl;
        assert_true(rangeA.has_value());

        std::cout << "rangeA = " << *rangeA << std::endl;
        assert_equal(*rangeA, range);

        std::cout << "rangeB.has_value() = " << rangeB.has_value() << std::endl;
        assert_false(rangeB.has_value());

        std::cout << "rangeC.has_value() = " << rangeC.has_value() << std::endl;
        assert_false(rangeC.has_value());
    })
    .test("prev_range()", [&]() {
        Datetime dtA(1989, Month::June, 8);
        Datetime dtB(1990, Month::June, 8);
        Datetime dtC(1991, Month::June, 8);
        Range range = Range(Datetime(1990, Month::January, 1),
                            Datetime(1991, Month::December, 1));

        auto filter = RangeFilter(range);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;
        std::cout << "dtC = " << dtC << std::endl;

        auto rangeA = filter.prev_range(dtA);
        auto rangeB = filter.prev_range(dtB);
        auto rangeC = filter.prev_range(dtC);

        std::cout << "rangeA.has_value() = " << rangeA.has_value() << std::endl;
        assert_false(rangeA.has_value());

        std::cout << "rangeB.has_value() = " << rangeB.has_value() << std::endl;
        assert_true(rangeB.has_value());

        std::cout << "rangeB = " << *rangeB << std::endl;
        assert_equal(*rangeB, range);

        std::cout << "rangeC.has_value() = " << rangeC.has_value() << std::endl;
        assert_true(rangeC.has_value());

        std::cout << "rangeC = " << *rangeC << std::endl;
        assert_equal(*rangeC, range);
    })
    .die_on_signal(SIGSEGV)
    .run();
}
