/*
 * year_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Friday December 25, 2020
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
        ASSERT_TRUE(rangeA.has_value());

        std::cout << "rangeA = " << *rangeA << std::endl;
        ASSERT_EQUAL(*rangeA, Range(Datetime(1981, Month::January, 1),
                                    Datetime(1982, Month::January, 1)));

        std::cout << "rangeB.has_value() = " << rangeB.has_value() << std::endl;
        ASSERT_FALSE(rangeB.has_value());

        std::cout << "rangeC.has_value() = " << rangeC.has_value() << std::endl;
        ASSERT_FALSE(rangeC.has_value());
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
        ASSERT_TRUE(rangeAA.has_value());

        std::cout << "rangeAA = " << *rangeAA << std::endl;
        ASSERT_EQUAL(*rangeAA, Range(Datetime(1981, Month::January, 1),
                                     Datetime(1982, Month::January, 1)));

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        ASSERT_TRUE(rangeAB.has_value());

        std::cout << "rangeAB = " << *rangeAB << std::endl;
        ASSERT_EQUAL(*rangeAB, Range(Datetime(1981, Month::January, 1),
                                     Datetime(1982, Month::January, 1)));

        std::cout << "rangeBA.has_value() = " << rangeBA.has_value() << std::endl;
        ASSERT_FALSE(rangeBA.has_value());

        std::cout << "rangeBB.has_value() = " << rangeBB.has_value() << std::endl;
        ASSERT_FALSE(rangeBB.has_value());

        std::cout << "rangeCA.has_value() = " << rangeCA.has_value() << std::endl;
        ASSERT_FALSE(rangeCA.has_value());

        std::cout << "rangeCB.has_value() = " << rangeCB.has_value() << std::endl;
        ASSERT_FALSE(rangeCB.has_value());
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
        ASSERT_FALSE(rangeA.has_value());

        std::cout << "rangeB.has_value() = " << rangeB.has_value() << std::endl;
        ASSERT_TRUE(rangeB.has_value());

        std::cout << "rangeB = " << *rangeB << std::endl;
        ASSERT_EQUAL(*rangeB, Range(Datetime(1980, Month::January, 1),
                                    Datetime(1981, Month::January, 1)));

        std::cout << "rangeC.has_value() = " << rangeC.has_value() << std::endl;
        ASSERT_TRUE(rangeC.has_value());

        std::cout << "rangeC = " << *rangeC << std::endl;
        ASSERT_EQUAL(*rangeC, Range(Datetime(1979, Month::January, 1),
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
        ASSERT_FALSE(rangeAA.has_value());

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        ASSERT_FALSE(rangeAB.has_value());

        std::cout << "rangeBA.has_value() = " << rangeBA.has_value() << std::endl;
        ASSERT_TRUE(rangeBA.has_value());

        std::cout << "rangeBA = " << *rangeBA << std::endl;
        ASSERT_EQUAL(*rangeBA, Range(Datetime(1980, Month::January, 1),
                                     Datetime(1981, Month::January, 1)));

        std::cout << "rangeBB.has_value() = " << rangeBB.has_value() << std::endl;
        ASSERT_TRUE(rangeBB.has_value());

        std::cout << "rangeBB = " << *rangeBB << std::endl;
        ASSERT_EQUAL(*rangeBB, Range(Datetime(1980, Month::January, 1),
                                     Datetime(1981, Month::January, 1)));

        std::cout << "rangeCA.has_value() = " << rangeCA.has_value() << std::endl;
        ASSERT_TRUE(rangeCA.has_value());

        std::cout << "rangeCA = " << *rangeCA << std::endl;
        ASSERT_EQUAL(*rangeCA, Range(Datetime(1979, Month::January, 1),
                                     Datetime(1980, Month::January, 1)));

        std::cout << "rangeCB.has_value() = " << rangeCB.has_value() << std::endl;
        ASSERT_TRUE(rangeCB.has_value());

        std::cout << "rangeCB = " << *rangeCB << std::endl;
        ASSERT_EQUAL(*rangeCB, Range(Datetime(1979, Month::January, 1),
                                     Datetime(1980, Month::January, 1)));
    })
    .die_on_signal(SIGSEGV)
    .run();
}
