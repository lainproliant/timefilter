/*
 * compiler.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday February 21, 2024
 */

#ifndef __TIMEFILTER_COMPILER_H
#define __TIMEFILTER_COMPILER_H

#include "timefilter/date.h"
#include "timefilter/duration.h"
#include "timefilter/join.h"
#include "timefilter/list.h"
#include "timefilter/month.h"
#include "timefilter/monthday.h"
#include "timefilter/parser.h"
#include "timefilter/range.h"
#include "timefilter/weekday.h"
#include "timefilter/year.h"
#include "moonlight/string.h"

namespace timefilter {

// ------------------------------------------------------------------
class Compiler {
 public:
     Compiler(const I18nStrings& i18n = I18nStrings::defaults()) : _i18n(i18n) { }

     enum State {
         INIT,
         DURATION,
         DURATION_JOIN,
         FILTER,
         FILTER_RANGE,
     };

     struct Context {
         std::stack<ListFilter::Pointer> filters;
         std::deque<Grammar::Token> tokens;
         std::optional<Duration> duration;

         ListFilter::Pointer top_filter() const {
             if (filters.empty()) {
                 throw CompilerError("Unexpected empty filter stack.");
             }
             return filters.top();
         }

         void new_filter() {
             filters.push(ListFilter::create());
         }

         ListFilter::Pointer pop_filter() {
             auto value = top_filter();
             filters.pop();
             return value;
         }

         bool at_end() const {
             return tokens.empty();
         }

         const Grammar::Token& front_token() const {
             if (tokens.empty()) {
                 THROW(CompilerError, "Unexpected end of expression.");
             }
             return tokens.front();
         }

         Grammar::Token pop_token() {
             auto tk = front_token();
             tokens.pop_front();
             return tk;
         }
     };

     Filter::Pointer compile_filter(const std::vector<Grammar::Token>& tokens) const {
         Context ctx;
         std::copy(tokens.begin(), tokens.end(), std::back_inserter(ctx.tokens));

         auto machine = state_machine(ctx, FILTER);
         ctx.filters.push(ListFilter::create());
         machine.run_until_complete();

         if (ctx.filters.empty() || ctx.filters.top()->empty()) {
             THROW(CompilerError, "No filter result.");
         }

         auto joined_filter = JoinFilter::create();
         while (! ctx.filters.empty()) {
             joined_filter->push(ctx.filters.top());
             ctx.filters.pop();
         }

         return joined_filter->simplify();
     }

     Duration compile_duration(const std::vector<Grammar::Token>& tokens) const {
         Context ctx;

         auto machine = state_machine(ctx, DURATION);
         machine.run_until_complete();

         if (! ctx.duration.has_value()) {
             THROW(CompilerError, "No duration result.");
         }

         return ctx.duration.value();
     }

 private:
     moonlight::automata::Lambda<Context, State>::Machine state_machine(Context& ctx, State init_state) const {
         return moonlight::automata::Lambda<Context, State>::builder(ctx)
         .init(init_state)
         .state(FILTER, [&](auto& m) {
             if (ctx.at_end()) {
                 m.pop();
                 return;
             }

             auto token = ctx.front_token();

             switch (token.type()) {
             case TokenType::OP_RANGE:
                 ctx.new_filter();
                 ctx.pop_token();
                 m.push(FILTER_RANGE);
                 m.push(FILTER);
                 break;

             case TokenType::OP_DURATION:
                 ctx.pop_token();
                 m.push(DURATION_JOIN);
                 m.push(DURATION);
                 break;

             case TokenType::OP_JOIN:
                 ctx.new_filter();
                 ctx.pop_token();
                 break;

             default:
                 ctx.top_filter()->push(parse_filter_token(token));
                 ctx.pop_token();
                 break;
             }
         })
         .state(DURATION, [&](auto& m) {
             if (ctx.at_end()) {
                 m.pop();
                 return;
             }

             auto token = ctx.front_token();

             if (token.type() == TokenType::DURATION) {
                 const int64_t value = std::stoll(token.capture().group(1));
                 const std::string suffix = token.capture().group(2);
                 auto factory = parse_duration_factory(suffix);

                 if (ctx.duration.has_value()) {
                     ctx.duration.value() += factory(value);
                 } else {
                     ctx.duration = factory(value);
                 }
                 ctx.pop_token();
             }

             m.pop();
         })
         .state(DURATION_JOIN, [&](auto& m) {
             auto filter = ctx.pop_filter();
             ctx.new_filter();
             ctx.top_filter()->push(DurationFilter::create(filter, ctx.duration.value()));
             ctx.duration.reset();
             m.pop();
         })
         .state(FILTER_RANGE, [&](auto& m) {
             auto rhs = ctx.pop_filter();
             auto lhs = ctx.pop_filter();
             ctx.new_filter();
             ctx.top_filter()->push(RangeFilter::create(lhs, rhs));
             m.pop();
         })
         .build();
     }

     static std::function<Duration(int64_t)> parse_duration_factory(const std::string& suffix) {
         static std::map<std::string, std::function<Duration(int64_t)>> factories = {
             {"h", Duration::of_hours},
             {"min", Duration::of_minutes},
             {"m", Duration::of_minutes},
             {"sec", Duration::of_seconds},
             {"s", Duration::of_seconds},
             {"ms", [](const auto v) { return Duration(Millis(v)); }},
             {"w", [](const auto v) { return Duration::of_days(7 * v); }},
             {"d", Duration::of_days}
         };

         return factories.at(moonlight::str::to_lower(suffix));
     }

