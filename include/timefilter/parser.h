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
#include "timefilter/filters.h"

namespace timefilter {

namespace lex = moonlight::lex;
namespace json = moonlight::json;
using moonlight::str::to_lower;

// ------------------------------------------------------------------
class CompilerError : public Error {
    using Error::Error;
};

// ------------------------------------------------------------------
class I18nStrings {
public:
    I18nStrings(const json::Object& obj) :
    _short_months(obj.get<json::Array>("short_months", format_months("%b")).extract<std::string>()),
    _long_months(obj.get<json::Array>("long_months", format_months("%B")).extract<std::string>()),
    _short_weekdays(obj.get<json::Array>("short_weekdays", format_weekdays("%a")).extract<std::string>()),
    _long_weekdays(obj.get<json::Array>("long_weekdays", format_weekdays("%A")).extract<std::string>()),
    _short_month_rx(make_rx(_short_months)),
    _long_month_rx(make_rx(_long_months)),
    _short_weekday_rx(make_rx(_short_weekdays)),
    _long_weekday_rx(make_rx(_long_weekdays)) {}

    I18nStrings() : I18nStrings(json::Object()) { }

    std::string short_weekday_rx() const {
        return _short_weekday_rx;
    }

    const std::string& long_weekday_rx() const {
        return _long_weekday_rx;
    }

    const std::string& short_month_rx() const {
        return _short_month_rx;
    }

    const std::string& long_month_rx() const {
        return _long_month_rx;
    }

    Weekday weekday(const std::string& s) const {
        for (size_t x = 0; x < 7; x++) {
            if (to_lower(s) == to_lower(_short_weekdays[x]) ||
                to_lower(s) == to_lower(_long_weekdays[x])) {
                return static_cast<Weekday>(x);
            }
        }
        throw ValueError("Unknown weekday: " + s);
    }

    Month month(const std::string& s) const {
        for (size_t x = 0; x < 12; x++) {
            if (to_lower(s) == to_lower(_short_months[x]) ||
                to_lower(s) == to_lower(_long_months[x])) {
                return static_cast<Month>(x);
            }
        }
        throw ValueError("Unknown month: " + s);
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

    template<class T>
    static std::string make_rx(const T& values) {
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
typedef std::function<Filter::Pointer(const I18nStrings&, const moonlight::lex::Token&)> FilterFactory;

// ------------------------------------------------------------------
inline lex::Grammar::Pointer make_grammar(const I18nStrings& i18n) {
    auto root = lex::Grammar::create();
    const std::string term = "([^\\w\\d]|$)";

    root
        ->def(lex::ignore("\\s+"))
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s ([0-9]{4,})", i18n.long_month_rx())).icase(), "dmy_long")
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s ([0-9]{4,})", i18n.short_month_rx())).icase(), "dmy_short")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2}) ([0-9]{4,})", i18n.long_month_rx())).icase(), "mdy_long")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2}) ([0-9]{4,})", i18n.short_month_rx())).icase(), "mdy_short")
        ->def(lex::match(tfm::format("%s ([0-9]{4,})", i18n.long_month_rx())).icase(), "my_long")
        ->def(lex::match(tfm::format("%s ([0-9]{4,})", i18n.short_month_rx())).icase(), "my_short")
        ->def(lex::match(tfm::format("([0-9]{4,}) %s", i18n.long_month_rx())).icase(), "ym_long")
        ->def(lex::match(tfm::format("([0-9]{4,}) %s", i18n.short_month_rx())).icase(), "ym_short")
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s", i18n.long_month_rx())).icase(), "dm_long")
        ->def(lex::match(tfm::format("([0-9]{1,2}) %s", i18n.short_month_rx())).icase(), "dm_short")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2})", i18n.long_month_rx())).icase(), "md_long")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2})", i18n.short_month_rx())).icase(), "md_short")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2})", i18n.long_weekday_rx())).icase(), "weekday_monthday_long")
        ->def(lex::match(tfm::format("%s ([0-9]{1,2})", i18n.short_weekday_rx())).icase(), "weekday_monthday_short")
        ->def(lex::match(i18n.long_month_rx() + term).icase(), "month_long")
        ->def(lex::match(i18n.short_month_rx() + term).icase(), "month_short")
        ->def(lex::match(i18n.long_weekday_rx() + term).icase(), "weekday_long")
        ->def(lex::match(i18n.short_weekday_rx() + term).icase(), "weekday_short")
        ->def(lex::match("([0-9]{4,})-([0-9]{2})-([0-9]{2})"), "iso_date")
        ->def(lex::match("([0-9]{1,2})/([0-9]{1,2})/([0-9]{4,})"), "us_date")
        ->def(lex::match("([0-9]{1,2})([0-9]{2})h"), "mil_time")
        ->def(lex::match("([0-9]{1,2}):([0-9]{2})\\s?(am|pm|a|p)").icase(), "h12_time")
        ->def(lex::match("([0-9]{1,2}):([0-9]{2})"), "h24_time")
        ->def(lex::match("[0-9]{4,}" + term), "year")
        ->def(lex::match("[MTWHFSU]{1,7}"), "weekdays");

    return root;
}

