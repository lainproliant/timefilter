/*
 * weekday_of_month_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Friday January 1, 2021
 *
 * Distributed under terms of the MIT license.
 */

#include "moonlight/test.h"
#include "timefilter/weekday_of_month_filter.h"
#include <csignal>
#include <iostream>

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter weekday_of_month_tests")
    .test("next_range()", [&]() {
        Datetime dtA = Datetime(2020, Month::January, 1);
        Datetime dtB = Datetime(2021, Month::January, 20);

        auto filterA = WeekdayOfMonthFilter::create(Weekday::Wednesday, 2);
        auto filterB = WeekdayOfMonthFilter::create(Weekday::Tuesday, -1);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;

        auto rangeAA = filterA->next_range(dtA);
        auto rangeAB = filterA->next_range(dtB);
        auto rangeBA = filterB->next_range(dtA);
        auto rangeBB = filterB->next_range(dtB);

        std::cout << "rangeAA.has_value() = " << rangeAA.has_value() << std::endl;
        assert_true(rangeAA.has_value());

        std::cout << "rangeAA = " << *rangeAA << std::endl;
        assert_equal(*rangeAA, Range(Datetime(2020, Month::January, 8),
                                     Datetime(2020, Month::January, 9)));

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        assert_true(rangeAB.has_value());

        std::cout << "rangeAB = " << *rangeAB << std::endl;
        assert_equal(*rangeAB, Range(Datetime(2021, Month::February, 10),
                                     Datetime(2021, Month::February, 11)));

        std::cout << "rangeBA.has_value() = " << rangeBA.has_value() << std::endl;
        assert_true(rangeBA.has_value());

        std::cout << "rangeBA = " << *rangeBA << std::endl;
        assert_equal(*rangeBA, Range(Datetime(2020, Month::January, 28),
                                     Datetime(2020, Month::January, 29)));

        std::cout << "rangeBB.has_value() = " << rangeBB.has_value() << std::endl;
        assert_true(rangeBB.has_value());

        std::cout << "rangeBB = " << *rangeBB << std::endl;
        assert_equal(*rangeBB, Range(Datetime(2021, Month::January, 26),
                                     Datetime(2021, Month::January, 27)));
    })
    .test("prev_range()", [&]() {
        Datetime dtA = Datetime(2020, Month::January, 1);
        Datetime dtB = Datetime(2021, Month::January, 20);

        auto filterA = WeekdayOfMonthFilter::create(Weekday::Wednesday, 2);
        auto filterB = WeekdayOfMonthFilter::create(Weekday::Tuesday, -1);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;

        auto rangeAA = filterA->prev_range(dtA);
        auto rangeAB = filterA->prev_range(dtB);
        auto rangeBA = filterB->prev_range(dtA);
        auto rangeBB = filterB->prev_range(dtB);

        std::cout << "rangeAA.has_value() = " << rangeAA.has_value() << std::endl;
        assert_true(rangeAA.has_value());

        std::cout << "rangeAA = " << *rangeAA << std::endl;
        assert_equal(*rangeAA, Range(Datetime(2019, Month::December, 11),
                                     Datetime(2019, Month::December, 12)));

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        assert_true(rangeAB.has_value());

        std::cout << "rangeAB = " << *rangeAB << std::endl;
        assert_equal(*rangeAB, Range(Datetime(2021, Month::January, 13),
                                     Datetime(2021, Month::January, 14)));

        std::cout << "rangeBA.has_value() = " << rangeBA.has_value() << std::endl;
        assert_true(rangeBA.has_value());

        std::cout << "rangeBA = " << *rangeBA << std::endl;
        assert_equal(*rangeBA, Range(Datetime(2019, Month::December, 31),
                                     Datetime(2020, Month::January, 1)));

        std::cout << "rangeBB.has_value() = " << rangeBB.has_value() << std::endl;
        assert_true(rangeBB.has_value());

        std::cout << "rangeBB = " << *rangeBB << std::endl;
        assert_equal(*rangeBB, Range(Datetime(2020, Month::December, 29),
                                     Datetime(2020, Month::December, 30)));
    })
    .run();
}
