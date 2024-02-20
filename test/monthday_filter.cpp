/*
 * monthday_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Tuesday December 29, 2020
 *
 * Distributed under terms of the MIT license.
 */

#include <csignal>
#include <iostream>
#include "moonlight/test.h"
#include "timefilter/monthday.h"

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter monthday_filter tests")
    .test("next_range()", [&]() {
        Datetime dtA(1988, Month::January, 1);
        Datetime dtB(1988, Month::May, 31);
        Datetime dtC(1988, Month::February, 1);
        Datetime dtD(1988, Month::February, 29);

        auto filterA = MonthdayFilter::create(1);
        auto filterB = MonthdayFilter::create(-2);
        auto filterC = MonthdayFilter::create(31);
        auto filterD = MonthdayFilter::create(29);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;

        auto rangeAA = filterA->next_range(dtA);
        auto rangeAB = filterA->next_range(dtB);
        auto rangeAC = filterA->next_range(dtC);
        auto rangeAD = filterA->next_range(dtD);
        auto rangeBA = filterB->next_range(dtA);
        auto rangeBB = filterB->next_range(dtB);
        auto rangeBC = filterB->next_range(dtC);
        auto rangeBD = filterB->next_range(dtD);
        auto rangeCA = filterC->next_range(dtA);
        auto rangeCB = filterC->next_range(dtB);
        auto rangeCC = filterC->next_range(dtC);
        auto rangeCD = filterC->next_range(dtD);
        auto rangeDA = filterD->next_range(dtA);
        auto rangeDB = filterD->next_range(dtB);
        auto rangeDC = filterD->next_range(dtC);
        auto rangeDD = filterD->next_range(dtD);

        std::cout << "rangeAA.has_value() = " << rangeAA.has_value() << std::endl;
        ASSERT_TRUE(rangeAA.has_value());

        std::cout << "rangeAA = " << *rangeAA << std::endl;
        ASSERT_EQUAL(*rangeAA, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::February, 2)));

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        ASSERT_TRUE(rangeAB.has_value());

        std::cout << "rangeAB = " << *rangeAB << std::endl;
        ASSERT_EQUAL(*rangeAB, Range(Datetime(1988, Month::June, 1),
                                     Datetime(1988, Month::June, 2)));

        std::cout << "rangeAC.has_value() = " << rangeAC.has_value() << std::endl;
        ASSERT_TRUE(rangeAC.has_value());

        std::cout << "rangeAC = " << *rangeAC << std::endl;
        ASSERT_EQUAL(*rangeAC, Range(Datetime(1988, Month::March, 1),
                                     Datetime(1988, Month::March, 2)));

        std::cout << "rangeAD.has_value() = " << rangeAD.has_value() << std::endl;
        ASSERT_TRUE(rangeAD.has_value());

        std::cout << "rangeAD = " << *rangeAD << std::endl;
        ASSERT_EQUAL(*rangeAD, Range(Datetime(1988, Month::March, 1),
                                     Datetime(1988, Month::March, 2)));

        std::cout << "rangeBA.has_value() = " << rangeBA.has_value() << std::endl;
        ASSERT_TRUE(rangeBA.has_value());

        std::cout << "rangeBA = " << *rangeBA << std::endl;
        ASSERT_EQUAL(*rangeBA, Range(Datetime(1988, Month::January, 30),
                                     Datetime(1988, Month::January, 31)));

        std::cout << "rangeBB.has_value() = " << rangeBB.has_value() << std::endl;
        ASSERT_TRUE(rangeBB.has_value());

        std::cout << "rangeBB = " << *rangeBB << std::endl;
        ASSERT_EQUAL(*rangeBB, Range(Datetime(1988, Month::June, 29),
                                     Datetime(1988, Month::June, 30)));

        std::cout << "rangeBC.has_value() = " << rangeBC.has_value() << std::endl;
        ASSERT_TRUE(rangeBC.has_value());

        std::cout << "rangeBC = " << *rangeBC << std::endl;
        ASSERT_EQUAL(*rangeBC, Range(Datetime(1988, Month::February, 28),
                                     Datetime(1988, Month::February, 29)));

        std::cout << "rangeBD.has_value() = " << rangeBD.has_value() << std::endl;
        ASSERT_TRUE(rangeBD.has_value());

        std::cout << "rangeBD = " << *rangeBD << std::endl;
        ASSERT_EQUAL(*rangeBD, Range(Datetime(1988, Month::March, 30),
                                     Datetime(1988, Month::March, 31)));

        std::cout << "rangeCA.has_value() = " << rangeCA.has_value() << std::endl;
        ASSERT_TRUE(rangeCA.has_value());

        std::cout << "rangeCA = " << *rangeCA << std::endl;
        ASSERT_EQUAL(*rangeCA, Range(Datetime(1988, Month::January, 31),
                                     Datetime(1988, Month::February, 1)));

        std::cout << "rangeCB.has_value() = " << rangeCB.has_value() << std::endl;
        ASSERT_TRUE(rangeCB.has_value());

        std::cout << "rangeCB = " << *rangeCB << std::endl;
        ASSERT_EQUAL(*rangeCB, Range(Datetime(1988, Month::July, 31),
                                     Datetime(1988, Month::August, 1)));

        std::cout << "rangeCC.has_value() = " << rangeCC.has_value() << std::endl;
        ASSERT_TRUE(rangeCC.has_value());

        std::cout << "rangeCC = " << *rangeCC << std::endl;
        ASSERT_EQUAL(*rangeCC, Range(Datetime(1988, Month::March, 31),
                                     Datetime(1988, Month::April, 1)));

        std::cout << "rangeCD.has_value() = " << rangeCD.has_value() << std::endl;
        ASSERT_TRUE(rangeCD.has_value());

        std::cout << "rangeCD = " << *rangeCD << std::endl;
        ASSERT_EQUAL(*rangeCD, Range(Datetime(1988, Month::March, 31),
                                     Datetime(1988, Month::April, 1)));

        std::cout << "rangeDA.has_value() = " << rangeDA.has_value() << std::endl;
        ASSERT_TRUE(rangeDA.has_value());

        std::cout << "rangeDA = " << *rangeDA << std::endl;
        ASSERT_EQUAL(*rangeDA, Range(Datetime(1988, Month::January, 29),
                                     Datetime(1988, Month::January, 30)));

        std::cout << "rangeDB.has_value() = " << rangeDB.has_value() << std::endl;
        ASSERT_TRUE(rangeDB.has_value());

        std::cout << "rangeDB = " << *rangeDB << std::endl;
        ASSERT_EQUAL(*rangeDB, Range(Datetime(1988, Month::June, 29),
                                     Datetime(1988, Month::June, 30)));

        std::cout << "rangeDC.has_value() = " << rangeDC.has_value() << std::endl;
        ASSERT_TRUE(rangeDC.has_value());

        std::cout << "rangeDC = " << *rangeDC << std::endl;
        ASSERT_EQUAL(*rangeDC, Range(Datetime(1988, Month::February, 29),
                                     Datetime(1988, Month::March, 1)));

        std::cout << "rangeDD.has_value() = " << rangeDD.has_value() << std::endl;
        ASSERT_TRUE(rangeDD.has_value());

        std::cout << "rangeDD = " << *rangeDD << std::endl;
        ASSERT_EQUAL(*rangeDD, Range(Datetime(1988, Month::March, 29),
                                     Datetime(1988, Month::March, 30)));
    })
    .test("prev_range()", [&]() {
        Datetime dtA(1988, Month::January, 1);
        Datetime dtB(1988, Month::May, 31);
        Datetime dtC(1988, Month::February, 1);
        Datetime dtD(1988, Month::February, 29);

        auto filterA = MonthdayFilter::create(1);
        auto filterB = MonthdayFilter::create(-2);
        auto filterC = MonthdayFilter::create(31);
        auto filterD = MonthdayFilter::create(29);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;

        auto rangeAA = filterA->prev_range(dtA);
        auto rangeAB = filterA->prev_range(dtB);
        auto rangeAC = filterA->prev_range(dtC);
        auto rangeAD = filterA->prev_range(dtD);
        auto rangeBA = filterB->prev_range(dtA);
        auto rangeBB = filterB->prev_range(dtB);
        auto rangeBC = filterB->prev_range(dtC);
        auto rangeBD = filterB->prev_range(dtD);
        auto rangeCA = filterC->prev_range(dtA);
        auto rangeCB = filterC->prev_range(dtB);
        auto rangeCC = filterC->prev_range(dtC);
        auto rangeCD = filterC->prev_range(dtD);
        auto rangeDA = filterD->prev_range(dtA);
        auto rangeDB = filterD->prev_range(dtB);
        auto rangeDC = filterD->prev_range(dtC);
        auto rangeDD = filterD->prev_range(dtD);

        std::cout << "rangeAA.has_value() = " << rangeAA.has_value() << std::endl;
        ASSERT_TRUE(rangeAA.has_value());

        std::cout << "rangeAA = " << *rangeAA << std::endl;
        ASSERT_EQUAL(*rangeAA, Range(Datetime(1988, Month::January, 1),
                                     Datetime(1988, Month::January, 2)));

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        ASSERT_TRUE(rangeAB.has_value());

        std::cout << "rangeAB = " << *rangeAB << std::endl;
        ASSERT_EQUAL(*rangeAB, Range(Datetime(1988, Month::May, 1),
                                     Datetime(1988, Month::May, 2)));

        std::cout << "rangeAC.has_value() = " << rangeAC.has_value() << std::endl;
        ASSERT_TRUE(rangeAC.has_value());

        std::cout << "rangeAC = " << *rangeAC << std::endl;
        ASSERT_EQUAL(*rangeAC, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::February, 2)));

        std::cout << "rangeAD.has_value() = " << rangeAD.has_value() << std::endl;
        ASSERT_TRUE(rangeAD.has_value());

        std::cout << "rangeAD = " << *rangeAD << std::endl;
        ASSERT_EQUAL(*rangeAD, Range(Datetime(1988, Month::February, 1),
                                     Datetime(1988, Month::February, 2)));

        std::cout << "rangeBA.has_value() = " << rangeBA.has_value() << std::endl;
        ASSERT_TRUE(rangeBA.has_value());

        std::cout << "rangeBA = " << *rangeBA << std::endl;
        ASSERT_EQUAL(*rangeBA, Range(Datetime(1987, Month::December, 30),
                                     Datetime(1987, Month::December, 31)));

        std::cout << "rangeBB.has_value() = " << rangeBB.has_value() << std::endl;
        ASSERT_TRUE(rangeBB.has_value());

        std::cout << "rangeBB = " << *rangeBB << std::endl;
        ASSERT_EQUAL(*rangeBB, Range(Datetime(1988, Month::May, 30),
                                     Datetime(1988, Month::May, 31)));

        std::cout << "rangeBC.has_value() = " << rangeBC.has_value() << std::endl;
        ASSERT_TRUE(rangeBC.has_value());

        std::cout << "rangeBC = " << *rangeBC << std::endl;
        ASSERT_EQUAL(*rangeBC, Range(Datetime(1988, Month::January, 30),
                                     Datetime(1988, Month::January, 31)));

        std::cout << "rangeBD.has_value() = " << rangeBD.has_value() << std::endl;
        ASSERT_TRUE(rangeBD.has_value());

        std::cout << "rangeBD = " << *rangeBD << std::endl;
        ASSERT_EQUAL(*rangeBD, Range(Datetime(1988, Month::February, 28),
                                     Datetime(1988, Month::February, 29)));

        std::cout << "rangeCA.has_value() = " << rangeCA.has_value() << std::endl;
        ASSERT_TRUE(rangeCA.has_value());

        std::cout << "rangeCA = " << *rangeCA << std::endl;
        ASSERT_EQUAL(*rangeCA, Range(Datetime(1987, Month::December, 31),
                                     Datetime(1988, Month::January, 1)));

        std::cout << "rangeCB.has_value() = " << rangeCB.has_value() << std::endl;
        ASSERT_TRUE(rangeCB.has_value());

        std::cout << "rangeCB = " << *rangeCB << std::endl;
        ASSERT_EQUAL(*rangeCB, Range(Datetime(1988, Month::May, 31),
                                     Datetime(1988, Month::June, 1)));

        std::cout << "rangeCC.has_value() = " << rangeCC.has_value() << std::endl;
        ASSERT_TRUE(rangeCC.has_value());

        std::cout << "rangeCC = " << *rangeCC << std::endl;
        ASSERT_EQUAL(*rangeCC, Range(Datetime(1988, Month::January, 31),
                                     Datetime(1988, Month::February, 1)));

        std::cout << "rangeCD.has_value() = " << rangeCD.has_value() << std::endl;
        ASSERT_TRUE(rangeCD.has_value());

        std::cout << "rangeCD = " << *rangeCD << std::endl;
        ASSERT_EQUAL(*rangeCD, Range(Datetime(1988, Month::January, 31),
                                     Datetime(1988, Month::February, 1)));

        std::cout << "rangeDA.has_value() = " << rangeDA.has_value() << std::endl;
        ASSERT_TRUE(rangeDA.has_value());

        std::cout << "rangeDA = " << *rangeDA << std::endl;
        ASSERT_EQUAL(*rangeDA, Range(Datetime(1987, Month::December, 29),
                                     Datetime(1987, Month::December, 30)));

        std::cout << "rangeDB.has_value() = " << rangeDB.has_value() << std::endl;
        ASSERT_TRUE(rangeDB.has_value());

        std::cout << "rangeDB = " << *rangeDB << std::endl;
        ASSERT_EQUAL(*rangeDB, Range(Datetime(1988, Month::May, 29),
                                     Datetime(1988, Month::May, 30)));

        std::cout << "rangeDC.has_value() = " << rangeDC.has_value() << std::endl;
        ASSERT_TRUE(rangeDC.has_value());

        std::cout << "rangeDC = " << *rangeDC << std::endl;
        ASSERT_EQUAL(*rangeDC, Range(Datetime(1988, Month::January, 29),
                                     Datetime(1988, Month::January, 30)));

        std::cout << "rangeDD.has_value() = " << rangeDD.has_value() << std::endl;
        ASSERT_TRUE(rangeDD.has_value());

        std::cout << "rangeDD = " << *rangeDD << std::endl;
        ASSERT_EQUAL(*rangeDD, Range(Datetime(1988, Month::February, 29),
                                     Datetime(1988, Month::March, 1)));
    })
    .die_on_signal(SIGSEGV)
    .run();
}
