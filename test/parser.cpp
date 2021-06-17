/*
 * parser.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday June 3, 2021
 *
 * Distributed under terms of the MIT license.
 */

#include "moonlight/test.h"
#include "timefilter/parser.h"

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    auto lex = lex::Lexer().throw_on_scan_failure(false);
    timefilter::I18nStrings i18n;
    auto grammar = timefilter::make_grammar(i18n);
    auto compiler = timefilter::Compiler(i18n, timefilter::make_factories());

    auto compile = [&](const std::string& expr) -> Filter::Pointer {
        auto tokens = lex.lex(grammar, expr);
        return compiler.compile(tokens);
    };

    return TestSuite("timefilter parser tests")
    .test("dmy_long", [&]() {
        auto filter = compile("8 June 1988");
        assert_true(filter->is_absolute());
        assert_equal(filter->absolute_range()->start(), Datetime(1988, Month::June, 8));
        assert_equal(filter->absolute_range()->end(), Datetime(1988, Month::June, 9));
    })
    .test("dmy_short", [&]() {
        auto filter = compile("8 Jun 1988");
        assert_true(filter->is_absolute());
        assert_equal(filter->absolute_range()->start(), Datetime(1988, Month::June, 8));
        assert_equal(filter->absolute_range()->end(), Datetime(1988, Month::June, 9));
    })
    .test("mdy_long", [&]() {
        auto filter = compile("June 8 1988");
        assert_true(filter->is_absolute());
        assert_equal(filter->absolute_range()->start(), Datetime(1988, Month::June, 8));
        assert_equal(filter->absolute_range()->end(), Datetime(1988, Month::June, 9));
    })
    .test("mdy_short", [&]() {
        auto filter = compile("Jun 8 1988");
        assert_true(filter->is_absolute());
        assert_equal(filter->absolute_range()->start(), Datetime(1988, Month::June, 8));
        assert_equal(filter->absolute_range()->end(), Datetime(1988, Month::June, 9));
    })
    .test("my_long", [&]() {
        auto filter = compile("September 2021");
        assert_true(filter->is_absolute());
        assert_equal(filter->absolute_range()->start(), Datetime(2021, Month::September, 1));
        assert_equal(filter->absolute_range()->end(), Datetime(2021, Month::October, 1));
    })
    .test("my_short", [&]() {
        auto filter = compile("Sep 2021");
        assert_true(filter->is_absolute());
        assert_equal(filter->absolute_range()->start(), Datetime(2021, Month::September, 1));
        assert_equal(filter->absolute_range()->end(), Datetime(2021, Month::October, 1));
    })
    .test("ym_long", [&]() {
        auto filter = compile("2021 September");
        assert_true(filter->is_absolute());
        assert_equal(filter->absolute_range()->start(), Datetime(2021, Month::September, 1));
        assert_equal(filter->absolute_range()->end(), Datetime(2021, Month::October, 1));
    })
    .test("ym_short", [&]() {
        auto filter = compile("2021 Sep");
        assert_true(filter->is_absolute());
        assert_equal(filter->absolute_range()->start(), Datetime(2021, Month::September, 1));
        assert_equal(filter->absolute_range()->end(), Datetime(2021, Month::October, 1));
    })
    .test("dm_long", [&]() {
        auto filter = compile("10 November");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::November, 10));
        assert_equal(next_range->end(), Datetime(2021, Month::November, 11));
        assert_equal(prev_range->start(), Datetime(2020, Month::November, 10));
        assert_equal(prev_range->end(), Datetime(2020, Month::November, 11));
    })
    .test("dm_short", [&]() {
        auto filter = compile("10 Nov");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::November, 10));
        assert_equal(next_range->end(), Datetime(2021, Month::November, 11));
        assert_equal(prev_range->start(), Datetime(2020, Month::November, 10));
        assert_equal(prev_range->end(), Datetime(2020, Month::November, 11));
    })
    .test("md_long", [&]() {
        auto filter = compile("November 10");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::November, 10));
        assert_equal(next_range->end(), Datetime(2021, Month::November, 11));
        assert_equal(prev_range->start(), Datetime(2020, Month::November, 10));
        assert_equal(prev_range->end(), Datetime(2020, Month::November, 11));
    })
    .test("md_short", [&]() {
        auto filter = compile("Nov 10");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::November, 10));
        assert_equal(next_range->end(), Datetime(2021, Month::November, 11));
        assert_equal(prev_range->start(), Datetime(2020, Month::November, 10));
        assert_equal(prev_range->end(), Datetime(2020, Month::November, 11));
    })
    .test("weekday_monthday_long", [&]() {
        auto filter = compile("Friday 13");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::August, 13));
        assert_equal(next_range->end(), Datetime(2021, Month::August, 14));
        assert_equal(prev_range->start(), Datetime(2020, Month::November, 13));
        assert_equal(prev_range->end(), Datetime(2020, Month::November, 14));
    })
    .test("weekday_monthday_short", [&]() {
        auto filter = compile("Fri 13");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::August, 13));
        assert_equal(next_range->end(), Datetime(2021, Month::August, 14));
        assert_equal(prev_range->start(), Datetime(2020, Month::November, 13));
        assert_equal(prev_range->end(), Datetime(2020, Month::November, 14));
    })
    .test("month_long", [&]() {
        auto filter = compile("October");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::October, 1));
        assert_equal(next_range->end(), Datetime(2021, Month::November, 1));
        assert_equal(prev_range->start(), Datetime(2020, Month::October, 1));
        assert_equal(prev_range->end(), Datetime(2020, Month::November, 1));
    })
    .test("month_short", [&]() {
        auto filter = compile("Oct");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::October, 1));
        assert_equal(next_range->end(), Datetime(2021, Month::November, 1));
        assert_equal(prev_range->start(), Datetime(2020, Month::October, 1));
        assert_equal(prev_range->end(), Datetime(2020, Month::November, 1));
    })
    .test("iso_date", [&]() {
        auto filter = compile("2021-06-03");
        assert_true(filter->is_absolute());
        auto range = filter->absolute_range();
        assert_equal(range->start(), Datetime(2021, Month::June, 3));
        assert_equal(range->end(), Datetime(2021, Month::June, 4));
    })
    .test("us_date", [&]() {
        auto filter = compile("6/3/2021");
        assert_true(filter->is_absolute());
        auto range = filter->absolute_range();
        assert_equal(range->start(), Datetime(2021, Month::June, 3));
        assert_equal(range->end(), Datetime(2021, Month::June, 4));
    })
    .test("mil_time", [&]() {
        auto filter = compile("2315h");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::January, 1, 23, 15));
        assert_equal(next_range->end(), Datetime(2021, Month::January, 1, 23, 16));
        assert_equal(prev_range->start(), Datetime(2020, Month::December, 31, 23, 15));
        assert_equal(prev_range->end(), Datetime(2020, Month::December, 31, 23, 16));
    })
    .test("h12_time am", [&]() {
        auto filter = compile("11:15 am");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::January, 1, 11, 15));
        assert_equal(next_range->end(), Datetime(2021, Month::January, 1, 11, 16));
        assert_equal(prev_range->start(), Datetime(2020, Month::December, 31, 11, 15));
        assert_equal(prev_range->end(), Datetime(2020, Month::December, 31, 11, 16));
    })
    .test("h12_time pm", [&]() {
        auto filter = compile("11:15 pm");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::January, 1, 23, 15));
        assert_equal(next_range->end(), Datetime(2021, Month::January, 1, 23, 16));
        assert_equal(prev_range->start(), Datetime(2020, Month::December, 31, 23, 15));
        assert_equal(prev_range->end(), Datetime(2020, Month::December, 31, 23, 16));
    })
    .test("h24_time", [&]() {
        auto filter = compile("13:15");
        assert_false(filter->is_absolute());
        auto pivot = Datetime(2021, Month::January, 1);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);
        assert_equal(next_range->start(), Datetime(2021, Month::January, 1, 13, 15));
        assert_equal(next_range->end(), Datetime(2021, Month::January, 1, 13, 16));
        assert_equal(prev_range->start(), Datetime(2020, Month::December, 31, 13, 15));
        assert_equal(prev_range->end(), Datetime(2020, Month::December, 31, 13, 16));
    })
    .run();
}
