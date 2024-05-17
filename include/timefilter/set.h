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
#include "timefilter/year.h"
#include "timefilter/weekday_of_month.h"

namespace timefilter {

class FilterSet : public Filter {
 public:
     typedef std::shared_ptr<FilterSet> Pointer;

     FilterSet() : Filter(FilterType::FilterSet) { }

     static Pointer create() {
         return std::make_shared<FilterSet>();
     }

     Pointer add(Filter::Pointer filter) {
         if (filter->is_absolute() && absolute_filter().has_value()) {
             THROW(Error, "Sets cannot contain more than one absolute filter.  Set already contains "
                   + (*absolute_filter())->type_name() + ", cannot add "
                   + filter->type_name() + ".");
         }

         if (filter->is_relative()) {
             THROW(Error, "Sets cannot contain other relative filters: " + filter->type_name());;
         }

         switch(filter->type()) {
         case FilterType::Month:
             _ingest_month_filter(filter);
             break;

         case FilterType::Monthday:
             _ingest_monthday_filter(filter);
             break;

         case FilterType::Time:
             _ingest_time_filter(filter);
             break;

         case FilterType::Weekday:
             _ingest_weekday_filter(filter);
             break;

         case FilterType::WeekdayOfMonth:
             _ingest_weekday_of_month_filter(filter);
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

 protected:
     std::string _repr() const override {
         std::vector<std::string> reprs;
         std::transform(_filters.begin(), _filters.end(), std::back_inserter(reprs), [](auto filter) {
             return filter->repr();
         });
         return moonlight::str::join(reprs, ",");
     }

 private:
     void _ingest_month_filter(Filter::Pointer filter) {
         std::shared_ptr<const MonthFilter> month_filter = static_pointer_cast<MonthFilter>(filter);
         std::set<Month> months = month_filter->months();
         auto prev_filter = pop_filter(FilterType::Month);
         if (prev_filter.has_value()) {
             std::shared_ptr<const MonthFilter> prev_month_filter = static_pointer_cast<MonthFilter>(prev_filter.value());
             std::copy(prev_month_filter->months().begin(),
                       prev_month_filter->months().end(),
                       std::inserter(months, months.begin()));
         }
         auto new_filter = MonthFilter::create(months);
         _filters.push_back(new_filter);
     }

     void _ingest_monthday_filter(Filter::Pointer filter) {
         std::shared_ptr<const MonthdayFilter> monthday_filter = static_pointer_cast<MonthdayFilter>(filter);
         std::set<int> days = monthday_filter->days();
         auto prev_filter = pop_filter(FilterType::Monthday);
         if (prev_filter.has_value()) {
             std::shared_ptr<const MonthdayFilter> prev_monthday_filter = static_pointer_cast<MonthdayFilter>(prev_filter.value());
             std::copy(prev_monthday_filter->days().begin(),
                       prev_monthday_filter->days().end(),
                       std::inserter(days, days.begin()));
         }
         auto new_filter = MonthdayFilter::create(days);
         _filters.push_back(new_filter);
     }

     void _ingest_time_filter(Filter::Pointer filter) {
         std::shared_ptr<const TimeFilter> time_filter = static_pointer_cast<TimeFilter>(filter);
         std::set<Time> times = time_filter->times();
         auto prev_filter = pop_filter(FilterType::Time);
         if (prev_filter.has_value()) {
             std::shared_ptr<const TimeFilter> prev_time_filter = static_pointer_cast<TimeFilter>(prev_filter.value());
             std::copy(prev_time_filter->times().begin(),
                       prev_time_filter->times().end(),
                       std::inserter(times, times.begin()));
         }
         auto new_filter = MonthdayFilter::create(days);
         _filters.push_back(new_filter);
     }

     void _ingest_weekday_filter(Filter::Pointer filter) {
         if (get_filter(FilterType::WeekdayOfMonth)) {
             THROW(Error, "Weekday and WeekdayOfMonth filters are mutually exclusive in sets.");
         }
         std::shared_ptr<WeekdayFilter> weekday_filter = static_pointer_cast<WeekdayFilter>(filter);
         std::set<Weekday> weekdays = weekday_filter->weekdays();
         auto prev_filter = pop_filter(FilterType::Weekday);
         if (prev_filter.has_value()) {
             std::shared_ptr<const WeekdayFilter> prev_weekday_filter = static_pointer_cast<WeekdayFilter>(prev_filter.value());
             std::copy(prev_weekday_filter->weekdays().begin(),
                       prev_weekday_filter->weekdays().end(),
                       std::inserter(weekdays, weekdays.begin()));
         }
         auto new_filter = WeekdayFilter::create(weekdays);
         _filters.push_back(new_filter);
     }

     void _ingest_weekday_of_month_filter(Filter::Pointer filter) {
         if (get_filter(FilterType::Weekday)) {
             THROW(Error, "Weekday and WeekdayOfMonth filters are mutually exclusive in sets.");
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

     void validate() const {
         auto monthday_filter_raw = get_filter(FilterType::Monthday);
         if (! monthday_filter_raw.has_value()) {
             return;
         }

         auto month_filter_raw = get_filter(FilterType::Month);
         if (! month_filter_raw.has_value()) {
             return;
         }

         auto monthday_filter = std::static_pointer_cast<MonthdayFilter>(monthday_filter_raw.value());
         auto month_filter = std::static_pointer_cast<MonthFilter>(month_filter_raw.value());

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

         auto year_filter_raw = get_filter(FilterType::Year);

         if (! year_filter_raw.has_value()) {
             return;
         }

         auto year_filter = static_pointer_cast<YearFilter>(year_filter_raw.value());

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

     std::vector<Filter::Pointer> _filters;
};

}
#endif /* !__TIMEFILTER_SET_H */
