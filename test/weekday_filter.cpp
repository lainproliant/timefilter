/*
 * weekday_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday December 30, 2020
 *
 * Distributed under terms of the MIT license.
 */

#include <csignal>
#include <iostream>
#include "moonlight/test.h"
#include "timefilter/weekday.h"

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter weekday_filter tests")
    .test("next_range() for single weekday", [&]() {
        Datetime dtA(1988, Month::June, 8);
        Datetime dtB(2000, Month::January, 1);
        Datetime dtC(2020, Month::December, 31);

        auto filterA = WeekdayFilter::create(Weekday::Monday);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtA weekday = " << dtA.format("%A") << std::endl;
        ASSERT_EQUAL(dtA.date().weekday(), Weekday::Wednesday);

        std::cout << "dtB = " << dtB << std::endl;
        std::cout << "dtB weekday = " << dtB.format("%A") << std::endl;
        ASSERT_EQUAL(dtB.date().weekday(), Weekday::Saturday);

        std::cout << "dtC = " << dtC << std::endl;
        std::cout << "dtC weekday = " << dtC.format("%A") << std::endl;
        ASSERT_EQUAL(dtC.date().weekday(), Weekday::Thursday);

        auto rangeAA = filterA->next_range(dtA);
        auto rangeAB = filterA->next_range(dtB);
        auto rangeAC = filterA->next_range(dtC);

        std::cout << "rangeAA.has_value() = " << rangeAA.has_value() << std::endl;
        ASSERT_TRUE(rangeAA.has_value());

        std::cout << "rangeAA = " << *rangeAA << std::endl;
        ASSERT_EQUAL(*rangeAA, Range(Datetime(1988, Month::June, 13),
                                     Datetime(1988, Month::June, 14)));

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        ASSERT_TRUE(rangeAB.has_value());

        std::cout << "rangeAB = " << *rangeAB << std::endl;
        ASSERT_EQUAL(*rangeAB, Range(Datetime(2000, Month::January, 3),
                                     Datetime(2000, Month::January, 4)));

        std::cout << "rangeAC.has_value() = " << rangeAC.has_value() << std::endl;
        ASSERT_TRUE(rangeAC.has_value());

        std::cout << "rangeAC = " << *rangeAC << std::endl;
        ASSERT_EQUAL(*rangeAC, Range(Datetime(2021, Month::January, 4),
                                     Datetime(2021, Month::January, 5)));
    })
    .test("prev_range() for single weekday", [&]() {
        Datetime dtA(1988, Month::June, 8);
        Datetime dtB(2000, Month::January, 1);
        Datetime dtC(2020, Month::December, 31);

        auto filterA = WeekdayFilter::create(Weekday::Monday);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtA weekday = " << dtA.format("%A") << std::endl;
        ASSERT_EQUAL(dtA.date().weekday(), Weekday::Wednesday);

        std::cout << "dtB = " << dtB << std::endl;
        std::cout << "dtB weekday = " << dtB.format("%A") << std::endl;
        ASSERT_EQUAL(dtB.date().weekday(), Weekday::Saturday);

        std::cout << "dtC = " << dtC << std::endl;
        std::cout << "dtC weekday = " << dtC.format("%A") << std::endl;
        ASSERT_EQUAL(dtC.date().weekday(), Weekday::Thursday);

        auto rangeAA = filterA->prev_range(dtA);
        auto rangeAB = filterA->prev_range(dtB);
        auto rangeAC = filterA->prev_range(dtC);

        std::cout << "rangeAA.has_value() = " << rangeAA.has_value() << std::endl;
        ASSERT_TRUE(rangeAA.has_value());

        std::cout << "rangeAA = " << *rangeAA << std::endl;
        ASSERT_EQUAL(*rangeAA, Range(Datetime(1988, Month::June, 6),
                                     Datetime(1988, Month::June, 7)));

        std::cout << "rangeAB.has_value() = " << rangeAB.has_value() << std::endl;
        ASSERT_TRUE(rangeAB.has_value());

        std::cout << "rangeAB = " << *rangeAB << std::endl;
        ASSERT_EQUAL(*rangeAB, Range(Datetime(1999, Month::December, 27),
                                     Datetime(1999, Month::December, 28)));

        std::cout << "rangeAC.has_value() = " << rangeAC.has_value() << std::endl;
        ASSERT_TRUE(rangeAC.has_value());

        std::cout << "rangeAC = " << *rangeAC << std::endl;
        ASSERT_EQUAL(*rangeAC, Range(Datetime(2020, Month::December, 28),
                                     Datetime(2020, Month::December, 29)));
    })
    .test("next_range() for multiple weekdays", [&]() {
        Datetime dt (2021, Month::January, 1);
        auto filter = WeekdayFilter::create(std::set{Weekday::Monday, Weekday::Tuesday, Weekday::Friday});

        std::cout << "dt = " << dt << std::endl;

        auto rangeA = filter->next_range(dt);
        std::cout << "rangeA.has_value() = " << rangeA.has_value() << std::endl;
        ASSERT_TRUE(rangeA.has_value());

        std::cout << "rangeA = " << *rangeA << std::endl;
        ASSERT_EQUAL(*rangeA, Range(Datetime(2021, Month::January, 4),
                                    Datetime(2021, Month::January, 5)));

        auto rangeB = filter->next_range(rangeA->start());
        std::cout << "rangeB.has_value() = " << rangeB.has_value() << std::endl;
        ASSERT_TRUE(rangeB.has_value());

        std::cout << "rangeB = " << *rangeB << std::endl;
        ASSERT_EQUAL(*rangeB, Range(Datetime(2021, Month::January, 5),
                                    Datetime(2021, Month::January, 6)));

        auto rangeC = filter->next_range(rangeB->start());
        std::cout << "rangeC.has_value() = " << rangeC.has_value() << std::endl;
        ASSERT_TRUE(rangeC.has_value());

        std::cout << "rangeC = " << *rangeC << std::endl;
        ASSERT_EQUAL(*rangeC, Range(Datetime(2021, Month::January, 8),
                                    Datetime(2021, Month::January, 9)));

        auto rangeD = filter->next_range(rangeC->start());
        std::cout << "rangeD.has_value() = " << rangeD.has_value() << std::endl;
        ASSERT_TRUE(rangeD.has_value());

        std::cout << "rangeD = " << *rangeD << std::endl;
        ASSERT_EQUAL(*rangeD, Range(Datetime(2021, Month::January, 11),
                                    Datetime(2021, Month::January, 12)));
    })
    .test("prev_range() for multiple weekdays", [&]() {
        Datetime dt (2021, Month::January, 1);
        auto filter = WeekdayFilter::create(std::set{Weekday::Monday, Weekday::Tuesday, Weekday::Friday});

        std::cout << "dt = " << dt << std::endl;

        auto rangeA = filter->prev_range(dt);
        std::cout << "rangeA.has_value() = " << rangeA.has_value() << std::endl;
        ASSERT_TRUE(rangeA.has_value());

        std::cout << "rangeA = " << *rangeA << std::endl;
        ASSERT_EQUAL(*rangeA, Range(Datetime(2021, Month::January, 1),
                                    Datetime(2021, Month::January, 2)));

        auto rangeB = filter->prev_range(rangeA->start() - Duration::of_days(1));
        std::cout << "rangeB.has_value() = " << rangeB.has_value() << std::endl;
        ASSERT_TRUE(rangeB.has_value());

        std::cout << "rangeB = " << *rangeB << std::endl;
        ASSERT_EQUAL(*rangeB, Range(Datetime(2020, Month::December, 29),
                                    Datetime(2020, Month::December, 30)));

        auto rangeC = filter->prev_range(rangeB->start() - Duration::of_days(1));
        std::cout << "rangeC.has_value() = " << rangeC.has_value() << std::endl;
        ASSERT_TRUE(rangeC.has_value());

        std::cout << "rangeC = " << *rangeC << std::endl;
        ASSERT_EQUAL(*rangeC, Range(Datetime(2020, Month::December, 28),
                                    Datetime(2020, Month::December, 29)));

        auto rangeD = filter->prev_range(rangeC->start() - Duration::of_days(1));
        std::cout << "rangeD.has_value() = " << rangeD.has_value() << std::endl;
        ASSERT_TRUE(rangeD.has_value());

        std::cout << "rangeD = " << *rangeD << std::endl;
        ASSERT_EQUAL(*rangeD, Range(Datetime(2020, Month::December, 25),
                                    Datetime(2020, Month::December, 26)));
    })
    .die_on_signal(SIGSEGV)
    .run();
}
