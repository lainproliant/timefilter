/*
 * list.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday January 6, 2021
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_LIST_H
#define __TIMEFILTER_LIST_H

#include <set>
#include <memory>
#include <string>
#include <algorithm>
#include <vector>
#include "moonlight/collect.h"
#include "timefilter/core.h"
#include "timefilter/month.h"
#include "timefilter/monthday.h"
#include "timefilter/time.h"
#include "timefilter/weekday.h"
#include "timefilter/year.h"

namespace timefilter {

// ------------------------------------------------------------------
EXCEPTION_SUBTYPE(Error, ListFilterValidationError);
typedef std::vector<Filter::Pointer>::const_iterator ListIterator;

// ------------------------------------------------------------------
class StackViewFilter : public Filter {
 public:
     StackViewFilter(ListIterator iter, ListIterator end) : Filter(FilterType::List), _iter(iter), _end(end) { }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         if (empty()) return {};

         auto next = cdr();
         auto dt = pivot;

         for (;;) {
             auto back_range = car()->prev_range(dt);

             if (back_range.has_value() && back_range->contains(dt)) {
                 if (next.empty() && back_range == Range::for_days(dt.date(), 1) && back_range->start() >= pivot) {
                     return back_range;

                 } else {
                     auto step_range = next.next_range(dt);
                     if (step_range.has_value() && back_range->contains(step_range->start())) {
                         return step_range;
                     }
                 }
             }

             auto range = car()->next_range(dt);
             if (range.has_value()) {
                 if (! next.empty()) {
                     auto step_range = next.next_range(range->start());
                     if (step_range.has_value() && range->contains(step_range->start())) {
                         return step_range;
                     }

                 } else {
                     return range;
                 }

                 dt = range->end();

             } else {
                 return {};
             }
         }
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         if (empty()) return {};

         auto next = cdr();
         auto date = pivot;
         std::optional<Range> result;

         for (;;) {
             auto range = car()->prev_range(date);

             if (range.has_value()) {
                 if (! next.empty()) {
                     auto step_date = std::min(range->end(), date);
                     auto step_range = next.prev_range(step_date);
                     if (step_range.has_value() && range->contains(step_range->start())) {
                         result = step_range;
                         break;
                     }

                 } else {
                     result = range;
                     break;
                 }

                 date = range->start() - minutes(1);

             } else {
                 result = {};
                 break;
             }
         }

         return result;
     }

     bool empty() const {
         return _iter == _end;
     }

     const Filter::Pointer car() const {
         return *_iter;
     }

     StackViewFilter cdr() const {
         return StackViewFilter(std::next(_iter), _end);
     }

 protected:
     std::string _repr() const override {
         std::vector<std::string> reprs;
         std::transform(_iter, _end, std::back_inserter(reprs), [](auto p) {
             return p->repr();
         });
         return moonlight::str::join(reprs, ", ");
     }

 private:
     ListIterator _iter;
     ListIterator _end;
};

// ------------------------------------------------------------------
class ListFilter : public Filter {
 public:
     typedef std::shared_ptr<const ListFilter> ConstPointer;
     typedef std::shared_ptr<ListFilter> Pointer;

     ListFilter() : Filter(FilterType::List) { }

     static Pointer create() {
         return std::make_shared<ListFilter>();
     }

     template<class C = std::initializer_list<std::shared_ptr<const Filter>>>
     static std::shared_ptr<ListFilter> create(const C& stack) {
         std::vector<Filter::Pointer> vec;
         for (auto filter : stack) {
             vec.push_back(filter);
         }
         return std::make_shared<ListFilter>(stack);
     }

     explicit ListFilter(const std::vector<Filter::Pointer> stack)
     : Filter(FilterType::List), _stack(sort_and_validate(stack)) { }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         return view().next_range(pivot);
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         return view().prev_range(pivot);
     }

     ListFilter& push(Filter::Pointer filter) {
         std::vector<Filter::Pointer> filters(_stack);
         if (filter->type() == FilterType::List) {
             auto list = static_pointer_cast<const ListFilter>(filter);
             std::copy(list->_stack.begin(), list->_stack.end(), std::back_inserter(filters));

         } else {
             filters.push_back(filter);
         }
         _stack = sort_and_validate(filters);
         return *this;
     }

     bool empty() const {
         return _stack.empty();
     }

 protected:
     std::string _repr() const override {
         std::vector<std::string> reprs;
         std::transform(_stack.begin(), _stack.end(), std::back_inserter(reprs), [](auto p) {
             return p->repr();
         });
         return moonlight::str::join(reprs, ", ");
     }

 private:
     static std::vector<Filter::Pointer> sort_and_validate(const std::vector<Filter::Pointer>& stack) {
         std::set<int> orders;

         auto combined_stack = combine_filters(stack);

         auto sorted = moonlight::collect::sorted(
             combined_stack,
             [](const auto& filterA, const auto& filterB) {
                 if (filterA->order_int() < 0 || filterB->order_int() < 0) {
                     throw ListFilterValidationError("Unorderable filter can't be added to a list.");
                 }
                 return filterA->order() < filterB->order();
             });
         for (auto filter : sorted) {
             if (orders.find(filter->order_int()) == orders.end()) {
                 orders.insert(filter->order_int());
             } else {
                 throw ListFilterValidationError(tfm::format("Multiple non-combinable filters with the same order %s provided.",
                                                             filter_order_name(filter->order())));

             }
         }

         verify_reachable_list(sorted);

         return sorted;
     }

     static void verify_reachable_list(const std::vector<Filter::Pointer>& stack) {
         std::set<int> years;
         std::set<Month> months;
         std::set<int> monthdays;

         for (auto filter : stack) {
             if (filter->type() == FilterType::Year) {
                 auto year_filter = static_pointer_cast<const YearFilter>(filter);
                 years.insert(year_filter->year());

             } else if (filter->type() == FilterType::Month) {
                 auto month_filter = static_pointer_cast<const MonthFilter>(filter);
                 std::copy(month_filter->months().begin(), month_filter->months().end(),
                           std::inserter(months, months.begin()));

             } else if (filter->type() == FilterType::Monthday) {
                 auto monthday_filter = static_pointer_cast<const MonthdayFilter>(filter);
                 std::copy(monthday_filter->monthdays().begin(), monthday_filter->monthdays().end(),
                           std::inserter(monthdays, monthdays.begin()));
             }
         }

         /*
          * The list is unreachable IFF ->
          *     - The only monthdays provided never occur in the only months provided, OR
          *     - IFF ->
          *         - The only monthday provided is 29, AND
          *         - The only month provided is February, AND
          *         - The only years provided are NOT leap years
          */
         if (monthdays.empty()) {
             return;
         }

         if (months.empty()) {
             return;
         }

         bool monthday_normal_reachable = false;

         for (auto monthday : monthdays) {
             for (auto month : months) {
                 if (last_day_of_month(2000 /* a leap year */, month) >= std::abs(monthday)) {
                     monthday_normal_reachable = true;
                     break;
                 }
             }
             if (monthday_normal_reachable) break;
         }

         if (! monthday_normal_reachable) {
             THROW(ListFilterValidationError, "None of the monthdays provided ever occur in the given months.");
         }

         if (years.empty()) {
             return;
         }

         if (months.size() == 1 && months.contains(Month::February) &&
             ((monthdays.size() == 1 && (monthdays.contains(29) ||
                                         monthdays.contains(-29))) ||
              (monthdays.size() == 2 && (monthdays.contains(29) &&
                                         monthdays.contains(-29))))) {

             for (auto year : years) {
                 if (is_leap_year(year)) {
                     return;
                 }
             }

             THROW(ListFilterValidationError, "The only days provided are Feb 29 (+/-) and none of the years provided are leap years.");
         }
     }

     static std::vector<Filter::Pointer> combine_filters(const std::vector<Filter::Pointer>& stack) {
         std::set<Month> months;
         std::set<int> monthdays;
         std::set<Time> times;
         std::set<Weekday> weekdays;
         std::vector<Filter::Pointer> output_filters;

         for (auto filter : stack) {
             switch(filter->type()) {
             case FilterType::Month: {
                auto month_filter = static_pointer_cast<const MonthFilter>(filter);
                std::copy(month_filter->months().begin(), month_filter->months().end(),
                          std::inserter(months, months.begin()));
                break;
             }
             case FilterType::Monthday: {
                auto monthday_filter = static_pointer_cast<const MonthdayFilter>(filter);
                std::copy(monthday_filter->monthdays().begin(), monthday_filter->monthdays().end(),
                          std::inserter(monthdays, monthdays.begin()));
                break;
             }
             case FilterType::Time: {
                auto time_filter = static_pointer_cast<const TimeFilter>(filter);
                std::copy(time_filter->times().begin(), time_filter->times().end(),
                          std::inserter(times, times.begin()));
                break;

             }
             case FilterType::Weekday: {
                auto weekday_filter = static_pointer_cast<const WeekdayFilter>(filter);
                std::copy(weekday_filter->weekdays().begin(), weekday_filter->weekdays().end(),
                          std::inserter(weekdays, weekdays.begin()));
                break;

             }
             default:
                 output_filters.push_back(filter);
             }
         }

         if (! months.empty()) {
             output_filters.push_back(MonthFilter::create(months));
         }

         if (! monthdays.empty()) {
             output_filters.push_back(MonthdayFilter::create(monthdays));
         }

         if (! times.empty()) {
             output_filters.push_back(TimeFilter::create(times));
         }

         if (! weekdays.empty()) {
             output_filters.push_back(WeekdayFilter::create(weekdays));
         }

         return output_filters;
     }

     StackViewFilter view() const {
         return StackViewFilter(_stack.begin(), _stack.end());
     }

     bool is_discrete() const override {
         for (auto filter : _stack) {
             if (filter->is_discrete()) {
                 return true;
             }
         }

         return false;
     }

     Filter::Pointer simplify() const override {
         if (_stack.size() == 0) {
             return shared_from_this();
         }

         if (_stack.size() == 1) {
             return _stack.at(0)->simplify();
         }

         auto list_filter = ListFilter::create();

         for (auto filter : _stack) {
             list_filter->push(filter->simplify());
         }

         return list_filter;
     }

     std::vector<Filter::Pointer> _stack;
};

}  // namespace timefilter

#endif /* !__TIMEFILTER_LIST_H */
