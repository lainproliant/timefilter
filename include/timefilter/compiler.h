/*
 * compiler.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday February 21, 2024
 */

#ifndef __TIMEFILTER_COMPILER_H
#define __TIMEFILTER_COMPILER_H

#include "timefilter/parser.h"

namespace timefilter {

// ------------------------------------------------------------------
inline std::map<TokenType, FilterFactory> make_factories() {
    std::map<TokenType, FilterFactory> factories;

    factories[DAY_MONTH_YEAR] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        auto list = std::make_shared<ListFilter>();
        int day = std::stoi(token.match().group(1));
        Month month = i18n.month(token.match().group(2));
        int year = std::stoi(token.match().group(3));
        list->push(MonthdayFilter::create(day));
        list->push(MonthFilter::create(month));
        list->push(YearFilter::create(year));

        return list;
    };

    factories[MONTH_DAY_YEAR] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        auto list = std::make_shared<ListFilter>();
        Month month = i18n.month(token.match().group(1));
        int day = std::stoi(token.match().group(2));
        int year = std::stoi(token.match().group(3));
        list->push(MonthdayFilter::create(day));
        list->push(MonthFilter::create(month));
        list->push(YearFilter::create(year));

        return list;
    };

    factories[MONTH_YEAR] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        auto list = std::make_shared<ListFilter>();
        Month month = i18n.month(token.match().group(1));
        int year = std::stoi(token.match().group(2));
        list->push(MonthFilter::create(month));
        list->push(YearFilter::create(year));
        return list;
    };

    factories[YEAR_MONTH] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        auto list = std::make_shared<ListFilter>();
        int year = std::stoi(token.match().group(1));
        Month month = i18n.month(token.match().group(2));
        list->push(MonthFilter::create(month));
        list->push(YearFilter::create(year));
        return list;
    };

    factories[MONTH_DAY] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        auto list = std::make_shared<ListFilter>();
        Month month = i18n.month(token.match().group(1));
        int day = std::stoi(token.match().group(2));
        list->push(MonthdayFilter::create(day));
        list->push(MonthFilter::create(month));

        return list;
    };

    factories[DAY_MONTH] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        auto list = std::make_shared<ListFilter>();
        int day = std::stoi(token.match().group(1));
        Month month = i18n.month(token.match().group(2));
        list->push(MonthdayFilter::create(day));
        list->push(MonthFilter::create(month));

        return list;
    };

    factories[MONTH] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        Month month = i18n.month(token.match().group(1));
        return MonthFilter::create(month);
    };

    factories[WEEKDAY] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        Weekday weekday = i18n.weekday(token.match().group(1));
        return WeekdayFilter::create(weekday);
    };

    factories[WEEKDAY_MONTHDAY] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        auto list = ListFilter::create();
        Weekday weekday = i18n.weekday(token.match().group(1));
        int monthday = std::stoi(token.match().group(2));
        list->push(WeekdayFilter::create(weekday));
        list->push(MonthdayFilter::create(monthday));

        return list;
    };

    factories[ISO_DATE] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
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

    factories[US_DATE]
    = [](const I18nStrings& i18n, const Grammar::Token& token) {
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

    factories[MIL_TIME] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        (void) i18n;
        int hour = std::stoi(token.match().group(1));
        int minute = std::stoi(token.match().group(2));
        return TimeFilter::create(Time(hour, minute));
    };

    factories[H12_TIME] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
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

    factories[H24_TIME] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        (void) i18n;
        int hour = std::stoi(token.match().group(1));
        int minute = std::stoi(token.match().group(2));
        return TimeFilter::create(Time(hour, minute));
    };

    factories[YEAR] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
        (void) i18n;
        return YearFilter::create(std::stoi(token.match().group()));
    };

    factories[WEEKDAYS] =
    [](const I18nStrings& i18n, const Grammar::Token& token) {
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

    return factories;
}

// ------------------------------------------------------------------
class Compiler {
 public:
     Compiler(
         const I18nStrings& i18n,
         const std::map<std::string, FilterFactory>& factories)
     : _i18n(i18n), _factories(factories) { }

     Filter::Pointer compile(const std::vector<Grammar::Token>& tokens) const {
         struct Context {
             ListFilter::Pointer filter = ListFilter::create();
             ListFilter::Pointer lhs_filter = nullptr;
             std::deque<Grammar::Token> tokens;
         };

         Context ctx;
         std::copy(tokens.begin(), tokens.end(), std::back_inserter(ctx.tokens));

         enum CompileState {
             EXPRESSION,
             RANGE_PLUS,
             RANGE_DASH
         };

         auto machine = moonlight::automata::Lambda<Context, CompileState>::builder(ctx)
         .init(EXPRESSION)
         .state(EXPRESSION, [&](auto& m) {
             if (ctx.tokens.empty()) {
                 m.pop();
                 return;
             }

             auto token = ctx.tokens.front();
             ctx.tokens.pop_front();

             if (token.type() == OP_RANGE_DASH || token.type() == OP_RANGE_PLUS) {
                 ctx.lhs_filter = ctx.filter;
                 ctx.filter = ListFilter::create();
                 if (token.type() == OP_RANGE_DASH) {
                     m.transition(RANGE_DASH);

                 } else if (token.type() == OP_RANGE_PLUS) {
                     m.transition(RANGE_PLUS);
                 }
                 m.push(EXPRESSION);
             }
         })
         .state(RANGE_PLUS, [&](auto& m) {

         })
         .state(RANGE_DASH, [&](auto& m) {

         })
         .build();

         return ctx.filter;
     }


     const timefilter::I18nStrings _i18n;
     const std::map<std::string, FilterFactory> _factories;
};

// ------------------------------------------------------------------
inline Filter::Pointer eval(const std::string& expr, const I18nStrings& i18n = I18nStrings::defaults()) {
    Parser parser(i18n);
    Compiler compiler(i18n, make_factories());
    return compiler.compile(parser.parse(expr));
}

}  // namespace timefilter


#endif /* !__TIMEFILTER_COMPILER_H */
