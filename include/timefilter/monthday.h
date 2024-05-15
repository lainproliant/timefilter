/*
 * monthday_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday December 17, 2020
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_MONTHDAY_FILTER_H
#define __TIMEFILTER_MONTHDAY_FILTER_H

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include "timefilter/core.h"
#include "moonlight/variadic.h"

namespace timefilter {

using moonlight::core::ValueError;

class MonthdayFilter : public Filter {
 public:
     explicit MonthdayFilter(const std::set<int>& days) : Filter(FilterType::Monthday) {
         std::copy(days.begin(), days.end(), std::back_inserter(_days));
         sort_and_validate();
     }

     template<class... TD>
     static std::shared_ptr<MonthdayFilter> create(int first, TD... params) {
         std::set<int> days;
         days.insert(first);
         moonlight::variadic::pass{days.insert(params)...};
         return create(days);
     }

     static std::shared_ptr<MonthdayFilter> create(int day) {
         std::set<int> days;
         days.insert(day);
         return create(days);
     }

     static std::shared_ptr<MonthdayFilter> create(const std::set<int>& days) {
         return std::make_shared<MonthdayFilter>(days);
     }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         Date month_pivot = pivot.date();

         for (;;) {
             auto dates = dates_for_month(month_pivot.year(), month_pivot.month());
             for (auto iter = dates.begin(); iter != dates.end(); iter++) {
                 if (*iter > pivot.date()) {
                     return Range::for_days(*iter, 1);
                 }
             }
             month_pivot = month_pivot.next_month();
         }
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         Date month_pivot = pivot.date();

         for (;;) {
             auto dates = dates_for_month(month_pivot.year(), month_pivot.month());
             for (auto iter = dates.rbegin(); iter != dates.rend(); iter++) {
                 if (*iter <= pivot.date()) {
                     return Range::for_days(*iter, 1);
                 }
             }
             month_pivot = month_pivot.prev_month();
         }
     }

     const std::vector<int>& monthdays() const {
         return _days;
     }

 protected:
     std::string _repr() const override {
         std::vector<int> monthdays(_days.begin(), _days.end());
         std::sort(monthdays.begin(), monthdays.end());
         return moonlight::str::join(monthdays, ", ");
     }

 private:
     std::vector<Date> dates_for_month(const int year, const Month month) const {
         std::vector<Date> dates;
         const int last_day = last_day_of_month(year, month);

         for (int day : _days) {
             if (std::abs(day) <= last_day) {
                 if (day < 0) {
                     dates.push_back(Date(year, month, last_day + day + 1));
                 } else {
                     dates.push_back(Date(year, month, day));
                 }
             }
         }

         std::sort(dates.begin(), dates.end());
         return dates;
     }

     void sort_and_validate() {
         for (int day : _days) {
             if (day == 0 || day < -31 || day > 31) {
                 throw ValueError("Offset x must be: '-31 <= x <= 31' and can't be 0 for offset in MonthdayFilter.");
             }
         }
         std::sort(_days.begin(), _days.end());
     }

     std::vector<int> _days;
};

}  // namespace timefilter

#endif /* !__TIMEFILTER_MONTHDAY_FILTER_H */
