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
#include "timefilter/filter.h"
#include "timefilter/list.h"
#include "timefilter/parser.h"
#include "timefilter/relative_range.h"
#include "timefilter/set.h"
#include "moonlight/string.h"

namespace timefilter {

// ------------------------------------------------------------------
EXCEPTION_SUBTYPE(Error, CompilerError);
#define THROW_COMPILE(msg, token) THROW(CompilerError, std::string(msg) + " @ " + token.repr())

// ------------------------------------------------------------------
class Compiler {
 public:
     Compiler(const I18nStrings& i18n = I18nStrings::defaults()) : _i18n(i18n) { }

     enum State {
         INIT,
         DURATION,
         DURATION_JOIN,
         FILTER,
         FILTER_AT,
         FILTER_RANGE
     };

     struct Context {
         FilterList::Pointer list = FilterList::create();
         FilterSet::Pointer set = nullptr;
         std::deque<Grammar::Token> tokens;
         std::optional<Duration> duration;
         Token last_token = Token(TokenType::COMMENT, moonlight::rx::Capture());

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
             last_token = front_token();
             tokens.pop_front();
             return last_token;
         }
     };

     Filter::Pointer compile_filter(const std::vector<Grammar::Token>& tokens) const {
         Context ctx;
         std::copy(tokens.begin(), tokens.end(), std::back_inserter(ctx.tokens));

         auto machine = state_machine(ctx, FILTER);
         ctx.set = FilterSet::create();
         machine.run_until_complete();

         if (! ctx.set->empty()) {
             ctx.list->push(ctx.set);
         }

         return ctx.list->simplify();
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
                 ctx.pop_token();
                 if (ctx.set->empty()) {
                     THROW_COMPILE("Empty filter set is invalid for left-hand size of range.", token);
                 }

                 ctx.list->push(ctx.set);
                 ctx.set = FilterSet::create();
                 m.push(FILTER_RANGE);
                 m.push(FILTER);
                 break;

             case TokenType::OP_AT: {
                 auto stack_trace = m.stack_trace();
                 if (stack_trace.size() > 2 && stack_trace[stack_trace.size() - 2] == FILTER_RANGE) {
                     m.pop();
                     return;
                 }

                 ctx.pop_token();
                 if (! ctx.set->empty()) {
                     ctx.list->push(ctx.set);
                     ctx.set = FilterSet::create();
                 }

                 m.push(FILTER_AT);
                 m.push(FILTER);
                 break;
             }
             case TokenType::OP_DURATION:
                 ctx.pop_token();
                 m.push(DURATION_JOIN);
                 m.push(DURATION);
                 break;

             case TokenType::OP_JOIN:
                 if (ctx.set->empty()) {
                     THROW_COMPILE("Empty filter set is invalid.", ctx.last_token);
                 }

                 ctx.list->push(ctx.set);
                 ctx.set = FilterSet::create();
                 break;

             case TokenType::COMMENT:
                 ctx.pop_token();
                 break;

             default:
                 ctx.set->add(parse_filter_token(token));
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

             } else {
                 m.pop();
             }
         })
         .state(DURATION_JOIN, [&](auto& m) {
             if (! ctx.duration.has_value()) {
                 THROW_COMPILE("No duration provided.", ctx.last_token);
             }
             ctx.list->push(FilterDuration::create(ctx.set, ctx.duration.value()));
             ctx.duration.reset();
             ctx.set = FilterSet::create();
             m.pop();
         })
         .state(FILTER_AT, [&](auto& m) {
             std::vector<Filter::Pointer> filters;

             if (ctx.list->empty()) {
                 THROW(CompilerError, "Empty list is invalid for left-hand size of set-joiner.");
             }

             while (! ctx.list->empty()) {
                 auto top_filter = ctx.list->pop();

                 switch (top_filter->type()) {
                 case FilterType::FilterSet: {
                     auto filter_set = std::static_pointer_cast<FilterSet>(std::const_pointer_cast<Filter>(top_filter));
                     filter_set->add(ctx.set);
                     filters.push_back(filter_set);
                     break;
                 }
                 case FilterType::RelativeRange: {
                     auto range = std::static_pointer_cast<const RelativeRangeFilter>(top_filter);
                     if (range->start_filter()->type() != FilterType::FilterSet) {
                         THROW(CompilerError, "Unexpected start filter in RelativeRangeFilter while merging sets: " + range->start_filter()->repr());
                     }
                     auto filter_set = std::static_pointer_cast<FilterSet>(std::const_pointer_cast<Filter>(range->start_filter()));
                     filter_set->add(ctx.set);
                     filters.push_back(RelativeRangeFilter::create(filter_set, range->end_filter()));
                     break;
                 }
                 case FilterType::Duration: {
                     auto duration = std::static_pointer_cast<const FilterDuration>(top_filter);
                     if (duration->filter()->type() != FilterType::FilterSet) {
                         THROW(CompilerError, "Unexpected filter in FilterDuration while merging sets: " + duration->filter()->repr());
                     }
                     auto filter_set = std::static_pointer_cast<FilterSet>(std::const_pointer_cast<Filter>(duration->filter()));
                     filter_set->add(ctx.set);
                     filters.push_back(FilterDuration::create(filter_set, duration->duration()));
                     break;
                 }
                 default:
                     THROW(CompilerError, "Unexpected filter type in context filter list: " + top_filter->repr());
                 }
             }

             for (auto filter : filters) {
                 ctx.list->push(filter);
             }

             ctx.set = FilterSet::create();
             m.pop();
         })
         .state(FILTER_RANGE, [&](auto& m) {
             if (ctx.set->empty()) {
                 THROW_COMPILE("Empty right-hand set is invalid for filter range.", ctx.last_token);
             }

             auto rhs = ctx.list->pop();
             ctx.list->push(RelativeRangeFilter::create(rhs, ctx.set));
             ctx.set = FilterSet::create();
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
         int factor = 1;

         switch(tk.type()) {
         case TokenType::DAY_MONTH:
             day = std::stoi(tk.capture().group(1));
             month = _i18n.month(tk.capture().group(3));

             if (tk.capture().group(2).size() > 0) {
                 factor = -1;
             }

             return FilterSet::create()
                ->add(MonthdayFilter::create(day * factor))
                ->add(MonthFilter::create(month));

         case TokenType::DAY_MONTH_YEAR:
             day = std::stoi(tk.capture().group(1));
             month = _i18n.month(tk.capture().group(2));
             year = std::stoi(tk.capture().group(3));

             return DateFilter::create(Date(year, month, day));

         case TokenType::DAY_OF_MONTH:
             day = std::stoi(tk.capture().group(1));
             if (tk.capture().group(2).size() > 0) {
                 factor = -1;
             }
             return MonthdayFilter::create(day * factor);

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
             if (tk.capture().group(3).size() > 0) {
                 factor = -1;
             }
             return FilterSet::create()
                ->add(MonthFilter::create(month))
                ->add(MonthdayFilter::create(day * factor));

         case TokenType::MONTH_DAY_YEAR:
             month = _i18n.month(tk.capture().group(1));
             day = std::stoi(tk.capture().group(2));
             year = std::stoi(tk.capture().group(3));

             return DateFilter::create(Date(year, month, day));

         case TokenType::MONTH_YEAR:
             month = _i18n.month(tk.capture().group(1));
             year = std::stoi(tk.capture().group(2));

             return FilterSet::create()
                ->add(MonthFilter::create(month))
                ->add(YearFilter::create(year));

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
             if (tk.capture().group(3).size() > 0) {
                 factor = -1;
             }

             return FilterSet::create()
                ->add(WeekdayFilter::create(weekday))
                ->add(MonthdayFilter::create(day * factor));

         case TokenType::YEAR:
             year = std::stoi(tk.capture().group(0));
             return YearFilter::create(year);

         case TokenType::YEAR_MONTH:
             year = std::stoi(tk.capture().group(1));
             month = _i18n.month(tk.capture().group(2));

             return FilterSet::create()
                ->add(YearFilter::create(year))
                ->add(MonthFilter::create(month));

         case TokenType::YEAR_MONTH_DAY:
             year = std::stoi(tk.capture().group(1));
             month = _i18n.month(tk.capture().group(2));
             day = std::stoi(tk.capture().group(3));

             return DateFilter::create(Date(year, month, day));

         default:
             THROW(CompilerError, "Invalid filter token type: " + token_type_name(tk.type()));
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
