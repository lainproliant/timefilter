/*
 * month_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Tuesday December 29, 2020
 *
 * Distributed under terms of the MIT license.
 */

#include <csignal>
#include <iostream>
#include "moonlight/test.h"
#include "timefilter/filters.h"

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter month_filter tests")
    .test("next_range()", [&]() {
        Datetime dt(1988, Month::June, 8);
        auto filterA = MonthFilter::create(Month::June);
        auto filterB = MonthFilter::create(Month::May);
        auto filterC = MonthFilter::create(Month::September);

        std::cout << "dt = " << dt << std::endl;

        auto rangeA = filterA->next_range(dt);
        auto rangeB = filterB->next_range(dt);
        auto rangeC = filterC->next_range(dt);

        ASSERT_TRUE(rangeA.has_value());
        std::cout << "rangeA = " << *rangeA << std::endl;
        ASSERT_EQUAL(*rangeA, Range(Datetime(1989, Month::June, 1),
                                    Datetime(1989, Month::July, 1)));

        ASSERT_TRUE(rangeB.has_value());
        std::cout << "rangeB = " << *rangeB << std::endl;
        ASSERT_EQUAL(*rangeB, Range(Datetime(1989, Month::May, 1),
                                    Datetime(1989, Month::June, 1)));

        ASSERT_TRUE(rangeC.has_value());
        std::cout << "rangeC = " << *rangeC << std::endl;
        ASSERT_EQUAL(*rangeC, Range(Datetime(1988, Month::September, 1),
                                    Datetime(1988, Month::October, 1)));
    })
    .test("next_range() on month boundaries", [&]() {
        Datetime dtA(1988, Month::June, 1);
        Datetime dtB(1988, Month::June, 30);
        Datetime dtC(1988, Month::February, 29);
        Datetime dtD(1987, Month::February, 28);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;
        std::cout << "dtC = " << dtC << std::endl;
        std::cout << "dtD = " << dtD << std::endl;

        auto filterA = MonthFilter::create(Month::June);
        auto filterB = MonthFilter::create(Month::February);

        auto rangeAA = filterA->next_range(dtA);
        auto rangeAB = filterA->next_range(dtB);
        auto rangeAC = filterA->next_range(dtC);
        auto rangeAD = filterA->next_range(dtD);
        auto rangeBA = filterB->next_range(dtA);
        auto rangeBB = filterB->next_range(dtB);
        auto rangeBC = filterB->next_range(dtC);
        auto rangeBD = filterB->next_range(dtD);

        ASSERT_TRUE(rangeAA.has_value());
        std::cout << "rangeAA = " << *rangeAA << std::endl;
        ASSERT_EQUAL(*rangeAA, Range(Datetime(1989, Month::June, 1),
                                     Datetime(1989, Month::July, 1)));

        ASSERT_TRUE(rangeAB.has_value());
        std::cout << "rangeAB = " << *rangeAB << std::endl;
        ASSERT_EQUAL(*rangeAB, Range(Datetime(1989, Month::June, 1),
                                     Datetime(1989, Month::July, 1)));

        ASSERT_TRUE(rangeAC.has_value());
        std::cout << "rangeAC = " << *rangeAC << std::endl;
        ASSERT_EQUAL(*rangeAC, Range(Datetime(1988, Month::June, 1),
                                     Datetime(1988, Month::July, 1)));

        ASSERT_TRUE(rangeAD.has_value());
        std::cout << "rangeAD = " << *rangeAD << std::endl;
        ASSERT_EQUAL(*rangeAD, Range(Datetime(1987, Month::June, 1),
                                     Datetime(1987, Month::July, 1)));

        ASSERT_TRUE(rangeBA.has_value());
        std::cout << "rangeBA = " << *rangeBA << std::endl;
        ASSERT_EQUAL(*rangeBA, Range(Datetime(1989, Month::February, 1),
                                     Datetime(1989, Month::March, 1)));

        ASSERT_TRUE(rangeBB.has_value());
        std::cout << "rangeBB = " << *rangeBB << std::endl;
        ASSERT_EQUAL(*rangeBB, Range(Datetime(1989, Month::February, 1),
                                     Datetime(1989, Month::March, 1)));

        ASSERT_TRUE(rangeBC.has_value());
        std::cout << "rangeBC = " << *rangeBC << std::endl;
        ASSERT_EQUAL(*rangeBC, Range(Datetime(1989, Month::February, 1),
                                     Datetime(1989, Month::March, 1)));

        ASSERT_TRUE(rangeBD.has_value());
        std::cout << "rangeBD = " << *rangeBD << std::endl;
        ASSERT_EQUAL(*rangeBD, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::March, 1)));
    })
    .test("prev_range() on month boundaries", [&]() {
        Datetime dtA(1988, Month::June, 1);
        Datetime dtB(1988, Month::June, 30);
        Datetime dtC(1988, Month::February, 29);
        Datetime dtD(1987, Month::February, 28);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;
        std::cout << "dtC = " << dtC << std::endl;
        std::cout << "dtD = " << dtD << std::endl;

        auto filterA = MonthFilter::create(Month::June);
        auto filterB = MonthFilter::create(Month::February);

        auto rangeAA = filterA->prev_range(dtA);
        auto rangeAB = filterA->prev_range(dtB);
        auto rangeAC = filterA->prev_range(dtC);
        auto rangeAD = filterA->prev_range(dtD);
        auto rangeBA = filterB->prev_range(dtA);
        auto rangeBB = filterB->prev_range(dtB);
        auto rangeBC = filterB->prev_range(dtC);
        auto rangeBD = filterB->prev_range(dtD);

        ASSERT_TRUE(rangeAA.has_value());
        std::cout << "rangeAA = " << *rangeAA << std::endl;
        ASSERT_EQUAL(*rangeAA, Range(Datetime(1988, Month::June, 1),
                                     Datetime(1988, Month::July, 1)));

        ASSERT_TRUE(rangeAB.has_value());
        std::cout << "rangeAB = " << *rangeAB << std::endl;
        ASSERT_EQUAL(*rangeAB, Range(Datetime(1988, Month::June, 1),
                                     Datetime(1988, Month::July, 1)));

        ASSERT_TRUE(rangeAC.has_value());
        std::cout << "rangeAC = " << *rangeAC << std::endl;
        ASSERT_EQUAL(*rangeAC, Range(Datetime(1987, Month::June, 1),
                                     Datetime(1987, Month::July, 1)));

        ASSERT_TRUE(rangeAD.has_value());
        std::cout << "rangeAD = " << *rangeAD << std::endl;
        ASSERT_EQUAL(*rangeAD, Range(Datetime(1986, Month::June, 1),
                                     Datetime(1986, Month::July, 1)));

        ASSERT_TRUE(rangeBA.has_value());
        std::cout << "rangeBA = " << *rangeBA << std::endl;
        ASSERT_EQUAL(*rangeBA, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::March, 1)));

        ASSERT_TRUE(rangeBB.has_value());
        std::cout << "rangeBB = " << *rangeBB << std::endl;
        ASSERT_EQUAL(*rangeBB, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::March, 1)));

        ASSERT_TRUE(rangeBC.has_value());
        std::cout << "rangeBC = " << *rangeBC << std::endl;
        ASSERT_EQUAL(*rangeBC, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::March, 1)));

        ASSERT_TRUE(rangeBD.has_value());
        std::cout << "rangeBD = " << *rangeBD << std::endl;
        ASSERT_EQUAL(*rangeBD, Range(Datetime(1987, Month::February, 1),
                                     Datetime(1987, Month::March, 1)));
    })
    .test("prev_range()", [&]() {
        Datetime dt(1988, Month::June, 8);
        auto filterA = MonthFilter::create(Month::June);
        auto filterB = MonthFilter::create(Month::May);
        auto filterC = MonthFilter::create(Month::September);

        std::cout << "dt = " << dt << std::endl;

        auto rangeA = filterA->prev_range(dt);
        auto rangeB = filterB->prev_range(dt);
        auto rangeC = filterC->prev_range(dt);

        ASSERT_TRUE(rangeA.has_value());
        std::cout << "rangeA = " << *rangeA << std::endl;
        ASSERT_EQUAL(*rangeA, Range(Datetime(1988, Month::June, 1),
                                    Datetime(1988, Month::July, 1)));

        ASSERT_TRUE(rangeB.has_value());
        std::cout << "rangeB = " << *rangeB << std::endl;
        ASSERT_EQUAL(*rangeB, Range(Datetime(1988, Month::May, 1),
                                    Datetime(1988, Month::June, 1)));

        ASSERT_TRUE(rangeC.has_value());
        std::cout << "rangeC = " << *rangeC << std::endl;
        ASSERT_EQUAL(*rangeC, Range(Datetime(1987, Month::September, 1),
                                    Datetime(1987, Month::October, 1)));
    })
    .die_on_signal(SIGSEGV)
    .run();
}
