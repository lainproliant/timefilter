/*
 * parser_compiler.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday June 3, 2021
 *
 * Distributed under terms of the MIT license.
 */

#include "moonlight/test.h"
#include "timefilter/parser.h"
#include "timefilter/compiler.h"

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    Parser parser;
    Compiler compiler;

    auto compile = [&](const std::string& expr) -> Filter::Pointer {
        auto tokens = parser.parse(expr);
        return compiler.compile_filter(tokens);
    };

    return TestSuite("timefilter parser tests")
    .test("dmy_long", [&]() {
        auto filter = compile("8 June 1988");
        tfm::printfln("filter = %s", *filter);
        const Datetime pivot = Datetime(Date(2020, 1, 1));
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_FALSE(next_range.has_value());
        ASSERT_EQUAL(prev_range->start(), Datetime(1988, Month::June, 8));
        ASSERT_EQUAL(prev_range->end(), Datetime(1988, Month::June, 9));
    })
    .test("dmy_short", [&]() {
        auto filter = compile("8 Jun 1988");
        tfm::printfln("filter = %s", *filter);
        const Datetime pivot = Datetime(Date(2020, 1, 1));
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_FALSE(next_range.has_value());
        ASSERT_EQUAL(prev_range->start(), Datetime(1988, Month::June, 8));
        ASSERT_EQUAL(prev_range->end(), Datetime(1988, Month::June, 9));
    })
    .test("mdy_long", [&]() {
        auto filter = compile("June 8 1988");
        tfm::printfln("filter = %s", *filter);
        const Datetime pivot = Datetime(Date(2020, 1, 1));
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_FALSE(next_range.has_value());
        ASSERT_EQUAL(prev_range->start(), Datetime(1988, Month::June, 8));
        ASSERT_EQUAL(prev_range->end(), Datetime(1988, Month::June, 9));
    })
    .test("mdy_short", [&]() {
        auto filter = compile("Jun 8 1988");
        tfm::printfln("filter = %s", *filter);
        const Datetime pivot = Datetime(Date(2020, 1, 1));
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_FALSE(next_range.has_value());
        ASSERT_EQUAL(prev_range->start(), Datetime(1988, Month::June, 8));
        ASSERT_EQUAL(prev_range->end(), Datetime(1988, Month::June, 9));
    })
    .test("my_long", [&]() {
        auto filter = compile("September 2021");
        tfm::printfln("filter = %s", *filter);
        const Datetime pivot = Datetime(Date(2020, 1, 1));
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::September, 1));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::October, 1));
        ASSERT_FALSE(prev_range.has_value());
    })
    .test("my_short", [&]() {
        auto filter = compile("Sep 2021");
        tfm::printfln("filter = %s", *filter);
        const Datetime pivot = Datetime(Date(2020, 1, 1));
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::September, 1));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::October, 1));
        ASSERT_FALSE(prev_range.has_value());
    })
    .test("ym_long", [&]() {
        auto filter = compile("2021 September");
        tfm::printfln("filter = %s", *filter);
        const Datetime pivot = Datetime(Date(2020, 1, 1));
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::September, 1));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::October, 1));
        ASSERT_FALSE(prev_range.has_value());
    })
    .test("ym_short", [&]() {
        auto filter = compile("2021 Sep");
        tfm::printfln("filter = %s", *filter);
        const Datetime pivot = Datetime(Date(2020, 1, 1));
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::September, 1));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::October, 1));
        ASSERT_FALSE(prev_range.has_value());
    })
    .test("dm_long", [&]() {
        auto filter = compile("10 November");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::November, 10));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::November, 11));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::November, 10));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::November, 11));
    })
    .test("dm_short", [&]() {
        auto filter = compile("10 Nov");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::November, 10));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::November, 11));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::November, 10));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::November, 11));
    })
    .test("md_long", [&]() {
        auto filter = compile("November 10");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::November, 10));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::November, 11));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::November, 10));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::November, 11));
    })
    .test("md_short", [&]() {
        auto filter = compile("Nov 10");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::November, 10));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::November, 11));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::November, 10));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::November, 11));
    })
    .test("weekday_monthday_long", [&]() {
        auto filter = compile("Friday 13");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::August, 13));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::August, 14));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::November, 13));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::November, 14));
    })
    .test("weekday_monthday_short", [&]() {
        auto filter = compile("Fri 13");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::August, 13));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::August, 14));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::November, 13));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::November, 14));
    })
    .test("month_long", [&]() {
        auto filter = compile("October");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::October, 1));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::November, 1));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::October, 1));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::November, 1));
    })
    .test("month_short", [&]() {
        auto filter = compile("Oct");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::October, 1));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::November, 1));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::October, 1));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::November, 1));
    })
    .test("iso_date", [&]() {
        auto filter = compile("2021-06-03");
        tfm::printfln("filter = %s", *filter);
        const Datetime pivot = Datetime(Date(2020, 1, 1));
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::June, 3));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::June, 4));
        ASSERT_FALSE(prev_range.has_value());
    })
    .test("us_date", [&]() {
        auto filter = compile("6/3/2021");
        tfm::printfln("filter = %s", *filter);
        const Datetime pivot = Datetime(Date(2020, 1, 1));
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::June, 3));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::June, 4));
        ASSERT_FALSE(prev_range.has_value());
    })
    .test("mil_time", [&]() {
        auto filter = compile("2315h");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::January, 1, 23, 15));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::January, 1, 23, 16));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::December, 31, 23, 15));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::December, 31, 23, 16));
    })
    .test("h12_time am", [&]() {
        auto filter = compile("11:15 am");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::January, 1, 11, 15));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::January, 1, 11, 16));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::December, 31, 11, 15));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::December, 31, 11, 16));
    })
    .test("h12_time pm", [&]() {
        auto filter = compile("11:15 pm");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::January, 1, 23, 15));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::January, 1, 23, 16));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::December, 31, 23, 15));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::December, 31, 23, 16));
    })
    .test("h24_time", [&]() {
        auto filter = compile("13:15");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::January, 1, 13, 15));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::January, 1, 13, 16));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::December, 31, 13, 15));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::December, 31, 13, 16));
    })
    .test("weekdays_repeating", [&]() {
        auto filter = compile("TH 6:30pm");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2024, Month::February, 12);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2024, Month::February, 13, 18, 30));
        ASSERT_EQUAL(next_range->end(), Datetime(2024, Month::February, 13, 18, 31));
        ASSERT_EQUAL(prev_range->start(), Datetime(2024, Month::February, 8, 18, 30));
        ASSERT_EQUAL(prev_range->end(), Datetime(2024, Month::February, 8, 18, 31));
    })
    .test("weekday_repeating_range", [&]() {
        auto filter = compile("W 3:00pm - 6:00pm");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::January, 6, 15, 00));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::January, 6, 18, 00));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::December, 30, 15, 00));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::December, 30, 18, 00));
    })
    .test("weekday_repeating_range_in_months", [&]() {
        auto filter = compile("W 3:00pm - 6:00pm @ April October");
        tfm::printfln("filter = %s", *filter);
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        tfm::printfln("next_range = %s", *next_range);
        tfm::printfln("prev_range = %s", *prev_range);
        ASSERT_EQUAL(next_range->start(), Datetime(2021, Month::April, 7, 15, 00));
        ASSERT_EQUAL(next_range->end(), Datetime(2021, Month::April, 7, 18, 00));
        ASSERT_EQUAL(prev_range->start(), Datetime(2020, Month::October, 28, 15, 00));
        ASSERT_EQUAL(prev_range->end(), Datetime(2020, Month::October, 28, 18, 00));
    })
    .run();
}
