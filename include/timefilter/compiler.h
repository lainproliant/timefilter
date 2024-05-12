/*
 * compiler.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday February 21, 2024
 */

#ifndef __TIMEFILTER_COMPILER_H
#define __TIMEFILTER_COMPILER_H

#include "timefilter/parser.h"
#include "moonlight/string.h"

namespace timefilter {

// ------------------------------------------------------------------
class Compiler {
 public:
     Compiler(const I18nStrings& i18n) : _i18n(i18n) { }

     enum State {
         INIT,
         DURATION,
         DURATION_JOIN,
         FILTER,
         FILTER_JOIN,
         FILTER_RANGE,
     };

     struct Context {
         std::stack<ListFilter::Pointer> filters;
         std::deque<Grammar::Token> tokens;
         std::optional<Duration> duration;

         ListFilter::Pointer filter() const {
             if (filters.empty()) {
                 return nullptr;
             }
             return filters.top();
         }

         size_t filter_depth() const {
             return filters.size();
         }

         void push_new_filter() {
             filters.push(ListFilter::create());
         }

         ListFilter::Pointer pop_filter() {
             auto value = filter();
             filters.pop();
             return value;
         }

         bool at_end() const {
             return tokens.empty();
         }

         void check_binop_pre(TokenType type) {
             if (filter_depth() != 1) {
                 THROW(CompilerError, tfm::format("Unexpected depth=%d for left hand side of %s.",
                                                  filter_depth(), token_type_name(type)));
             }

             if (filter() == nullptr || filter()->empty()) {
                 THROW(CompilerError, tfm::format("Left hand side of %s is an empty filter.", token_type_name(type)));
             }
         }

         void check_binop_post(TokenType type) {
             if (filter_depth() != 2) {
                 THROW(CompilerError, tfm::format("Unexpected depth=%d for right hand side of %s.",
                                                  filter_depth(), token_type_name(type)));
             }

             if (filter() == nullptr || filter()->empty()) {
                 THROW(CompilerError, tfm::format("Right hand side of %s is an empty filter.", token_type_name(type)));
             }
         }

         const Grammar::Token& token() const {
             if (tokens.empty()) {
                 THROW(CompilerError, "Unexpected end of expression.");
             }
             return tokens.front();
         }

         Grammar::Token pop_token() {
             auto tk = token();
             tokens.pop_front();
             return tk;
         }
     };

     Filter::Pointer compile_filter(const std::vector<Grammar::Token>& tokens) const {
         Context ctx;
         std::copy(tokens.begin(), tokens.end(), std::back_inserter(ctx.tokens));

         auto machine = state_machine(ctx, FILTER);
         machine.run_until_complete();

         if (ctx.filter() == nullptr || ctx.filter()->empty()) {
             THROW(CompilerError, "No filter result.");
         }

         return ctx.filter();
     }

     Duration compile_duration(const std::vector<Grammar::Token>& tokens) const {
         Context ctx = duration_context(tokens);

         auto machine = state_machine(ctx, DURATION);
         machine.run_until_complete();

         if (! ctx.duration.has_value()) {
             THROW(CompilerError, "No duration result.");
         }

         return ctx.duration.value();
     }

 private:
     Context filter_context(const std::vector<Grammar::Token>& tokens) const {
         Context ctx;
         std::copy(tokens.begin(), tokens.end(), std::back_inserter(ctx.tokens));
         ctx.push_new_filter();
         return ctx;
     }

     Context duration_context(const std::vector<Grammar::Token>& tokens) const {
         Context ctx;
         ctx.duration = Duration::zero();
         return ctx;
     }

     moonlight::automata::Lambda<Context, State>::Machine state_machine(Context& ctx, State init_state) const {
         return moonlight::automata::Lambda<Context, State>::builder(ctx)
         .init(init_state)
         .state(FILTER, [&](auto& m) {
             if (ctx.at_end()) {
                 m.pop();
                 return;
             }

             auto token = ctx.token();

             switch (token.type()) {
             case TokenType::OP_RANGE:
                 ctx.check_binop_pre(token.type());
                 ctx.push_new_filter();
                 ctx.pop_token();
                 m.push(FILTER_RANGE);
                 m.push(FILTER);
                 break;

             case TokenType::OP_DURATION:
                 ctx.check_binop_pre(token.type());
                 ctx.push_new_filter();
                 ctx.pop_token();
                 ctx.duration = Duration::zero();
                 m.push(DURATION_JOIN);
                 m.push(DURATION);
                 break;

             case TokenType::OP_JOIN:
                 ctx.check_binop_pre(token.type());
                 ctx.push_new_filter();
                 ctx.pop_token();
                 m.push(FILTER_JOIN);
                 m.push(FILTER);
                 break;

             default:
                 ctx.filter()->push(parse_filter_token(token));
                 ctx.pop_token();
                 break;
             }
         })
         .state(DURATION, [&](auto& m) {
             if (ctx.at_end()) {
                 m.pop();
                 return;
             }

             auto token = ctx.token();

             if (token.type() == TokenType::DURATION) {
                 const int64_t value = std::stoll(token.capture().group(1));
                 const std::string code = token.capture().group(2);
                 auto factory = _duration_meta(code);
                 ctx.duration += factory(value);
             }
         })
         .state(DURATION_JOIN, [&](auto& m) {

         })
         .state(FILTER_JOIN, [&](auto& m) {

         })
         .state(FILTER_RANGE, [&](auto& m) {

         })
         .build();
     }


     static DurationMetaFactory make_duration_meta_factory() {
         DurationMetaFactory factory;

         factory["h", "hr"] = Duration::of_hours;
         factory["min", "m"] = Duration::of_minutes;
         factory["sec", "s"] = Duration::of_seconds;
         factory["ms"] = [](int millis) { return Duration(Millis(millis)); };
         factory["w"] = [](int weeks) { return Duration::of_days(weeks * 7); };
         factory["d"] = Duration::of_days;

         return factory;
     }

     static std::function<Duration(int64_t)> parse_duration_factory(const std::string& suffix) {

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

             return DatetimeFilter::create(Datetime(Date(year, month, day), Time::start_of_day()));

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

             return DateFilter::create(Date(year, month, day));

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