     Filter::Pointer parse_filter_token(const Token& tk) const {
         int day, month_num, year, hour, minute;
         Month month;
         Weekday weekday;
         std::set<Weekday> weekdays;
         std::string buffer;

         switch(tk.type()) {
         case TokenType::DAY_MONTH:
             day = std::stoi(tk.capture().group(1));
             month = _i18n.month(tk.capture().group(2));

             return ListFilter::create({
                 MonthdayFilter::create(day),
                 MonthFilter::create(month)
             });

         case TokenType::DAY_MONTH_YEAR:
             day = std::stoi(tk.capture().group(1));
             month = _i18n.month(tk.capture().group(2));
             year = std::stoi(tk.capture().group(3));

             return DateFilter::create(Date(year, month, day));

         case TokenType::DAY_OF_MONTH:
             day = std::stoi(tk.capture().group(1));
             return MonthdayFilter::create(day);

         case TokenType::H12_TIME:
             hour = std::stoi(tk.capture().group(1));
             minute = std::stoi(tk.capture().group(2));
             buffer = tk.capture().group(3);

             if (moonlight::str::to_lower(buffer)[0] == 'p') {
                 if (hour != 12) {
                     hour += 12;
                 }
             } else {
                 if (hour == 12) {
                     hour = 0;
                 }
             }

             return TimeFilter::create(Time(hour, minute));

         case TokenType::H24_TIME:
         case TokenType::MIL_TIME:
             hour = std::stoi(tk.capture().group(1));
             minute = std::stoi(tk.capture().group(2));

             return TimeFilter::create(Time(hour, minute));

         case TokenType::ISO_DATE:
             year = std::stoi(tk.capture().group(1));
             month_num = std::stoi(tk.capture().group(2));
             day = std::stoi(tk.capture().group(3));

             return DateFilter::create(Date(year, month_num, day));

         case TokenType::MONTH:
             month = _i18n.month(tk.capture().group(1));
             return MonthFilter::create(month);

         case TokenType::MONTH_DAY:
             month = _i18n.month(tk.capture().group(1));
             day = std::stoi(tk.capture().group(2));
             return ListFilter::create({
                 MonthFilter::create(month),
                 MonthdayFilter::create(day)
             });

         case TokenType::MONTH_DAY_YEAR:
             month = _i18n.month(tk.capture().group(1));
             day = std::stoi(tk.capture().group(2));
             year = std::stoi(tk.capture().group(3));

             return DateFilter::create(Date(year, month, day));

         case TokenType::MONTH_YEAR:
             month = _i18n.month(tk.capture().group(1));
             year = std::stoi(tk.capture().group(2));

             return ListFilter::create({
                 MonthFilter::create(month),
                 YearFilter::create(year)
             });

         case TokenType::US_DATE:
             month_num = std::stoi(tk.capture().group(1));
             day = std::stoi(tk.capture().group(2));
             year = std::stoi(tk.capture().group(3));

             return DateFilter::create(Date(year, month_num, day));

         case TokenType::WEEKDAY:
             weekday = _i18n.weekday(tk.capture().group(1));
             return WeekdayFilter::create(weekday);

         case TokenType::WEEKDAYS:
             buffer = moonlight::str::to_lower(tk.capture().group(0));
             for (auto c : buffer) {
                 weekdays.insert(static_cast<Weekday>(weekday_offsets().find(c)));
             }

             return WeekdayFilter::create(weekdays);

         case TokenType::WEEKDAY_MONTHDAY:
             weekday = _i18n.weekday(tk.capture().group(1));
             day = std::stoi(tk.capture().group(2));

             return ListFilter::create({
                 WeekdayFilter::create(weekday),
                 MonthdayFilter::create(day)
             });

         case TokenType::YEAR:
             year = std::stoi(tk.capture().group(0));
             return YearFilter::create(year);

         case TokenType::YEAR_MONTH:
             year = std::stoi(tk.capture().group(1));
             month = _i18n.month(tk.capture().group(2));

             return ListFilter::create({
                 YearFilter::create(year),
                 MonthFilter::create(month)
             });

         case TokenType::YEAR_MONTH_DAY:
             year = std::stoi(tk.capture().group(1));
             month = _i18n.month(tk.capture().group(2));
             day = std::stoi(tk.capture().group(3));

             return DateFilter::create(Date(year, month, day));

         default:
             THROW(CompilerError, tfm::format("Invalid filter token type: %s", token_type_name(tk.type())));
             break;
         }
     }

     static const std::string& weekday_offsets() {
         static const std::string offsets = "umtwhfs";
         return offsets;
     }

     const timefilter::I18nStrings _i18n;
};

// ------------------------------------------------------------------
inline Filter::Pointer compile_filter(const std::string& expr, const I18nStrings& i18n = I18nStrings::defaults()) {
    Parser parser(i18n);
    Compiler compiler(i18n);
    return compiler.compile_filter(parser.parse(expr));
}

// ------------------------------------------------------------------
inline Duration compile_duration(const std::string& expr, const I18nStrings& i18n = I18nStrings::defaults()) {
    Parser parser(i18n);
    Compiler compiler(i18n);
    return compiler.compile_duration(parser.parse(expr));
}

}  // namespace timefilter


#endif /* !__TIMEFILTER_COMPILER_H */
