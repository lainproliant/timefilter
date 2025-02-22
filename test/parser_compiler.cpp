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

    auto filter_test = [&](const std::string& expr,
                           const Datetime& pivot,
                           const std::optional<Range>& exp_prev_range,
                           const std::optional<Range>& exp_next_range) {
        auto filter = compile(expr);
        tfm::printfln("expr = '%s', filter = %s", expr, *filter);
        auto next_range = filter->next_range(pivot);
        auto prev_range = filter->prev_range(pivot);

        if (exp_prev_range.has_value()) {
            tfm::printfln("exp_prev_range = %s", *exp_prev_range);
            ASSERT_TRUE(prev_range.has_value());
            tfm::printfln("prev_range = %s", *prev_range);
            ASSERT_EQUAL(*prev_range, *exp_prev_range);

        } else {
            ASSERT_FALSE(prev_range.has_value());
        }

        if (exp_next_range.has_value()) {
            tfm::printfln("exp_next_range = %s", *exp_next_range);
            ASSERT_TRUE(next_range.has_value());
            tfm::printfln("next_range = %s", *next_range);
            ASSERT_EQUAL(*next_range, *exp_next_range);

        } else {
            ASSERT_FALSE(next_range.has_value());
        }
    };

    return TestSuite("timefilter parser tests")
    .test("dmy_long", [&]() {
        filter_test("8 June 1988",
                    Datetime(2020, Month::January, 1),
                    Range(
                        Datetime(1988, Month::June, 8),
                        Datetime(1988, Month::June, 9)),
                    {});
    })
    .test("dmy_short", [&]() {
        filter_test("8 Jun 1988",
                    Datetime(2020, Month::January, 1),
                    Range(
                        Datetime(1988, Month::June, 8),
                        Datetime(1988, Month::June, 9)),
                    {});
    })
    .test("mdy_long", [&]() {
        filter_test("June 8 1988",
                    Datetime(2020, Month::January, 1),
                    Range(
                        Datetime(1988, Month::June, 8),
                        Datetime(1988, Month::June, 9)),
                    {});
    })
    .test("mdy_short", [&]() {
        filter_test("Jun 8 1988",
                    Datetime(2020, Month::January, 1),
                    Range(
                        Datetime(1988, Month::June, 8),
                        Datetime(1988, Month::June, 9)),
                    {});
    })
    .test("my_long", [&]() {
        filter_test("September 2021",
                    Datetime(2020, Month::January, 1),
                    {},
                    Range(
                        Datetime(2021, Month::September, 1),
                        Datetime(2021, Month::October, 1)));
    })
    .test("my_short", [&]() {
        filter_test("Sep 2021",
                    Datetime(2020, Month::January, 1),
                    {},
                    Range(
                        Datetime(2021, Month::September, 1),
                        Datetime(2021, Month::October, 1)));
    })
    .test("ym_long", [&]() {
        filter_test("2021 September",
                    Datetime(2020, Month::January, 1),
                    {},
                    Range(
                        Datetime(2021, Month::September, 1),
                        Datetime(2021, Month::October, 1)));
    })
    .test("ym_short", [&]() {
        filter_test("2021 Sep",
                    Datetime(2020, Month::January, 1),
                    {},
                    Range(
                        Datetime(2021, Month::September, 1),
                        Datetime(2021, Month::October, 1)));
    })
    .test("dm_long", [&]() {
        filter_test("10 November",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::November, 10),
                        Datetime(2020, Month::November, 11)),
                    Range(
                        Datetime(2021, Month::November, 10),
                        Datetime(2021, Month::November, 11)));
    })
    .test("dm_short", [&]() {
        filter_test("10 Nov",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::November, 10),
                        Datetime(2020, Month::November, 11)),
                    Range(
                        Datetime(2021, Month::November, 10),
                        Datetime(2021, Month::November, 11)));
    })
    .test("md_long", [&]() {
        filter_test("November 10",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::November, 10),
                        Datetime(2020, Month::November, 11)),
                    Range(
                        Datetime(2021, Month::November, 10),
                        Datetime(2021, Month::November, 11)));
    })
    .test("md_short", [&]() {
        filter_test("Nov 10",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::November, 10),
                        Datetime(2020, Month::November, 11)),
                    Range(
                        Datetime(2021, Month::November, 10),
                        Datetime(2021, Month::November, 11)));
    })
    .test("weekday_monthday_long", [&]() {
        filter_test("Friday 13",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::November, 13),
                        Datetime(2020, Month::November, 14)
                    ),
                    Range(
                        Datetime(2021, Month::August, 13),
                        Datetime(2021, Month::August, 14)));
    })
    .test("weekday_monthday_short", [&]() {
        filter_test("Fri 13",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::November, 13),
                        Datetime(2020, Month::November, 14)
                    ),
                    Range(
                        Datetime(2021, Month::August, 13),
                        Datetime(2021, Month::August, 14)));
    })
    .test("month_long", [&]() {
        filter_test("October",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::October, 1),
                        Datetime(2020, Month::November, 1)
                    ),
                    Range(
                        Datetime(2021, Month::October, 1),
                        Datetime(2021, Month::November, 1)
                    ));
    })
    .test("month_short", [&]() {
        filter_test("Oct",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::October, 1),
                        Datetime(2020, Month::November, 1)
                    ),
                    Range(
                        Datetime(2021, Month::October, 1),
                        Datetime(2021, Month::November, 1)
                    ));
    })
    .test("iso_date", [&]() {
        filter_test("2021-06-03",
                    Datetime(2020, Month::January, 1),
                    {},
                    Range(
                        Datetime(2021, Month::June, 3),
                        Datetime(2021, Month::June, 4)
                    ));
    })
    .test("us_date", [&]() {
        filter_test("6/3/2021",
                    Datetime(1999, Month::January, 1),
                    {},
                    Range(
                        Datetime(2021, Month::June, 3),
                        Datetime(2021, Month::June, 4)
                    ));
    })
    .test("mil_time", [&]() {
        filter_test("2315h",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::December, 31, 23, 15),
                        Datetime(2020, Month::December, 31, 23, 16)
                    ),
                    Range(
                        Datetime(2021, Month::January, 1, 23, 15),
                        Datetime(2021, Month::January, 1, 23, 16)
                    ));
    })
    .test("h12_time am", [&]() {
        filter_test("11:15 am",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::December, 31, 11, 15),
                        Datetime(2020, Month::December, 31, 11, 16)
                    ),
                    Range(
                        Datetime(2021, Month::January, 1, 11, 15),
                        Datetime(2021, Month::January, 1, 11, 16)
                    ));
    })
    .test("h12_time pm", [&]() {
        filter_test("11:15 pm",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::December, 31, 23, 15),
                        Datetime(2020, Month::December, 31, 23, 16)
                    ),
                    Range(
                        Datetime(2021, Month::January, 1, 23, 15),
                        Datetime(2021, Month::January, 1, 23, 16)
                    ));
    })
    .test("h24_time", [&]() {
        filter_test("13:15",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::December, 31, 13, 15),
                        Datetime(2020, Month::December, 31, 13, 16)
                    ),
                    Range(
                        Datetime(2021, Month::January, 1, 13, 15),
                        Datetime(2021, Month::January, 1, 13, 16)
                    ));
    })
    .test("weekdays_repeating", [&]() {
        filter_test("TH 6:30pm",
                    Datetime(2024, Month::February, 12),
                    Range(
                        Datetime(2024, Month::February, 8, 18, 30),
                        Datetime(2024, Month::February, 8, 18, 31)
                    ),
                    Range(
                        Datetime(2024, Month::February, 13, 18, 30),
                        Datetime(2024, Month::February, 13, 18, 31)
                    ));
    })
    .test("weekday_repeating_range", [&]() {
        filter_test("W 3:00pm - 6:00pm",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::December, 30, 15, 00),
                        Datetime(2020, Month::December, 30, 18, 00)
                    ),
                    Range(
                        Datetime(2021, Month::January, 6, 15, 00),
                        Datetime(2021, Month::January, 6, 18, 00)
                    ));
    })
    .test("weekday_repeating_range_in_months", [&]() {
        filter_test("W 3:00pm - 6:00pm @ April October",
                    Datetime(2021, Month::January, 1),
                    Range(
                        Datetime(2020, Month::October, 28, 15, 00),
                        Datetime(2020, Month::October, 28, 18, 00)
                    ),
                    Range(
                        Datetime(2021, Month::April, 7, 15, 00),
                        Datetime(2021, Month::April, 7, 18, 00)
                    ));
    })
    .run();
}
