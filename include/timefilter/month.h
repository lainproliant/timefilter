/*
 * month.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#ifndef __TIMEFILTER_MONTH_H
#define __TIMEFILTER_MONTH_H

#include "timefilter/filter.h"

namespace timefilter {

class MonthFilter : public Filter {
 public:
     MonthFilter(Month month) : Filter(FilterType::Month), _months({month}) { }
     MonthFilter(const std::set<Month>& months) : Filter(FilterType::Month), _months(months) { }

     static Pointer create(Month month) {
         return std::make_shared<MonthFilter>(month);
     }

     static Pointer create(const std::set<Month>& months) {
         return std::make_shared<MonthFilter>(months);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         for (int year = dt.date().year(); year - dt.date().year() <= 1; year ++) {
             auto ranges = month_ranges(dt.zone(), year);
             for (auto iter = ranges.begin(); iter != ranges.end(); iter++) {
                 if (dt < iter->start()) {
                     return *iter;
                 }
             }
         }

         THROW(Error, "Month filter could not find a next range.");
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         for (int year = dt.date().year(); dt.date().year() - year <= 1; year --) {
             auto ranges = month_ranges(dt.zone(), year);
             for (auto iter = ranges.rbegin(); iter != ranges.rend(); iter++) {
                 if (dt >= iter->start()) {
                     return *iter;
                 }
             }
         }

         THROW(Error, "Month filter could not find a prev range.");
     }

     const std::set<Month>& months() const {
         return _months;
     }

 protected:
     std::string _repr() const override {
         std::vector<int> months;
         std::transform(_months.begin(), _months.end(), std::back_inserter(months), [](Month month) {
             return static_cast<std::underlying_type_t<Month>>(month);
         });
         std::sort(months.begin(), months.end());
         return moonlight::str::join(months, ",");
     }


 private:
     std::vector<Range> month_ranges(const Zone& zone, int year) const {
         std::vector<Range> ranges;
         std::vector<Month> months = sorted_months();

         std::transform(months.begin(), months.end(),
                        std::back_inserter(ranges),
                        [=](Month month) {
                            const Date date = Date(year, month);
                            const Datetime start_dt = Datetime(zone, date);
                            const Datetime end_dt = Datetime(zone, date.next_month());
                            return Range(start_dt, end_dt);
                        });

         return ranges;
     }

     std::vector<Month> sorted_months() const {
         std::vector<Month> months;
         std::copy(_months.begin(), _months.end(), std::back_inserter(months));
         std::sort(months.begin(), months.end());
         return months;
     }

     const std::set<Month> _months;
};

}


#endif /* !__TIMEFILTER_MONTH_H */