// ------------------------------------------------------------------
inline std::map<std::string, FilterFactory> make_factories() {
    const auto dmy_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        auto list = std::make_shared<ListFilter>();
        int day = std::stoi(token.match().group(1));
        Month month = i18n.month(token.match().group(2));
        int year = std::stoi(token.match().group(3));
        list->push(MonthdayFilter::create(day));
        list->push(MonthFilter::create(month));
        list->push(YearFilter::create(year));

        return list;
    };

    const auto mdy_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        auto list = std::make_shared<ListFilter>();
        Month month = i18n.month(token.match().group(1));
        int day = std::stoi(token.match().group(2));
        int year = std::stoi(token.match().group(3));
        list->push(MonthdayFilter::create(day));
        list->push(MonthFilter::create(month));
        list->push(YearFilter::create(year));

        return list;
    };

    const auto my_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        auto list = std::make_shared<ListFilter>();
        Month month = i18n.month(token.match().group(1));
        int year = std::stoi(token.match().group(2));
        list->push(MonthFilter::create(month));
        list->push(YearFilter::create(year));
        return list;
    };

    const auto ym_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        auto list = std::make_shared<ListFilter>();
        int year = std::stoi(token.match().group(1));
        Month month = i18n.month(token.match().group(2));
        list->push(MonthFilter::create(month));
        list->push(YearFilter::create(year));
        return list;
    };

    const auto md_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        auto list = std::make_shared<ListFilter>();
        Month month = i18n.month(token.match().group(1));
        int day = std::stoi(token.match().group(2));
        list->push(MonthdayFilter::create(day));
        list->push(MonthFilter::create(month));

        return list;
    };

    const auto dm_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        auto list = std::make_shared<ListFilter>();
        int day = std::stoi(token.match().group(1));
        Month month = i18n.month(token.match().group(2));
        list->push(MonthdayFilter::create(day));
        list->push(MonthFilter::create(month));

        return list;
    };

    const auto month_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        Month month = i18n.month(token.match().group(1));
        return MonthFilter::create(month);
    };

    const auto weekday_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        Weekday weekday = i18n.weekday(token.match().group(1));
        return WeekdayFilter::create(weekday);
    };

    const auto weekday_monthday_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        auto list = ListFilter::create();
        Weekday weekday = i18n.weekday(token.match().group(1));
        int monthday = std::stoi(token.match().group(2));
        list->push(WeekdayFilter::create(weekday));
        list->push(MonthdayFilter::create(monthday));

        return list;
    };

    const auto iso_date_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        (void) i18n;
        auto list = ListFilter::create();
        int year = std::stoi(token.match().group(1));
        Month month = static_cast<Month>(std::stoi(token.match().group(2)) - 1);
        int day = std::stoi(token.match().group(3));
        list->push(MonthdayFilter::create(day));
        list->push(MonthFilter::create(month));
        list->push(YearFilter::create(year));

        return list;
    };

    const auto us_date_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        (void) i18n;
        auto list = ListFilter::create();
        Month month = static_cast<Month>(std::stoi(token.match().group(1)) - 1);
        int day = std::stoi(token.match().group(2));
        int year = std::stoi(token.match().group(3));
        list->push(MonthdayFilter::create(day));
        list->push(MonthFilter::create(month));
        list->push(YearFilter::create(year));

        return list;
    };

    const auto mil_time_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        (void) i18n;
        int hour = std::stoi(token.match().group(1));
        int minute = std::stoi(token.match().group(2));
        return TimeFilter::create(Time(hour, minute));
    };

    const auto h12_time_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        (void) i18n;
        int hour = std::stoi(token.match().group(1));
        int minute = std::stoi(token.match().group(2));
        std::string ampm = token.match().group(3);
        if ((to_lower(ampm) == "p" || to_lower(ampm) == "pm") && hour < 12) {
            hour += 12;
        } else if ((to_lower(ampm) == "a" || to_lower(ampm) == "am") && hour == 12) {
            hour = 0;
        }
        return TimeFilter::create(Time(hour, minute));
    };

    const auto h24_time_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        (void) i18n;
        int hour = std::stoi(token.match().group(1));
        int minute = std::stoi(token.match().group(2));
        return TimeFilter::create(Time(hour, minute));
    };

    const auto year_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        (void) i18n;
        return YearFilter::create(std::stoi(token.match().group()));
    };

    const auto weekdays_factory = [](const I18nStrings& i18n, const lex::Token& token) {
        (void) i18n;
        static std::map<char, Weekday> weekday_map = {
            {'M', Weekday::Monday},
            {'T', Weekday::Tuesday},
            {'W', Weekday::Wednesday},
            {'H', Weekday::Thursday},
            {'F', Weekday::Friday},
            {'S', Weekday::Saturday},
            {'U', Weekday::Sunday}
        };
        std::set<Weekday> weekdays;
        for (auto c : token.match().group()) {
            weekdays.insert(weekday_map.find(c)->second);
        }
        return WeekdayFilter::create(weekdays);
    };

    std::map<std::string, FilterFactory> factories = {
        {"dmy_long", dmy_factory},
        {"dmy_short", dmy_factory},
        {"mdy_long", mdy_factory},
        {"mdy_short", mdy_factory},
        {"my_long", my_factory},
        {"my_short", my_factory},
        {"ym_long", ym_factory},
        {"ym_short", ym_factory},
        {"md_long", md_factory},
        {"md_short", md_factory},
        {"dm_long", dm_factory},
        {"dm_short", dm_factory},
        {"month_short", month_factory},
        {"month_long", month_factory},
        {"weekday_short", weekday_factory},
        {"weekday_long", weekday_factory},
        {"weekday_monthday_short", weekday_monthday_factory},
        {"weekday_monthday_long", weekday_monthday_factory},
        {"iso_date", iso_date_factory},
        {"us_date", us_date_factory},
        {"mil_time", mil_time_factory},
        {"h12_time", h12_time_factory},
        {"h24_time", h24_time_factory},
        {"year", year_factory},
        {"weekdays", weekdays_factory}
    };

    return factories;
}

// ------------------------------------------------------------------
class Compiler {
public:
    Compiler(
        const I18nStrings& i18n,
        const std::map<std::string, FilterFactory>& factories)
    : _i18n(i18n), _factories(factories) { }

    Filter::Pointer compile(const std::vector<moonlight::lex::Token>& tokens) const {
        auto list = ListFilter::create();
        for (auto token : tokens) {
            auto iter = _factories.find(token.type());
            if (iter == _factories.end()) {
                throw CompilerError(tfm::format("Missing token factory for type %s", token.type()));
            }
            list->push(iter->second(_i18n, token));
        }
        return list;
    }

private:
    const timefilter::I18nStrings _i18n;
    const std::map<std::string, FilterFactory> _factories;
};

}

#endif /* !__TIMEFILTER_PARSER_H */
