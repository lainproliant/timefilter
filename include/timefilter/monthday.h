/*
 * monthday.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#ifndef __TIMEFILTER_MONTHDAY_H
#define __TIMEFILTER_MONTHDAY_H

#include "timefilter/filter.h"

namespace timefilter {

class MonthdayFilter : public Filter {
 public:
     MonthdayFilter(const int day) : Filter(FilterType::Monthday), _days({day}) {
         validate();
     }

     MonthdayFilter(const std::set<int>& days) : Filter(FilterType::Monthday), _days(days) {
         validate();
     }

     template<class V>
     static Pointer create(const V& param) {
         return std::make_shared<MonthdayFilter>(param);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         Date range_month = dt.date().start_of_month();

         while (range_month.year() - dt.date().year() <= 8) {
             auto ranges = monthday_ranges(dt.zone(), range_month.year(), range_month.month());
             std::cout << "rm = " << range_month << ", dt = " << dt << ", ranges = " << moonlight::str::join(ranges, ",") << std::endl;
             for (auto iter = ranges.begin(); iter != ranges.end(); iter++) {
                 if (dt < iter->start()) {
                     return *iter;
                 }
             }

             range_month = range_month.next_month();
         }

//         for (Date date = dt.date().start_of_month();
//              std::abs(date.year() - dt.date().year()) <= 8;
//              date = date.next_month()) {
//             auto ranges = monthday_ranges(dt.zone(), date.year(), date.month());
//             std::cout << "dy = " << std::abs(date.year() - dt.date().year()) << ", dt = " << dt << ", ranges = " << moonlight::str::join(ranges, ",") << std::endl;
//             for (auto iter = ranges.begin(); iter != ranges.end(); iter++) {
//                 if (dt < iter->start()) {
//                     return *iter;
//                 }
//             }
//         }

        THROW(Error, "Monthday filter could not find a next range.");
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         Date range_month = dt.date().start_of_month();

         while (dt.date().year() - range_month.year() <= 8) {
             auto ranges = monthday_ranges(dt.zone(), range_month.year(), range_month.month());
             std::cout << "rm = " << range_month << ", dt = " << dt << ", ranges = " << moonlight::str::join(ranges, ",") << std::endl;
             for (auto iter = ranges.rbegin(); iter != ranges.rend(); iter++) {
                 if (dt >= iter->start()) {
                     return *iter;
                 }
             }

             range_month = range_month.prev_month();
         }
//         for (Date date = dt.date().start_of_month();
//              dt.date().year() - date.year() <= 8;
//              date = date.prev_month()) {
//             auto ranges = monthday_ranges(dt.zone(), date.year(), date.month());
//             for (auto iter = ranges.rbegin(); iter != ranges.rend(); iter++) {
//                 if (dt >= iter->start()) {
//                     return *iter;
//                 }
//             }
//         }

        THROW(Error, "Monthday filter could not find a prev range.");
     }

     const std::set<int>& days() const {
         return _days;
     }

 protected:
     std::string _repr() const override {
         std::vector<int> monthdays;
         std::copy(_days.begin(), _days.end(), std::back_inserter(monthdays));
         std::sort(monthdays.begin(), monthdays.end());
         return moonlight::str::join(monthdays, ",");
     }

 private:
     void validate() const {
         if (_days.size() == 0) {
             THROW(Error, "At least one monthday must be provided for MonthdayFilter.");

         }

         for (int day : _days) {
             if (day == 0 || day < -31 || day > 31) {
                 THROW(Error, "Offset x must be: '-31 <= x <= 31' and can't be 0 for offset in MonthdayFilter.");
             }
         }
     }

     std::vector<Range> monthday_ranges(const Zone& zone, int year, Month month) const {
         std::vector<Range> ranges;
         const int last_day = last_day_of_month(year, month);

         for (auto day : _days) {
             if (std::abs(day) <= last_day) {
                 auto date = Date(year, month, day > 0 ? day : last_day + day + 1);

                 ranges.push_back(Range(
                         Datetime(zone, date),
                         Datetime(zone, date.advance_days(1))));
             }
         }

         std::sort(ranges.begin(), ranges.end(), [](const Range& rgA, const Range& rgB) {
             return rgA.start() < rgB.start();
         });

         return ranges;
     }

     const std::set<int> _days;
};

}


#endif /* !__TIMEFILTER_MONTHDAY_H */
