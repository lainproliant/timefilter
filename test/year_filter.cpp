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

        std::cout << "rangeA.has_value() = " << rangeA.has_value() << std::endl;
        assert_true(rangeA.has_value());

        std::cout << "rangeA = " << *rangeA << std::endl;
        assert_equal(*rangeA, Range(Datetime(1981, Month::January, 1),
                                    Datetime(1982, Month::January, 1)));

        std::cout << "rangeB.has_value() = " << rangeB.has_value() << std::endl;
        assert_false(rangeB.has_value());

        std::cout << "rangeC.has_value() = " << rangeC.has_value() << std::endl;
        assert_false(rangeC.has_value());
    })
    .test("next_range() on year boundaries", [&]() {
        Datetime dtA(1980, Month::January, 1);
        Datetime dtB(1980, Month::December, 31);
        YearFilter filterA(1981);
        YearFilter filterB(1980);
        YearFilter filterC(1979);

        auto rangeAA = filterA.next_range(dtA);
        auto rangeAB = filterA.next_range(dtB);
        auto rangeBA = filterB.next_range(dtA);
        auto rangeBB = filterB.next_range(dtB);
        auto rangeCA = filterC.next_range(dtA);
        auto rangeCB = filterC.next_range(dtB);

        std::cout << "rangeAA.has_value() = " << rangeAA.has_value() << std::endl;
        assert_true(rangeAA.has_value());

        std::cout << "rangeAA = " << *rangeAA << std::endl;
        assert_equal(*rangeAA, Range(Datetime(1981, Month::January, 1),
                                     Datetime(1982, Month::January, 1)));

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        assert_true(rangeAB.has_value());

        std::cout << "rangeAB = " << *rangeAB << std::endl;
        assert_equal(*rangeAB, Range(Datetime(1981, Month::January, 1),
                                     Datetime(1982, Month::January, 1)));

        std::cout << "rangeBA.has_value() = " << rangeBA.has_value() << std::endl;
        assert_false(rangeBA.has_value());

        std::cout << "rangeBB.has_value() = " << rangeBB.has_value() << std::endl;
        assert_false(rangeBB.has_value());

        std::cout << "rangeCA.has_value() = " << rangeCA.has_value() << std::endl;
        assert_false(rangeCA.has_value());

        std::cout << "rangeCB.has_value() = " << rangeCB.has_value() << std::endl;
        assert_false(rangeCB.has_value());
    })
    .test("prev_range()", [&]() {
        Datetime dt(1980, Month::April, 1);
        YearFilter filterA(1981);
        YearFilter filterB(1980);
        YearFilter filterC(1979);

        auto rangeA = filterA.prev_range(dt);
        auto rangeB = filterB.prev_range(dt);
        auto rangeC = filterC.prev_range(dt);

        std::cout << "rangeA.has_value() = " << rangeA.has_value() << std::endl;
        assert_false(rangeA.has_value());

        std::cout << "rangeB.has_value() = " << rangeB.has_value() << std::endl;
        assert_true(rangeB.has_value());

        std::cout << "rangeB = " << *rangeB << std::endl;
        assert_equal(*rangeB, Range(Datetime(1980, Month::January, 1),
                                    Datetime(1981, Month::January, 1)));

        std::cout << "rangeC.has_value() = " << rangeC.has_value() << std::endl;
        assert_true(rangeC.has_value());

        std::cout << "rangeC = " << *rangeC << std::endl;
        assert_equal(*rangeC, Range(Datetime(1979, Month::January, 1),
                                    Datetime(1980, Month::January, 1)));
    })
    .test("prev_range() on year boundaries", [&]() {
        Datetime dtA(1980, Month::January, 1);
        Datetime dtB(1980, Month::December, 31);
        YearFilter filterA(1981);
        YearFilter filterB(1980);
        YearFilter filterC(1979);

        auto rangeAA = filterA.prev_range(dtA);
        auto rangeAB = filterA.prev_range(dtB);
        auto rangeBA = filterB.prev_range(dtA);
        auto rangeBB = filterB.prev_range(dtB);
        auto rangeCA = filterC.prev_range(dtA);
        auto rangeCB = filterC.prev_range(dtB);

        std::cout << "rangeAA.has_value() = " << rangeAA.has_value() << std::endl;
        assert_false(rangeAA.has_value());

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        assert_false(rangeAB.has_value());

        std::cout << "rangeBA.has_value() = " << rangeBA.has_value() << std::endl;
        assert_true(rangeBA.has_value());

        std::cout << "rangeBA = " << *rangeBA << std::endl;
        assert_equal(*rangeBA, Range(Datetime(1980, Month::January, 1),
                                     Datetime(1981, Month::January, 1)));

        std::cout << "rangeBB.has_value() = " << rangeBB.has_value() << std::endl;
        assert_true(rangeBB.has_value());

        std::cout << "rangeBB = " << *rangeBB << std::endl;
        assert_equal(*rangeBB, Range(Datetime(1980, Month::January, 1),
                                     Datetime(1981, Month::January, 1)));

        std::cout << "rangeCA.has_value() = " << rangeCA.has_value() << std::endl;
        assert_true(rangeCA.has_value());

        std::cout << "rangeCA = " << *rangeCA << std::endl;
        assert_equal(*rangeCA, Range(Datetime(1979, Month::January, 1),
                                     Datetime(1980, Month::January, 1)));

        std::cout << "rangeCB.has_value() = " << rangeCB.has_value() << std::endl;
        assert_true(rangeCB.has_value());

        std::cout << "rangeCB = " << *rangeCB << std::endl;
        assert_equal(*rangeCB, Range(Datetime(1979, Month::January, 1),
                                     Datetime(1980, Month::January, 1)));
    })
    .die_on_signal(SIGSEGV)
    .run();
}
