/*
 * offset_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Sunday September 5, 2021
 *
 * Distributed under terms of the MIT license.
 */

#include "moonlight/test.h"
#include "timefilter/offset_filter.h"
#include "timefilter/monthday_filter.h"
#include <csignal>
#include <iostream>

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter offset_filter tests")
    .die_on_signal(SIGSEGV)
    .test("next_range() single offseter for a month day", []() {
        auto filter = MonthdayFilter::create(13);
        auto offset_filter = OffsetFilter::create(filter, Duration::of_days(-1));
        auto pivot = Datetime(2077, Month::May, 1);

        auto next_occurrence = filter->next_range(pivot);
        auto next_offseter = offset_filter->next_range(pivot);

        assert_true(next_occurrence.has_value());
        std::cout << next_occurrence.value() << std::endl;
        assert_equal(next_occurrence.value(),
                     Range(Datetime(2077, Month::May, 13),
                           Datetime(2077, Month::May, 14)));

        assert_true(next_offseter.has_value());
        std::cout << next_offseter.value() << std::endl;
        assert_equal(next_offseter.value(),
                     Range(Datetime(2077, Month::May, 12),
                           Datetime(2077, Month::May, 13)));
    })
    .run();

}
