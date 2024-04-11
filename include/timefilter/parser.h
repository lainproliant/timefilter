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

#include <memory>
#include <set>
#include <vector>
#include <map>
#include <string>
#include <deque>
#include <algorithm>

#include "moonlight/lex.h"
#include "moonlight/date.h"
#include "moonlight/linked_map.h"
#include "moonlight/maps.h"
#include "moonlight/string.h"
#include "moonlight/json.h"
#include "moonlight/automata.h"
#include "timefilter/filters.h"

namespace timefilter {

namespace lex = moonlight::lex;
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
enum TokenType {
    DAY_MONTH_YEAR,
    MONTH_DAY_YEAR,
    MONTH_YEAR,
    YEAR_MONTH,
    DAY_MONTH,
    MONTH_DAY,
    WEEKDAY_MONTHDAY,
    MONTH,
    WEEKDAY,
    ISO_DATE,
    US_DATE,
    MIL_TIME,
    H12_TIME,
    H24_TIME,
    YEAR,
    WEEKDAYS,
    DURATION,
    OP_RANGE_DASH,
    OP_RANGE_PLUS
};

using Grammar = lex::Grammar<TokenType>;

// ------------------------------------------------------------------
typedef std::function<Filter::Pointer(const I18nStrings&, const Grammar::Token&)> FilterFactory;

// ------------------------------------------------------------------
inline Grammar::Pointer make_grammar(const I18nStrings& i18n) {
    auto root = Grammar::create();
    const std::string term = "([^\\w\\d]|$)";

    root
    ->def(lex::ignore("\\s+"))
    ->def(lex::match(tfm::format("([0-9]{1,2}) %s ([0-9]{4,})", i18n.long_month_rx())).icase(), DAY_MONTH_YEAR)
    ->def(lex::match(tfm::format("([0-9]{1,2}) %s ([0-9]{4,})", i18n.short_month_rx())).icase(), DAY_MONTH_YEAR)
    ->def(lex::match(tfm::format("%s ([0-9]{1,2}) ([0-9]{4,})", i18n.long_month_rx())).icase(), MONTH_DAY_YEAR)
    ->def(lex::match(tfm::format("%s ([0-9]{1,2}) ([0-9]{4,})", i18n.short_month_rx())).icase(), MONTH_DAY_YEAR)
    ->def(lex::match(tfm::format("%s ([0-9]{4,})", i18n.long_month_rx())).icase(), MONTH_YEAR)
    ->def(lex::match(tfm::format("%s ([0-9]{4,})", i18n.short_month_rx())).icase(), MONTH_YEAR)
    ->def(lex::match(tfm::format("([0-9]{4,}) %s", i18n.long_month_rx())).icase(), YEAR_MONTH)
    ->def(lex::match(tfm::format("([0-9]{4,}) %s", i18n.short_month_rx())).icase(), YEAR_MONTH)
    ->def(lex::match(tfm::format("([0-9]{1,2}) %s", i18n.long_month_rx())).icase(), DAY_MONTH)
    ->def(lex::match(tfm::format("([0-9]{1,2}) %s", i18n.short_month_rx())).icase(), DAY_MONTH)
    ->def(lex::match(tfm::format("%s ([0-9]{1,2})", i18n.long_month_rx())).icase(), MONTH_DAY)
    ->def(lex::match(tfm::format("%s ([0-9]{1,2})", i18n.short_month_rx())).icase(), MONTH_DAY)
    ->def(lex::match(tfm::format("%s ([0-9]{1,2})", i18n.long_weekday_rx())).icase(), WEEKDAY_MONTHDAY)
    ->def(lex::match(tfm::format("%s ([0-9]{1,2})", i18n.short_weekday_rx())).icase(), WEEKDAY_MONTHDAY)
    ->def(lex::match(i18n.long_month_rx() + term).icase(), MONTH)
    ->def(lex::match(i18n.short_month_rx() + term).icase(), MONTH)
    ->def(lex::match(i18n.long_weekday_rx() + term).icase(), WEEKDAY)
    ->def(lex::match(i18n.short_weekday_rx() + term).icase(), WEEKDAY)
    ->def(lex::match("([0-9]{4,})-([0-9]{2})-([0-9]{2})"), ISO_DATE)
    ->def(lex::match("([0-9]{1,2})/([0-9]{1,2})/([0-9]{4,})"), US_DATE)
    ->def(lex::match("([0-9]{1,2})([0-9]{2})h"), MIL_TIME)
    ->def(lex::match("([0-9]{1,2}):([0-9]{2})\\s?(am|pm|a|p)").icase(), H12_TIME)
    ->def(lex::match("([0-9]{1,2}):([0-9]{2})"), H24_TIME)
    ->def(lex::match("[0-9]{4,}" + term), YEAR)
    ->def(lex::match("[MTWHFSU]{1,7}"), WEEKDAYS)
    ->def(lex::match("([0-9]+)([wdhms])").icase(), DURATION)
    ->def(lex::match("-"), OP_RANGE_DASH)
    ->def(lex::match("+"), OP_RANGE_PLUS);

    return root;
}

// ------------------------------------------------------------------
class Parser {
 public:
     explicit Parser(const I18nStrings& i18n = I18nStrings::defaults())
     : _i18n(i18n), _lex(Grammar::Lexer()), _grammar(make_grammar(i18n)) { }

     std::vector<Grammar::Token> parse(const std::string& expr) const {
         return _lex.lex(_grammar, expr);
     }

 private:
     const I18nStrings _i18n;
     const Grammar::Lexer _lex;
     const Grammar::Pointer _grammar;
};

}  // namespace timefilter

#endif /* !__TIMEFILTER_PARSER_H */
