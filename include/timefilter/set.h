/*
 * set.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#ifndef __TIMEFILTER_SET_H
#define __TIMEFILTER_SET_H

#include "timefilter/filter.h"
#include "timefilter/month.h"
#include "timefilter/monthday.h"
#include "timefilter/time.h"
#include "timefilter/weekday.h"
#include "timefilter/weekday_monthday.h"
#include "timefilter/year.h"
#include <stack>

namespace timefilter {

class FilterSet : public Filter {
 public:
     typedef std::shared_ptr<FilterSet> Pointer;

     FilterSet() : Filter(FilterType::FilterSet) { }
     FilterSet(Pointer set) : Filter(FilterType::FilterSet), _filters(set->_filters) { }

     static Pointer create() {
         return std::make_shared<FilterSet>();
     }

     static Pointer create(Pointer set) {
         return std::make_shared<FilterSet>(set);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         return _next_range(Range::eternity(), dt, get_filter_stack());
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         return _prev_range(Range::eternity(), dt, get_filter_stack());
     }

     bool empty() const {
         return _filters.empty();
     }

     size_t size() const {
         return _filters.size();
     }

     Pointer add(Filter::Pointer filter) {
         if (filter->type() == FilterType::FilterSet) {
             auto filter_set = std::static_pointer_cast<const FilterSet>(filter);
             for (auto set_filter : filter_set->_filters) {
                 add(set_filter);
             }
             return std::static_pointer_cast<FilterSet>(shared_from_this());
         }

         if (filter->is_relative()) {
             THROW(Error, "Sets cannot contain other relative filters: " + filter->type_name());;
         }

         if (filter->is_absolute() && absolute_filter().has_value()) {
             THROW(Error, "Sets cannot contain more than one absolute filter.  Set already contains "
                   + (*absolute_filter())->type_name() + ", cannot add "
                   + filter->type_name() + ".");
         }

         switch(filter->type()) {
         case FilterType::Datetime:
             THROW(Error, "Datetime filter is absolute and atomic, thus cannot be part of a filter set.");

         case FilterType::Month:
             ingest_month_filter(filter);
             break;

         case FilterType::Monthday:
             ingest_monthday_filter(filter);
             break;

         case FilterType::Time:
             ingest_time_filter(filter);
             break;

         case FilterType::Weekday:
             ingest_weekday_filter(filter);
             break;

         case FilterType::WeekdayMonthday:
             ingest_weekday_monthday_filter(filter);

         case FilterType::WeekdayOfMonth:
             ingest_weekday_of_month_filter(filter);
             break;

         default:
             if (filter->is_absolute()) {
                 _filters.push_back(filter);
                 break;
             }
             THROW(Error, "Filter set not prepared to handle filter: " + filter->type_name());
         }
         validate();

         return std::static_pointer_cast<FilterSet>(shared_from_this());
     }

     bool is_absolute() const override {
         for (auto filter : _filters) {
             if (! filter->is_absolute()) {
                 return false;
             }
         }

         return true;
     }

 protected:
     std::string _repr() const override {
         std::vector<std::string> reprs;
         std::transform(_filters.begin(), _filters.end(), std::back_inserter(reprs), [](auto filter) {
             return filter->repr();
         });
         return moonlight::str::join(reprs, ",");
     }

 private:
     void ingest_month_filter(Filter::Pointer filter) {
         std::shared_ptr<const MonthFilter> month_filter = static_pointer_cast<const MonthFilter>(filter);
         std::set<Month> months = month_filter->months();
         auto prev_filter = pop_filter(FilterType::Month);
         if (prev_filter.has_value()) {
             std::shared_ptr<const MonthFilter> prev_month_filter = static_pointer_cast<const MonthFilter>(prev_filter.value());
             std::copy(prev_month_filter->months().begin(),
                       prev_month_filter->months().end(),
                       std::inserter(months, months.begin()));
         }
         auto new_filter = MonthFilter::create(months);
         _filters.push_back(new_filter);
     }

     void ingest_monthday_filter(Filter::Pointer filter) {
         if (get_filter(FilterType::WeekdayOfMonth)) {
             THROW(Error, "Monthday is mutually exclusive with WeekdayOfMonth filters in sets.");
         }

         auto prev_filter = get_filter({FilterType::Weekday, FilterType::Monthday, FilterType::WeekdayMonthday});
         Filter::Pointer new_filter = filter;

         if (prev_filter.has_value()) {
             pop_filter(prev_filter.value()->type());
             auto monthday_filter = std::static_pointer_cast<const MonthdayFilter>(filter);
             std::set<int> monthdays = monthday_filter->days();

             switch(prev_filter.value()->type()) {
             case FilterType::Weekday: {
                 new_filter = WeekdayMonthdayFilter::create(std::static_pointer_cast<const WeekdayFilter>(prev_filter.value())->weekdays(), monthdays);
                 break;
             }

             case FilterType::Monthday: {
                 const auto& prev_monthdays = std::static_pointer_cast<const MonthdayFilter>(prev_filter.value())->days();
                 std::copy(prev_monthdays.begin(), prev_monthdays.end(), std::inserter(monthdays, monthdays.begin()));
                 new_filter = MonthdayFilter::create(monthdays);
                 break;
             }

             case FilterType::WeekdayMonthday: {
                 auto prev_wm_filter = std::static_pointer_cast<const WeekdayMonthdayFilter>(prev_filter.value());
                 std::copy(prev_wm_filter->monthdays().begin(), prev_wm_filter->monthdays().end(), std::inserter(monthdays, monthdays.begin()));
                 new_filter = WeekdayMonthdayFilter::create(prev_wm_filter->weekdays(), monthdays);
                 break;
             }

             default:
                 THROW(Error, "Unexpected filter type while combining Monthday filter into set: " + prev_filter.value()->type_name());
             }
         }

         _filters.push_back(new_filter);
     }

     void ingest_time_filter(Filter::Pointer filter) {
         std::shared_ptr<const TimeFilter> time_filter = static_pointer_cast<const TimeFilter>(filter);
         std::set<Time> times = time_filter->times();
         auto prev_filter = pop_filter(FilterType::Time);
         if (prev_filter.has_value()) {
             std::shared_ptr<const TimeFilter> prev_time_filter = static_pointer_cast<const TimeFilter>(prev_filter.value());
             std::copy(prev_time_filter->times().begin(),
                       prev_time_filter->times().end(),
                       std::inserter(times, times.begin()));
         }
         auto new_filter = TimeFilter::create(times);
         _filters.push_back(new_filter);
     }

     void ingest_weekday_filter(Filter::Pointer filter) {
         if (get_filter(FilterType::WeekdayOfMonth)) {
             THROW(Error, "Weekday and WeekdayOfMonth filters are mutually exclusive in sets.");
         }

         auto prev_filter = get_filter({FilterType::Weekday, FilterType::Monthday, FilterType::WeekdayMonthday});
         Filter::Pointer new_filter = filter;

         if (prev_filter.has_value()) {
             pop_filter(prev_filter.value()->type());
             auto weekday_filter = std::static_pointer_cast<const WeekdayFilter>(filter);
             std::set<Weekday> weekdays = weekday_filter->weekdays();

             switch(prev_filter.value()->type()) {
             case FilterType::Weekday: {
                 const auto& prev_weekdays = std::static_pointer_cast<const WeekdayFilter>(prev_filter.value())->weekdays();
                 std::copy(prev_weekdays.begin(), prev_weekdays.end(), std::inserter(weekdays, weekdays.begin()));
                 new_filter = WeekdayFilter::create(weekdays);
                 break;
             }

             case FilterType::Monthday: {
                 new_filter = WeekdayMonthdayFilter::create(weekdays, std::static_pointer_cast<const MonthdayFilter>(prev_filter.value())->days());
                 break;
             }

             case FilterType::WeekdayMonthday: {
                 auto prev_wm_filter = std::static_pointer_cast<const WeekdayMonthdayFilter>(prev_filter.value());
                 std::copy(prev_wm_filter->weekdays().begin(), prev_wm_filter->weekdays().end(), std::inserter(weekdays, weekdays.begin()));
                 new_filter = WeekdayMonthdayFilter::create(weekdays, prev_wm_filter->monthdays());
                 break;
             }

             default:
                 THROW(Error, "Unexpected filter type while combining Weekday filter into set: " + prev_filter.value()->type_name());
             }
         }

         _filters.push_back(new_filter);
     }

     void ingest_weekday_monthday_filter(Filter::Pointer filter) {
         if (get_filter(FilterType::WeekdayOfMonth)) {
             THROW(Error, "WeekdayMonthday and WeekdayOfMonth filters are mutually exclusive in sets.");
         }

         auto prev_filter = get_filter({FilterType::Weekday, FilterType::Monthday, FilterType::WeekdayMonthday});
         Filter::Pointer new_filter = filter;

         if (prev_filter.has_value()) {
             pop_filter(prev_filter.value()->type());
             auto wm_filter = std::static_pointer_cast<const WeekdayMonthdayFilter>(filter);
             std::set<Weekday> weekdays = wm_filter->weekdays();
             std::set<int> monthdays = wm_filter->monthdays();

             switch(prev_filter.value()->type()) {
             case FilterType::Weekday: {
                 const auto& prev_weekdays = std::static_pointer_cast<const WeekdayFilter>(prev_filter.value())->weekdays();
                 std::copy(prev_weekdays.begin(), prev_weekdays.end(), std::inserter(weekdays, weekdays.begin()));
                 new_filter = WeekdayMonthdayFilter::create(weekdays, monthdays);
                 break;
             }

             case FilterType::Monthday: {
                 const auto& prev_monthdays = std::static_pointer_cast<const MonthdayFilter>(prev_filter.value())->days();
                 std::copy(prev_monthdays.begin(), prev_monthdays.end(), std::inserter(monthdays, monthdays.begin()));
                 new_filter = WeekdayMonthdayFilter::create(weekdays, monthdays);
                 break;
             }

             case FilterType::WeekdayMonthday: {
                 auto prev_wm_filter = std::static_pointer_cast<const WeekdayMonthdayFilter>(prev_filter.value());
                 std::copy(prev_wm_filter->weekdays().begin(), prev_wm_filter->weekdays().end(), std::inserter(weekdays, weekdays.begin()));
                 std::copy(prev_wm_filter->monthdays().begin(), prev_wm_filter->monthdays().end(), std::inserter(monthdays, monthdays.begin()));
                 new_filter = WeekdayMonthdayFilter::create(weekdays, monthdays);
                 break;
             }

             default:
                 THROW(Error, "Unexpected filter type while combining WeekdayMonthday filter into set: " + prev_filter.value()->type_name());
             }
         }

         _filters.push_back(new_filter);
     }

     void ingest_weekday_of_month_filter(Filter::Pointer filter) {
         if (get_filter({FilterType::Weekday, FilterType::Monthday, FilterType::WeekdayMonthday})) {
             THROW(Error, "WeekdayOfMonth is mutually exclusive with Weekday and Monthday filters in sets.");
         }

         if (get_filter(FilterType::WeekdayOfMonth)) {
             THROW(Error, "Multiple WeekdayOfMonth filters cannot be combined in sets.");
         }

         _filters.push_back(filter);
     }

     std::optional<Filter::Pointer> absolute_filter() const {
         for (auto filter : _filters) {
             if (filter->is_absolute()) {
                 return filter;
             }
         }

         return {};
     }

     std::optional<Filter::Pointer> get_filter(FilterType type) const {
         return get_filter(std::set{type});
     }

     std::optional<Filter::Pointer> get_filter(const std::set<FilterType>& types) const {
         auto iter = _filters.begin();

         for (;iter != _filters.end(); iter++) {
             auto filter = *iter;
             if (types.contains(filter->type())) {
                 return filter;
             }
         }

         return {};
     }

     std::optional<Filter::Pointer> pop_filter(FilterType type) {
         auto iter = _filters.begin();

         for (;iter != _filters.end(); iter++) {
             auto filter = *iter;
             if (filter->type() == type) {
                 break;
             }
         }

         if (iter != _filters.end()) {
             auto filter = *iter;
             _filters.erase(iter);
             return filter;
         }

         return {};
     }

     std::stack<Filter::Pointer> get_filter_stack() const {
         std::stack<Filter::Pointer> stack;
         std::vector<Filter::Pointer> filters;

         auto time_filter_box = get_filter(FilterType::Time);
         if (time_filter_box.has_value()) {
             stack.push(time_filter_box.value());
             filters.push_back(time_filter_box.value());
         }

         auto day_filter_box = get_filter({FilterType::Monthday, FilterType::Weekday, FilterType::WeekdayMonthday, FilterType::WeekdayOfMonth});

         if (day_filter_box.has_value()) {
             stack.push(day_filter_box.value());
             filters.push_back(day_filter_box.value());
         }

         auto month_filter_box = get_filter(FilterType::Month);
         if (month_filter_box.has_value()) {
             stack.push(month_filter_box.value());
             filters.push_back(month_filter_box.value());
         }

         auto abs_filter_box = absolute_filter();
         if (abs_filter_box.has_value()) {
             stack.push(abs_filter_box.value());
             filters.push_back(abs_filter_box.value());
         }

         return stack;
     }

     void validate() const {
         auto monthday_filter_box = get_filter(FilterType::Monthday);
         if (! monthday_filter_box.has_value()) {
             return;
         }

         auto month_filter_box = get_filter(FilterType::Month);
         if (! month_filter_box.has_value()) {
             return;
         }

         auto monthday_filter = std::static_pointer_cast<const MonthdayFilter>(monthday_filter_box.value());
         auto month_filter = std::static_pointer_cast<const MonthFilter>(month_filter_box.value());

         bool monthdays_reachable_normally = false;

         for (auto monthday : monthday_filter->days()) {
             for (auto month : month_filter->months()) {
                 if (last_day_of_month(2000 /* leap year */, month) >= std::abs(monthday)) {
                     monthdays_reachable_normally = true;
                     break;
                 }
             }
             if (monthdays_reachable_normally) {
                 break;
             }
         }

         if (! monthdays_reachable_normally) {
             THROW(Error, "None of the monthdays provided ever occur in the given months.");
         }

         auto year_filter_box = get_filter(FilterType::Year);

         if (! year_filter_box.has_value()) {
             return;
         }

         auto year_filter = static_pointer_cast<const YearFilter>(year_filter_box.value());

         if (is_leap_year(year_filter->year())) {
             return;
         }

         if (month_filter->months().size() == 1 &&
             month_filter->months().contains(Month::February)) {
             auto days = monthday_filter->days();

             if ((days.size() == 1 && (days.contains(29) || days.contains(-29))) ||
                 (days.size() == 2 && (days.contains(29) && days.contains(-29)))) {
                 THROW(Error, "Year " + std::to_string(year_filter->year()) + "is not a leap year.");
             }
         }
     }

     static std::optional<Range> _next_range(const Range& abs_range, const Datetime& dt, std::stack<Filter::Pointer> stack) {
         if (stack.empty()) {
             return {};
         }

         auto filter = stack.top();
         stack.pop();

         auto next_rg = filter->next_range(dt);

         if (next_rg.has_value()) {
             if (! next_rg->intersects(abs_range)) {
                 return {};
             }

             if (! stack.empty()) {
                 auto inner_rg = _next_range(next_rg.value(), next_rg->start(), stack);

                 if (inner_rg.has_value() && inner_rg->intersects(next_rg.value())) {
                     next_rg = inner_rg->clip_to(next_rg.value());

                 } else {
                     stack.push(filter);
                     next_rg = _next_range(abs_range, next_rg->start(), stack);
                 }
             }
         }

         if (! next_rg.has_value() && ! stack.empty()) {
             auto prev_rg = filter->prev_range(dt);
             if (prev_rg.has_value() && prev_rg->contains(dt)) {
                 next_rg = _next_range(prev_rg.value(), dt, stack);
                 if (next_rg.has_value() && prev_rg->contains(next_rg->start())) {
                     next_rg = next_rg->clip_to(prev_rg.value());
                 } else {
                     next_rg = {};
                 }
             }
         }

         if (next_rg.has_value()) {
             next_rg = next_rg->clip_to(abs_range);
         }

         return next_rg;
     }

     static std::optional<Range> _prev_range(const Range& abs_range, const Datetime& dt, std::stack<Filter::Pointer> stack) {
         if (stack.empty()) {
             return {};
         }

         auto filter = stack.top();
         stack.pop();

         auto prev_rg = filter->prev_range(dt);

         if (prev_rg.has_value()) {
             if (! prev_rg->intersects(abs_range)) {
                 return {};
             }

             if (! stack.empty()) {
                 auto inner_rg = _prev_range(prev_rg.value(), prev_rg->end(), stack);

                 if (inner_rg.has_value() && prev_rg->intersects(inner_rg.value())) {
                     prev_rg = inner_rg->clip_to(prev_rg.value());

                 } else {
                     stack.push(filter);
                     prev_rg = _prev_range(abs_range, prev_rg->start() - Duration::of_seconds(1), stack);
                 }
             }
         }

         if (prev_rg.has_value()) {
             prev_rg = prev_rg->clip_to(abs_range);
         }

         return prev_rg;
     }

     std::vector<Filter::Pointer> _filters;
};

}
#endif /* !__TIMEFILTER_SET_H */
