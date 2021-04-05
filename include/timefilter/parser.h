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
#include "moonlight/json.h"
#include "timefilter/core.h"

namespace timefilter {

namespace lex = moonlight::lex;

// ------------------------------------------------------------------
class I18nStrings {
public:
    I18nStrings(const moonlight::json::Wrapper& json) {
    }

    I18nStrings() : I18nStrings(moonlight::json::Wrapper()) { }

    const std::string& short_weekday_rx_group() const {
        return make_rx_group(moonlight::maps::values(_short_weekday_map));
    }

    const std::string& long_weekday_rx_group() {
        return make_rx_group(moonlight::maps::values(_long_weekday_map));
    }

    const std::string& short_month_rx_group() {
        return make_rx_group(moonlight::maps::values(_short_month_map));
    }

    const std::string& long_month_rx_group() {
        return make_rx_group(moonlight::maps::values(_long_month_map));
    }

private:
    static std::vector<std::string> format_weekdays(const std::string& fmt) {
        std::vector<std::string> weekday_to_format;
        const auto start_date = moonlight::date::Date(2021, 3, 28);
        for (int x = 0; x < 7; x++) {
            auto dt = moonlight::date::Datetime(start_date.advance_days(x));
            weekday_to_format.push_back(dt.format(fmt));
        }
        return weekday_to_format;
    }

    static std::vector<std::string> format_months(const std::string& fmt) {
        std::vector<std::string> month_to_format;
        for (auto date = moonlight::date::Date(2021, 1, 1);
             date.year() < 2022;
             date = date.next_month()) {
            auto dt = moonlight::date::Datetime(date);
            month_to_format.push_back(dt.format(fmt));
        }

        return month_to_format;
    }

    static const MonthFormatMap& get_locale_short_month_map() {
        static const auto short_months = format_months("%b");
        return short_months;
    }

    static const MonthFormatMap& get_locale_long_month_map() {
        static const auto long_months = format_months("%B");
        return long_months;
    }

    static const WeekdayFormatMap& get_locale_short_weekday_map() {
        static const auto short_weekdays = format_weekdays("%a");
        return short_weekdays;
    }

    static const WeekdayFormatMap& get_locale_long_weekday_map() {
        static const auto long_weekdays = format_weekdays("%A");
        return long_weekdays;
    }

    template<class T>
    static std::string make_rx_group(const T& values) {
        std::ostringstream sb;
        sb << "(" << moonlight::str::join(values, "|") << ")";
        return sb.str();
    }

    const std::vector<std::string> _short_months;
    const std::vector<std::string> _long_months;
    const std::vector<std::string> _short_weekdays;
    const std::vector<std::string> _long_weekdays;
    const std::string _short_month_rx;
    const std::string _long_month_rx;
    const std::string _short_weekday_rx;
    const std::string _long_weekday_rx;
};


// ------------------------------------------------------------------
inline lex::Grammar::Pointer make_grammar() {
    auto root = lex::Grammar::create();
    const std::string term = "([^\\w\\d]|$)";

    root
        ->def(lex::ignore("\\s+"))
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s ([0-9]{4})", long_month_rx_group())).icase(), "dmy_long")
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s ([0-9]{4})", short_month_rx_group())).icase(), "dmy_short")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2}) ([0-9]{4})", long_month_rx_group())).icase(), "mdy_long")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2}) ([0-9]{4})", short_month_rx_group())).icase(), "mdy_short")
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s", long_month_rx_group())).icase(), "dm_long")
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s", short_month_rx_group())).icase(), "dm_short")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2})", long_month_rx_group())).icase(), "md_long")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2})", short_month_rx_group())).icase(), "md_short")
        ->def(lex::match(long_month_rx_group() + term).icase(), "month_long")
        ->def(lex::match(short_month_rx_group() + term).icase(), "month_short")
        ->def(lex::match(long_weekday_rx_group() + term).icase(), "weekday_long")
        ->def(lex::match(short_weekday_rx_group() + term).icase(), "weekday_short")
        ->def(lex::match("([0-9]{4})-([0-9]{2})-([0-9]{2})"), "iso_date")
        ->def(lex::match("([0-9]{2})/([0-9]{2})/([0-9]{4})"), "us_date")
        ->def(lex::match("([0-9]{1,2})([0-9]{2})h"), "mil_time")
        ->def(lex::match("([0-9]{1,2}):([0-9]{2})\\s?(am|pm|a|p)").icase(), "12h_time")
        ->def(lex::match("([0-9]{1,2}):([0-9]{2})"), "24h_time");

    return root;
}

}

#endif /* !__TIMEFILTER_PARSER_H */
