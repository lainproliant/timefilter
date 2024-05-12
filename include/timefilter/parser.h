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

#include <vector>
#include <string>

#include "moonlight/date.h"
#include "moonlight/string.h"
#include "moonlight/json.h"
#include "timefilter/filters.h"
#include "timefilter/tokens.h"

namespace timefilter {

namespace json = moonlight::json;
using moonlight::str::to_lower;

// ------------------------------------------------------------------
EXCEPTION_SUBTYPE(Error, CompilerError);

// ------------------------------------------------------------------
class I18nStrings {
 public:
     explicit I18nStrings(const json::Object& obj) :
     _short_months(obj.get<json::Array>("short_months", format_months("%b")).extract<std::string>()),
     _long_months(obj.get<json::Array>("long_months", format_months("%B")).extract<std::string>()),
     _short_weekdays(obj.get<json::Array>("short_weekdays", format_weekdays("%a")).extract<std::string>()),
     _long_weekdays(obj.get<json::Array>("long_weekdays", format_weekdays("%A")).extract<std::string>()),
     _short_month_rx(make_rx(_short_months)),
     _long_month_rx(make_rx(_long_months)),
     _short_weekday_rx(make_rx(_short_weekdays)),
     _long_weekday_rx(make_rx(_long_weekdays)) {}

     I18nStrings() : I18nStrings(json::Object()) { }

     static const I18nStrings& defaults() {
         static const I18nStrings i18n;
         return i18n;
     }

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
inline Grammar make_grammar(const I18nStrings& i18n) {
    const std::string term = "(?:[^\\w\\d]|$)";

    return Grammar()
    .def(lex::ignore("\\s+"))
    .def(lex::match(tfm::format("([0-9]{1,2})(?:\\w+)? %s ([0-9]{4,})", i18n.long_month_rx())).icase(), TokenType::DAY_MONTH_YEAR)
    .def(lex::match(tfm::format("([0-9]{1,2})(?:\\w+)? %s ([0-9]{4,})", i18n.short_month_rx())).icase(), TokenType::DAY_MONTH_YEAR)
    .def(lex::match(tfm::format("%s ([0-9]{1,2})(?:\\w+)? ([0-9]{4,})", i18n.long_month_rx())).icase(), TokenType::MONTH_DAY_YEAR)
    .def(lex::match(tfm::format("%s ([0-9]{1,2})(?:\\w+)? ([0-9]{4,})", i18n.short_month_rx())).icase(), TokenType::MONTH_DAY_YEAR)
    .def(lex::match(tfm::format("([0-9]{4,}) %s ([0-9]{1,2})(?:\\w+)?", i18n.long_month_rx())).icase(), TokenType::YEAR_MONTH_DAY)
    .def(lex::match(tfm::format("([0-9]{4,}) %s ([0-9]{1,2})(?:\\w+)?", i18n.short_month_rx())).icase(), TokenType::YEAR_MONTH_DAY)
    .def(lex::match(tfm::format("%s ([0-9]{4,})", i18n.long_month_rx())).icase(), TokenType::MONTH_YEAR)
    .def(lex::match(tfm::format("%s ([0-9]{4,})", i18n.short_month_rx())).icase(), TokenType::MONTH_YEAR)
    .def(lex::match(tfm::format("([0-9]{4,}) %s", i18n.long_month_rx())).icase(), TokenType::YEAR_MONTH)
    .def(lex::match(tfm::format("([0-9]{4,}) %s", i18n.short_month_rx())).icase(), TokenType::YEAR_MONTH)
    .def(lex::match(tfm::format("%s ([0-9]{1,2})(?:\\w+)?", i18n.long_month_rx())).icase(), TokenType::MONTH_DAY)
    .def(lex::match(tfm::format("%s ([0-9]{1,2})(?:\\w+)?", i18n.short_month_rx())).icase(), TokenType::MONTH_DAY)
    .def(lex::match(tfm::format("%s ([0-9]{1,2})(?:\\w+)?", i18n.long_weekday_rx())).icase(), TokenType::WEEKDAY_MONTHDAY)
    .def(lex::match(tfm::format("%s ([0-9]{1,2})(?:\\w+)?", i18n.short_weekday_rx())).icase(), TokenType::WEEKDAY_MONTHDAY)
    .def(lex::match(i18n.long_month_rx() + term).icase(), TokenType::MONTH)
    .def(lex::match(i18n.short_month_rx() + term).icase(), TokenType::MONTH)
    .def(lex::match(i18n.long_weekday_rx() + term).icase(), TokenType::WEEKDAY)
    .def(lex::match(i18n.short_weekday_rx() + term).icase(), TokenType::WEEKDAY)
    .def(lex::match("([0-9]{4,})-([0-9]{2})-([0-9]{2})"), TokenType::ISO_DATE)
    .def(lex::match("([0-9]{1,2})/([0-9]{1,2})/([0-9]{4,})"), TokenType::US_DATE)
    .def(lex::match("([0-9]{1,2})([0-9]{2})h"), TokenType::MIL_TIME)
    .def(lex::match("([0-9]{1,2}):([0-9]{2})\\s?(am|pm|a|p)").icase(), TokenType::H12_TIME)
    .def(lex::match("([0-9]{1,2}):([0-9]{2})"), TokenType::H24_TIME)
    .def(lex::match("[0-9]{4,}" + term), TokenType::YEAR)
    .def(lex::match("[MTWHFSU]{1,7}"), TokenType::WEEKDAYS)
    .def(lex::match("([0-9]+)(hr)").icase(), TokenType::DURATION)
    .def(lex::match("([0-9]+)(min)").icase(), TokenType::DURATION)
    .def(lex::match("([0-9]+)(sec)").icase(), TokenType::DURATION)
    .def(lex::match("([0-9]+)(ms)").icase(), TokenType::DURATION)
    .def(lex::match("([0-9]+)([wdhms])").icase(), TokenType::DURATION)
    .def(lex::match(tfm::format("([0-9]{1,2})(?:\\w+) %s", i18n.long_month_rx())).icase(), TokenType::DAY_MONTH)
    .def(lex::match(tfm::format("([0-9]{1,2})(?:\\w+) %s", i18n.short_month_rx())).icase(), TokenType::DAY_MONTH)
    .def(lex::match("-"), TokenType::OP_RANGE)
    .def(lex::match("\\+"), TokenType::OP_DURATION)
    .def(lex::match(","), TokenType::OP_JOIN);
}

// ------------------------------------------------------------------
class Parser {
 public:
     explicit Parser(const I18nStrings& i18n = I18nStrings::defaults())
     : _i18n(i18n), _grammar(make_grammar(i18n)) { }

     std::vector<Grammar::Token> parse(const std::string& expr) const {
         return _grammar.lexer().lex(expr);
     }

 private:
     const I18nStrings _i18n;
     const Grammar _grammar;
};

}  // namespace timefilter

#endif /* !__TIMEFILTER_PARSER_H */
