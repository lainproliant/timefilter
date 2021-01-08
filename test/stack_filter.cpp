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

        std::cout << "dtA = " << dtA << std::endl;
        std::cout << "dtB = " << dtB << std::endl;

        auto filterA = StackFilter()
            .push(MonthFilter::create(Month::December))
            .push(WeekdayFilter::create(Weekday::Monday, Weekday::Wednesday, Weekday::Friday));

        auto rangeAA = filterA.next_range(dtA);
        auto rangeAB = filterA.next_range(dtB);
        std::cout << "rangeAA = " << *rangeAA << std::endl;
        std::cout << "rangeAB = " << *rangeAB << std::endl;

    })
    .die_on_signal(SIGSEGV)
    .run();
}
