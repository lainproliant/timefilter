/*
 * month_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Tuesday December 29, 2020
 *
 * Distributed under terms of the MIT license.
 */

#include "moonlight/test.h"
#include "timefilter/month_filter.h"
#include <csignal>
#include <iostream>

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter month_filter tests")
    .test("next_range()", [&]() {
        Datetime dt (1988, Month::June, 8);
        MonthFilter filterA(Month::June);
        MonthFilter filterB(Month::May);
        MonthFilter filterC(Month::September);

        std::cout << "dt = " << dt << std::endl;

        auto rangeA = filterA.next_range(dt);
        auto rangeB = filterB.next_range(dt);
        auto rangeC = filterC.next_range(dt);

        assert_true(rangeA.has_value());
        std::cout << "rangeA = " << *rangeA << std::endl;
        assert_equal(*rangeA, Range(Datetime(1989, Month::June, 1),
                                    Datetime(1989, Month::July, 1)));

        assert_true(rangeB.has_value());
        std::cout << "rangeB = " << *rangeB << std::endl;
        assert_equal(*rangeB, Range(Datetime(1989, Month::May, 1),
                                    Datetime(1989, Month::June, 1)));

        assert_true(rangeC.has_value());
        std::cout << "rangeC = " << *rangeC << std::endl;
        assert_equal(*rangeC, Range(Datetime(1988, Month::September, 1),
                                    Datetime(1988, Month::October, 1)));
    })
    .test("next_range() on month boundaries", [&]() {
        Datetime dtA (1988, Month::June, 1);
        Datetime dtB (1988, Month::June, 30);
        Datetime dtC (1988, Month::February, 29);
        Datetime dtD (1987, Month::February, 28);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;
        std::cout << "dtC = " << dtC << std::endl;
        std::cout << "dtD = " << dtD << std::endl;

        MonthFilter filterA(Month::June);
        MonthFilter filterB(Month::February);

        auto rangeAA = filterA.next_range(dtA);
        auto rangeAB = filterA.next_range(dtB);
        auto rangeAC = filterA.next_range(dtC);
        auto rangeAD = filterA.next_range(dtD);
        auto rangeBA = filterB.next_range(dtA);
        auto rangeBB = filterB.next_range(dtB);
        auto rangeBC = filterB.next_range(dtC);
        auto rangeBD = filterB.next_range(dtD);

        assert_true(rangeAA.has_value());
        std::cout << "rangeAA = " << *rangeAA << std::endl;
        assert_equal(*rangeAA, Range(Datetime(1989, Month::June, 1),
                                     Datetime(1989, Month::July, 1)));

        assert_true(rangeAB.has_value());
        std::cout << "rangeAB = " << *rangeAB << std::endl;
        assert_equal(*rangeAB, Range(Datetime(1989, Month::June, 1),
                                     Datetime(1989, Month::July, 1)));

        assert_true(rangeAC.has_value());
        std::cout << "rangeAC = " << *rangeAC << std::endl;
        assert_equal(*rangeAC, Range(Datetime(1988, Month::June, 1),
                                     Datetime(1988, Month::July, 1)));

        assert_true(rangeAD.has_value());
        std::cout << "rangeAD = " << *rangeAD << std::endl;
        assert_equal(*rangeAD, Range(Datetime(1987, Month::June, 1),
                                     Datetime(1987, Month::July, 1)));

        assert_true(rangeBA.has_value());
        std::cout << "rangeBA = " << *rangeBA << std::endl;
        assert_equal(*rangeBA, Range(Datetime(1989, Month::February, 1),
                                     Datetime(1989, Month::March, 1)));

        assert_true(rangeBB.has_value());
        std::cout << "rangeBB = " << *rangeBB << std::endl;
        assert_equal(*rangeBB, Range(Datetime(1989, Month::February, 1),
                                     Datetime(1989, Month::March, 1)));

        assert_true(rangeBC.has_value());
        std::cout << "rangeBC = " << *rangeBC << std::endl;
        assert_equal(*rangeBC, Range(Datetime(1989, Month::February, 1),
                                     Datetime(1989, Month::March, 1)));

        assert_true(rangeBD.has_value());
        std::cout << "rangeBD = " << *rangeBD << std::endl;
        assert_equal(*rangeBD, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::March, 1)));
    })
    .test("prev_range() on month boundaries", [&]() {
        Datetime dtA (1988, Month::June, 1);
        Datetime dtB (1988, Month::June, 30);
        Datetime dtC (1988, Month::February, 29);
        Datetime dtD (1987, Month::February, 28);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;
        std::cout << "dtC = " << dtC << std::endl;
        std::cout << "dtD = " << dtD << std::endl;

        MonthFilter filterA(Month::June);
        MonthFilter filterB(Month::February);

        auto rangeAA = filterA.prev_range(dtA);
        auto rangeAB = filterA.prev_range(dtB);
        auto rangeAC = filterA.prev_range(dtC);
        auto rangeAD = filterA.prev_range(dtD);
        auto rangeBA = filterB.prev_range(dtA);
        auto rangeBB = filterB.prev_range(dtB);
        auto rangeBC = filterB.prev_range(dtC);
        auto rangeBD = filterB.prev_range(dtD);

        assert_true(rangeAA.has_value());
        std::cout << "rangeAA = " << *rangeAA << std::endl;
        assert_equal(*rangeAA, Range(Datetime(1988, Month::June, 1),
                                     Datetime(1988, Month::July, 1)));

        assert_true(rangeAB.has_value());
        std::cout << "rangeAB = " << *rangeAB << std::endl;
        assert_equal(*rangeAB, Range(Datetime(1988, Month::June, 1),
                                     Datetime(1988, Month::July, 1)));

        assert_true(rangeAC.has_value());
        std::cout << "rangeAC = " << *rangeAC << std::endl;
        assert_equal(*rangeAC, Range(Datetime(1987, Month::June, 1),
                                     Datetime(1987, Month::July, 1)));

        assert_true(rangeAD.has_value());
        std::cout << "rangeAD = " << *rangeAD << std::endl;
        assert_equal(*rangeAD, Range(Datetime(1986, Month::June, 1),
                                     Datetime(1986, Month::July, 1)));

        assert_true(rangeBA.has_value());
        std::cout << "rangeBA = " << *rangeBA << std::endl;
        assert_equal(*rangeBA, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::March, 1)));

        assert_true(rangeBB.has_value());
        std::cout << "rangeBB = " << *rangeBB << std::endl;
        assert_equal(*rangeBB, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::March, 1)));

        assert_true(rangeBC.has_value());
        std::cout << "rangeBC = " << *rangeBC << std::endl;
        assert_equal(*rangeBC, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::March, 1)));

        assert_true(rangeBD.has_value());
        std::cout << "rangeBD = " << *rangeBD << std::endl;
        assert_equal(*rangeBD, Range(Datetime(1987, Month::February, 1),
                                     Datetime(1987, Month::March, 1)));
    })
    .test("prev_range()", [&]() {
        Datetime dt (1988, Month::June, 8);
        MonthFilter filterA(Month::June);
        MonthFilter filterB(Month::May);
        MonthFilter filterC(Month::September);

        std::cout << "dt = " << dt << std::endl;

        auto rangeA = filterA.prev_range(dt);
        auto rangeB = filterB.prev_range(dt);
        auto rangeC = filterC.prev_range(dt);

        assert_true(rangeA.has_value());
        std::cout << "rangeA = " << *rangeA << std::endl;
        assert_equal(*rangeA, Range(Datetime(1988, Month::June, 1),
                                    Datetime(1988, Month::July, 1)));

        assert_true(rangeB.has_value());
        std::cout << "rangeB = " << *rangeB << std::endl;
        assert_equal(*rangeB, Range(Datetime(1988, Month::May, 1),
                                    Datetime(1988, Month::June, 1)));

        assert_true(rangeC.has_value());
        std::cout << "rangeC = " << *rangeC << std::endl;
        assert_equal(*rangeC, Range(Datetime(1987, Month::September, 1),
                                    Datetime(1987, Month::October, 1)));
    })
    .die_on_signal(SIGSEGV)
    .run();
}
