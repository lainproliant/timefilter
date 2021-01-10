/*
 * stack_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday January 6, 2021
 *
 * Distributed under terms of the MIT license.
 */
#include "moonlight/test.h"
#include "timefilter/stack_filter.h"
#include "timefilter/month_filter.h"
#include "timefilter/weekday_filter.h"
#include "timefilter/time_filter.h"
#include <csignal>
#include <iostream>

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter stack_filter tests")
    .test("next_range(): single stack entry", [&]() {
        Datetime dtA(2021, Month::January, 6);
        Datetime dtB(2020, Month::June, 8);
        Datetime dtC(2021, Month::December, 2, 12, 00);
        Datetime dtD(2021, Month::December, 1, 13, 00);

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;
        std::cout << "dtC = " << dtC << std::endl;
        std::cout << "dtD = " << dtD << std::endl;

        auto filterA = StackFilter()
            .push(MonthFilter::create(Month::December))
            .push(WeekdayFilter::create(Weekday::Monday, Weekday::Wednesday, Weekday::Friday))
            .push(TimeFilter::create(Time(12, 00)));

        auto rangeAA = filterA.next_range(dtA);
        auto rangeAB = filterA.next_range(dtB);
        auto rangeAC = filterA.next_range(dtC);
        auto rangeAD = filterA.next_range(dtD);
        std::cout << "rangeAA = " << *rangeAA << std::endl;
        std::cout << "rangeAB = " << *rangeAB << std::endl;
        std::cout << "rangeAC = " << *rangeAC << std::endl;
        std::cout << "rangeAD = " << *rangeAD << std::endl;

    })
    .die_on_signal(SIGSEGV)
    .run();
}
