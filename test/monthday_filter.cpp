/*
 * monthday_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Tuesday December 29, 2020
 *
 * Distributed under terms of the MIT license.
 */

#include "moonlight/test.h"
#include "timefilter/monthday_filter.h"
#include <csignal>
#include <iostream>

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter monthday_filter tests")
    .test("next_range()", [&]() {
        Datetime dtA (1988, Month::January, 1);
        Datetime dtB (1988, Month::May, 31);
        Datetime dtC (1988, Month::February, 1);
        Datetime dtD (1988, Month::February, 29);

        MonthdayFilter filterA(1);
        MonthdayFilter filterB(-2);
        MonthdayFilter filterC(31);
        MonthdayFilter filterD(29);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;

        auto rangeAA = filterA.next_range(dtA);
        auto rangeAB = filterA.next_range(dtB);
        auto rangeAC = filterA.next_range(dtC);
        auto rangeAD = filterA.next_range(dtD);
        auto rangeBA = filterB.next_range(dtA);
        auto rangeBB = filterB.next_range(dtB);
        auto rangeBC = filterB.next_range(dtC);
        auto rangeBD = filterB.next_range(dtD);
        auto rangeCA = filterC.next_range(dtA);
        auto rangeCB = filterC.next_range(dtB);
        auto rangeCC = filterC.next_range(dtC);
        auto rangeCD = filterC.next_range(dtD);
        auto rangeDA = filterD.next_range(dtA);
        auto rangeDB = filterD.next_range(dtB);
        auto rangeDC = filterD.next_range(dtC);
        auto rangeDD = filterD.next_range(dtD);

        std::cout << "rangeAA.has_value() = " << rangeAA.has_value() << std::endl;
        assert_true(rangeAA.has_value());

        std::cout << "rangeAA = " << *rangeAA << std::endl;
        assert_equal(*rangeAA, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::February, 2)));

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        assert_true(rangeAB.has_value());

        std::cout << "rangeAB = " << *rangeAB << std::endl;
        assert_equal(*rangeAB, Range(Datetime(1988, Month::June, 1),
                                     Datetime(1988, Month::June, 2)));

        std::cout << "rangeAC.has_value() = " << rangeAC.has_value() << std::endl;
        assert_true(rangeAC.has_value());

        std::cout << "rangeAC = " << *rangeAC << std::endl;
        assert_equal(*rangeAC, Range(Datetime(1988, Month::March, 1),
                                     Datetime(1988, Month::March, 2)));

        std::cout << "rangeAD.has_value() = " << rangeAD.has_value() << std::endl;
        assert_true(rangeAD.has_value());

        std::cout << "rangeAD = " << *rangeAD << std::endl;
        assert_equal(*rangeAD, Range(Datetime(1988, Month::March, 1),
                                     Datetime(1988, Month::March, 2)));

        std::cout << "rangeBA.has_value() = " << rangeBA.has_value() << std::endl;
        assert_true(rangeBA.has_value());

        std::cout << "rangeBA = " << *rangeBA << std::endl;
        assert_equal(*rangeBA, Range(Datetime(1988, Month::January, 29),
                                     Datetime(1988, Month::January, 30)));

        std::cout << "rangeBB.has_value() = " << rangeBB.has_value() << std::endl;
        assert_true(rangeBB.has_value());

        std::cout << "rangeBB = " << *rangeBB << std::endl;
        assert_equal(*rangeBB, Range(Datetime(1988, Month::June, 28),
                                     Datetime(1988, Month::June, 29)));

        std::cout << "rangeBC.has_value() = " << rangeBC.has_value() << std::endl;
        assert_true(rangeBC.has_value());

        std::cout << "rangeBC = " << *rangeBC << std::endl;
        assert_equal(*rangeBC, Range(Datetime(1988, Month::February, 27),
                                     Datetime(1988, Month::February, 28)));

        std::cout << "rangeBD.has_value() = " << rangeBD.has_value() << std::endl;
        assert_true(rangeBD.has_value());

        std::cout << "rangeBD = " << *rangeBD << std::endl;
        assert_equal(*rangeBD, Range(Datetime(1988, Month::March, 29),
                                     Datetime(1988, Month::March, 30)));
    })
    .die_on_signal(SIGSEGV)
    .run();
}
