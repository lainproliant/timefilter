/*
 * parser.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Sunday March 28, 2021
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_PARSER_H
#define __TIMEFILTER_PARSER_H

#include "moonlight/lex.h"
#include "moonlight/date.h"
#include "moonlight/linked_map.h"
#include "moonlight/maps.h"
#include "moonlight/string.h"
#include "timefilter/core.h"

namespace timefilter {

namespace lex = moonlight::lex;

typedef moonlight::linked_map<moonlight::date::Weekday, std::string> WeekdayFormatMap;
typedef moonlight::linked_map<moonlight::date::Month, std::string> MonthFormatMap;

// ------------------------------------------------------------------
inline WeekdayFormatMap format_weekdays(const std::string& fmt) {
    WeekdayFormatMap weekday_to_format;
    const auto start_date = moonlight::date::Date(2021, 3, 28);
    for (int x = 0; x < 7; x++) {
        auto dt = moonlight::date::Datetime(start_date.advance_days(x));
        weekday_to_format.insert({dt.date().weekday(), dt.format(fmt)});
    }
    return weekday_to_format;
}

// ------------------------------------------------------------------
inline MonthFormatMap format_months(const std::string& fmt) {
    MonthFormatMap month_to_format;
    for (auto date = moonlight::date::Date(2021, 1, 1);
         date.year() < 2022;
         date = date.next_month()) {
        auto dt = moonlight::date::Datetime(date);
        month_to_format.insert({dt.date().month(), dt.format(fmt)});
    }

    return month_to_format;
}

// ------------------------------------------------------------------
inline const MonthFormatMap& short_month_map() {
    static const auto short_months = format_months("%b");
    return short_months;
}

// ------------------------------------------------------------------
inline const MonthFormatMap& long_month_map() {
    static const auto long_months = format_months("%B");
    return long_months;
}

// ------------------------------------------------------------------
inline const WeekdayFormatMap& short_weekday_map() {
    static const auto short_weekdays = format_weekdays("%a");
    return short_weekdays;
}

// ------------------------------------------------------------------
inline const WeekdayFormatMap& long_weekday_map() {
    static const auto long_weekdays = format_weekdays("%A");
    return long_weekdays;
}

// ------------------------------------------------------------------
template<class T>
inline std::string make_rx_group(const T& values) {
    std::ostringstream sb;
    sb << "(" << moonlight::str::join(values, "|") << ")";
    return sb.str();
}

// ------------------------------------------------------------------
inline const std::string& short_weekday_rx_group() {
    static const auto group = make_rx_group(moonlight::maps::values(short_weekday_map()));
    return group;
}

// ------------------------------------------------------------------
inline const std::string& long_weekday_rx_group() {
    static const auto group = make_rx_group(moonlight::maps::values(long_weekday_map()));
    return group;
}

// ------------------------------------------------------------------
inline const std::string& short_month_rx_group() {
    static const auto group = make_rx_group(moonlight::maps::values(short_month_map()));
    return group;
}

// ------------------------------------------------------------------
inline const std::string& long_month_rx_group() {
    static const auto group = make_rx_group(moonlight::maps::values(long_month_map()));
    return group;
}

// ------------------------------------------------------------------
inline lex::Grammar::Pointer make_grammar() {
    auto root = lex::Grammar::create();

    root
        ->def(lex::ignore("\\s+"))
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s ([0-9]{4})", short_month_rx_group())).icase(), "dmy_short")
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s ([0-9]{4})", long_month_rx_group())).icase(), "dmy_long")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2}) ([0-9]{4})", short_month_rx_group())).icase(), "mdy_short")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2}) ([0-9]{4})", long_month_rx_group())).icase(), "mdy_long")
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s", short_month_rx_group())).icase(), "dm_short")
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s", long_month_rx_group())).icase(), "dm_long")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2})", short_month_rx_group())).icase(), "md_short")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2})", long_month_rx_group())).icase(), "md_long")
        ->def(lex::match(short_month_rx_group()).icase(), "month_short")
        ->def(lex::match(long_month_rx_group()).icase(), "month_long")
        ->def(lex::match(short_weekday_rx_group()).icase(), "weekday_short")
        ->def(lex::match(long_weekday_rx_group()).icase(), "weekday_long")
        ->def(lex::match("([0-9]{4})-([0-9]{2})-([0-9]{2})"), "iso_date")
        ->def(lex::match("([0-9]{2})/([0-9]{2})/([0-9]{4})"), "us_date")
        ->def(lex::match("([0-9]{1,2})([0-9]{2})h"), "mil_time")
        ->def(lex::match("([0-9]{1,2}):([0-9]{2})\\s?(am|pm|a|p)").icase(), "12h_time")
        ->def(lex::match("([0-9]{1,2}):([0-9]{2})"), "24h_time");

    return root;
}

}

#endif /* !__TIMEFILTER_PARSER_H */
