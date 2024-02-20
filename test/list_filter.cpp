/*
 * list_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday January 6, 2021
 *
 * Distributed under terms of the MIT license.
 */
#include <csignal>
#include <iostream>
#include "moonlight/test.h"
#include "timefilter/list_filter.h"
#include "timefilter/month_filter.h"
#include "timefilter/weekday_filter.h"
#include "timefilter/weekday_of_month_filter.h"
#include "timefilter/time_filter.h"

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter list_filter tests")
    .test("next_range()", [&]() {
        Datetime dtA(2021, Month::January, 6);
        Datetime dtB(2020, Month::June, 8);
        Datetime dtC(2021, Month::December, 2, 12, 0);
        Datetime dtD(2021, Month::December, 1, 13, 0);
        Datetime dtE(2029, Month::February, 1);
        Datetime dtF(2022, Month::January, 31);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;
        std::cout << "dtC = " << dtC << std::endl;
        std::cout << "dtD = " << dtD << std::endl;
        std::cout << "dtE = " << dtE << std::endl;
        std::cout << "dtF = " << dtF << std::endl;

        auto filterA = ListFilter()
            .push(MonthFilter::create(Month::December))
            .push(WeekdayFilter::create(Weekday::Monday, Weekday::Wednesday, Weekday::Friday))
            .push(TimeFilter::create(Time(12, 15)));

        auto filterB = ListFilter()
            .push(MonthFilter::create(Month::January, Month::June))
            .push(WeekdayOfMonthFilter::create(Weekday::Sunday, -1))
            .push(TimeFilter::create(Time(10, 30)));

        auto rangeAA = filterA.next_range(dtA);
        auto rangeAB = filterA.next_range(dtB);
        auto rangeAC = filterA.next_range(dtC);
        auto rangeAD = filterA.next_range(dtD);
        auto rangeAE = filterA.next_range(dtE);
        auto rangeAF = filterA.next_range(dtF);
        auto rangeBA = filterB.next_range(dtA);
        auto rangeBB = filterB.next_range(dtB);
        auto rangeBC = filterB.next_range(dtC);
        auto rangeBD = filterB.next_range(dtD);
        auto rangeBE = filterB.next_range(dtE);
        auto rangeBF = filterB.next_range(dtF);

        std::cout << "rangeAA.has_value() = " << rangeAA.has_value() << std::endl;
        ASSERT_TRUE(rangeAA.has_value());
        std::cout << "rangeAA = " << *rangeAA << std::endl;
        ASSERT_EQUAL(*rangeAA, Range(Datetime(2021, Month::December, 1, 12, 15),
                                     Datetime(2021, Month::December, 1, 12, 16)));

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        ASSERT_TRUE(rangeAB.has_value());
        std::cout << "rangeAB = " << *rangeAB << std::endl;
        ASSERT_EQUAL(*rangeAB, Range(Datetime(2020, Month::December, 2, 12, 15),
                                     Datetime(2020, Month::December, 2, 12, 16)));

        std::cout << "rangeAC.has_value() = " << rangeAC.has_value() << std::endl;
        ASSERT_TRUE(rangeAC.has_value());
        std::cout << "rangeAC = " << *rangeAC << std::endl;
        ASSERT_EQUAL(*rangeAC, Range(Datetime(2021, Month::December, 3, 12, 15),
                                     Datetime(2021, Month::December, 3, 12, 16)));

        std::cout << "rangeAD.has_value() = " << rangeAD.has_value() << std::endl;
        ASSERT_TRUE(rangeAD.has_value());
        std::cout << "rangeAD = " << *rangeAD << std::endl;
        ASSERT_EQUAL(*rangeAD, Range(Datetime(2021, Month::December, 3, 12, 15),
                                     Datetime(2021, Month::December, 3, 12, 16)));

        std::cout << "rangeAE.has_value() = " << rangeAE.has_value() << std::endl;
        ASSERT_TRUE(rangeAE.has_value());
        std::cout << "rangeAE = " << *rangeAE << std::endl;
        ASSERT_EQUAL(*rangeAE, Range(Datetime(2029, Month::December, 3, 12, 15),
                                     Datetime(2029, Month::December, 3, 12, 16)));

        std::cout << "rangeAF.has_value() = " << rangeAF.has_value() << std::endl;
        ASSERT_TRUE(rangeAF.has_value());
        std::cout << "rangeAF = " << *rangeAF << std::endl;
        ASSERT_EQUAL(*rangeAF, Range(Datetime(2022, Month::December, 2, 12, 15),
                                     Datetime(2022, Month::December, 2, 12, 16)));

        std::cout << "rangeBA.has_value() = " << rangeBA.has_value() << std::endl;
        ASSERT_TRUE(rangeBA.has_value());
        std::cout << "rangeBA = " << *rangeBA << std::endl;
        ASSERT_EQUAL(*rangeBA, Range(Datetime(2021, Month::January, 31, 10, 30),
                                     Datetime(2021, Month::January, 31, 10, 31)));

        std::cout << "rangeBB.has_value() = " << rangeBB.has_value() << std::endl;
        ASSERT_TRUE(rangeBB.has_value());
        std::cout << "rangeBB = " << *rangeBB << std::endl;
        ASSERT_EQUAL(*rangeBB, Range(Datetime(2020, Month::June, 28, 10, 30),
                                     Datetime(2020, Month::June, 28, 10, 31)));

        std::cout << "rangeBC.has_value() = " << rangeBC.has_value() << std::endl;
        ASSERT_TRUE(rangeBC.has_value());
        std::cout << "rangeBC = " << *rangeBC << std::endl;
        ASSERT_EQUAL(*rangeBC, Range(Datetime(2022, Month::January, 30, 10, 30),
                                     Datetime(2022, Month::January, 30, 10, 31)));

        std::cout << "rangeBD.has_value() = " << rangeBD.has_value() << std::endl;
        ASSERT_TRUE(rangeBD.has_value());
        std::cout << "rangeBD = " << *rangeBD << std::endl;
        ASSERT_EQUAL(*rangeBD, Range(Datetime(2022, Month::January, 30, 10, 30),
                                     Datetime(2022, Month::January, 30, 10, 31)));

        std::cout << "rangeBE.has_value() = " << rangeBE.has_value() << std::endl;
        ASSERT_TRUE(rangeBE.has_value());
        std::cout << "rangeBE = " << *rangeBE << std::endl;
        ASSERT_EQUAL(*rangeBE, Range(Datetime(2029, Month::June, 24, 10, 30),
                                     Datetime(2029, Month::June, 24, 10, 31)));

        std::cout << "rangeBF.has_value() = " << rangeBF.has_value() << std::endl;
        ASSERT_TRUE(rangeBF.has_value());
        std::cout << "rangeBF = " << *rangeBF << std::endl;
        ASSERT_EQUAL(*rangeBF, Range(Datetime(2022, Month::June, 26, 10, 30),
                                     Datetime(2022, Month::June, 26, 10, 31)));
    })
    .test("prev_range()", [&]() {
        Datetime dtA(2021, Month::January, 6);
        Datetime dtB(2020, Month::June, 8);
        Datetime dtC(2021, Month::December, 2, 12, 0);
        Datetime dtD(2021, Month::December, 1, 13, 0);
        Datetime dtE(2029, Month::February, 1);
        Datetime dtF(2022, Month::January, 31);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;
        std::cout << "dtC = " << dtC << std::endl;
        std::cout << "dtD = " << dtD << std::endl;
        std::cout << "dtE = " << dtE << std::endl;
        std::cout << "dtF = " << dtF << std::endl;

        auto filterA = ListFilter()
            .push(MonthFilter::create(Month::December))
            .push(WeekdayFilter::create(Weekday::Monday, Weekday::Wednesday, Weekday::Friday))
            .push(TimeFilter::create(Time(12, 15)));

        auto filterB = ListFilter()
            .push(MonthFilter::create(Month::January, Month::June))
            .push(WeekdayOfMonthFilter::create(Weekday::Sunday, -1))
            .push(TimeFilter::create(Time(10, 30)));

        auto rangeAA = filterA.prev_range(dtA);
        auto rangeAB = filterA.prev_range(dtB);
        auto rangeAC = filterA.prev_range(dtC);
        auto rangeAD = filterA.prev_range(dtD);
        auto rangeAE = filterA.prev_range(dtE);
        auto rangeAF = filterA.prev_range(dtF);
        auto rangeBA = filterB.prev_range(dtA);
        auto rangeBB = filterB.prev_range(dtB);
        auto rangeBC = filterB.prev_range(dtC);
        auto rangeBD = filterB.prev_range(dtD);
        auto rangeBE = filterB.prev_range(dtE);
        auto rangeBF = filterB.prev_range(dtF);

        std::cout << "rangeAA.has_value() = " << rangeAA.has_value() << std::endl;
        ASSERT_TRUE(rangeAA.has_value());
        std::cout << "rangeAA = " << *rangeAA << std::endl;
        ASSERT_EQUAL(*rangeAA, Range(Datetime(2020, Month::December, 30, 12, 15),
                                     Datetime(2020, Month::December, 30, 12, 16)));

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        ASSERT_TRUE(rangeAB.has_value());
        std::cout << "rangeAB = " << *rangeAB << std::endl;
        ASSERT_EQUAL(*rangeAB, Range(Datetime(2019, Month::December, 30, 12, 15),
                                     Datetime(2019, Month::December, 30, 12, 16)));

        std::cout << "rangeAC.has_value() = " << rangeAC.has_value() << std::endl;
        ASSERT_TRUE(rangeAC.has_value());
        std::cout << "rangeAC = " << *rangeAC << std::endl;
        ASSERT_EQUAL(*rangeAC, Range(Datetime(2021, Month::December, 1, 12, 15),
                                     Datetime(2021, Month::December, 1, 12, 16)));

        std::cout << "rangeAD.has_value() = " << rangeAD.has_value() << std::endl;
        ASSERT_TRUE(rangeAD.has_value());
        std::cout << "rangeAD = " << *rangeAD << std::endl;
        ASSERT_EQUAL(*rangeAD, Range(Datetime(2021, Month::December, 1, 12, 15),
                                     Datetime(2021, Month::December, 1, 12, 16)));

        std::cout << "rangeAE.has_value() = " << rangeAE.has_value() << std::endl;
        ASSERT_TRUE(rangeAE.has_value());
        std::cout << "rangeAE = " << *rangeAE << std::endl;
        ASSERT_EQUAL(*rangeAE, Range(Datetime(2028, Month::December, 29, 12, 15),
                                     Datetime(2028, Month::December, 29, 12, 16)));

        std::cout << "rangeAF.has_value() = " << rangeAF.has_value() << std::endl;
        ASSERT_TRUE(rangeAF.has_value());
        std::cout << "rangeAF = " << *rangeAF << std::endl;
        ASSERT_EQUAL(*rangeAF, Range(Datetime(2021, Month::December, 31, 12, 15),
                                     Datetime(2021, Month::December, 31, 12, 16)));

        std::cout << "rangeBA.has_value() = " << rangeBA.has_value() << std::endl;
        ASSERT_TRUE(rangeBA.has_value());
        std::cout << "rangeBA = " << *rangeBA << std::endl;
        ASSERT_EQUAL(*rangeBA, Range(Datetime(2020, Month::June, 28, 10, 30),
                                     Datetime(2020, Month::June, 28, 10, 31)));

        std::cout << "rangeBB.has_value() = " << rangeBB.has_value() << std::endl;
        ASSERT_TRUE(rangeBB.has_value());
        std::cout << "rangeBB = " << *rangeBB << std::endl;
        ASSERT_EQUAL(*rangeBB, Range(Datetime(2020, Month::January, 26, 10, 30),
                                     Datetime(2020, Month::January, 26, 10, 31)));

        std::cout << "rangeBC.has_value() = " << rangeBC.has_value() << std::endl;
        ASSERT_TRUE(rangeBC.has_value());
        std::cout << "rangeBC = " << *rangeBC << std::endl;
        ASSERT_EQUAL(*rangeBC, Range(Datetime(2021, Month::June, 27, 10, 30),
                                     Datetime(2021, Month::June, 27, 10, 31)));

        std::cout << "rangeBD.has_value() = " << rangeBD.has_value() << std::endl;
        ASSERT_TRUE(rangeBD.has_value());
        std::cout << "rangeBD = " << *rangeBD << std::endl;
        ASSERT_EQUAL(*rangeBD, Range(Datetime(2021, Month::June, 27, 10, 30),
                                     Datetime(2021, Month::June, 27, 10, 31)));

        std::cout << "rangeBE.has_value() = " << rangeBE.has_value() << std::endl;
        ASSERT_TRUE(rangeBE.has_value());
        std::cout << "rangeBE = " << *rangeBE << std::endl;
        ASSERT_EQUAL(*rangeBE, Range(Datetime(2029, Month::January, 28, 10, 30),
                                     Datetime(2029, Month::January, 28, 10, 31)));

        std::cout << "rangeBF.has_value() = " << rangeBF.has_value() << std::endl;
        ASSERT_TRUE(rangeBF.has_value());
        std::cout << "rangeBF = " << *rangeBF << std::endl;
        ASSERT_EQUAL(*rangeBF, Range(Datetime(2022, Month::January, 30, 10, 30),
                                     Datetime(2022, Month::January, 30, 10, 31)));
    })
    .die_on_signal(SIGSEGV)
    .run();
}
