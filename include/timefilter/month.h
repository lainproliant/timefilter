/*
 * month_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday December 17, 2020
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_MONTH_FILTER_H
#define __TIMEFILTER_MONTH_FILTER_H

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include "timefilter/core.h"
#include "moonlight/variadic.h"

namespace timefilter {

// ------------------------------------------------------------------
class MonthFilter : public Filter {
 public:
     explicit MonthFilter(const std::set<Month>& months) : Filter(FilterType::Month), _months(months) { }

     template<class... TD>
     static std::shared_ptr<MonthFilter> create(Month first, TD... params) {
         std::set<Month> months;
         months.insert(first);
         moonlight::variadic::pass{months.insert(params)...}; return create(months);
     }

     static std::shared_ptr<MonthFilter> create(Month month) {
         std::set<Month> months;
         months.insert(month);
         return create(months);
     }

     static std::shared_ptr<MonthFilter> create(const std::set<Month>& months) {
         return std::make_shared<MonthFilter>(months);
     }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         Date date;

         for (date = pivot.date().with_day(1).next_month();
              _months.find(date.month()) == _months.end();
              date = date.next_month()) { }

         return range(date, pivot.zone());
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         Date date;

         for (date = pivot.date().with_day(1);
              _months.find(date.month()) == _months.end();
              date = date.prev_month()) { }

         return range(date, pivot.zone());
     }

     const std::set<Month>& months() const {
         return _months;
     }


 protected:
     std::string _repr() const override {
         std::vector<int> nmonths;
         std::transform(_months.begin(), _months.end(), std::back_inserter(nmonths), [](auto m) {
             return static_cast<int>(m);
         });
         std::sort(nmonths.begin(), nmonths.end());
         return moonlight::str::join(nmonths, ", ");
     }

 private:
     Range range(const Date& month_start, const Zone& zone) const {
         return Range(
             Datetime(month_start),
             Datetime(month_start.next_month())).zone(zone);
     }

     std::set<Month> _months;
};

}  // namespace timefilter

#endif /* !__TIMEFILTER_MONTH_FILTER_H */
