/*
 * weekday_of_month.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#ifndef __TIMEFILTER_WEEKDAY_OF_MONTH_H
#define __TIMEFILTER_WEEKDAY_OF_MONTH_H

#include "timefilter/filter.h"

namespace timefilter {

class WeekdayOfMonthFilter : public Filter {
 public:
     WeekdayOfMonthFilter(Weekday weekday, int offset) : Filter(FilterType::WeekdayOfMonth), _weekday(weekday), _offset(offset) {
         validate();
     }

     static Pointer create(Weekday weekday, int offset) {
         return std::make_shared<WeekdayOfMonthFilter>(weekday, offset);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         for (Date year_month = dt.date().start_of_month();
              Datetime(dt.zone(), year_month) - dt <= Duration::of_days(500);
              year_month = year_month.next_month()) {
             std::optional<Range> range = monthday_range(dt.zone(), year_month.year(), year_month.month());
             if (range.has_value() && dt < range->start()) {
                 return range;
             }
         }

        THROW(Error, "WeekdayOfMonth filter could not find a next range.");
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         for (Date year_month = dt.date().start_of_month();
              dt - Datetime(dt.zone(), year_month) <= Duration::of_days(500);
              year_month = year_month.prev_month()) {
             std::optional<Range> range = monthday_range(dt.zone(), year_month.year(), year_month.month());
             if (range.has_value() && dt >= range->start()) {
                 return range;
             }
         }

        THROW(Error, "WeekdayOfMonth filter could not find a prev range.");
     }

     Weekday weekday() const {
         return _weekday;
     }

     int offset() const {
         return _offset;
     }

 protected:
     std::string _repr() const override {
         static const std::vector<std::string> weekday_names = {
             "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
         };
         std::ostringstream sb;
         int weekday_id = static_cast<std::underlying_type_t<Weekday>>(_weekday);
         sb << weekday_names[weekday_id] << "/" << _offset;
         return sb.str();
     }

 private:
     void validate() const {
         if (_offset < -5 || _offset > 5 || _offset == 0) {
             THROW(Error, "Offset must be: '-5 <= x <= 5' and can't be 0 for offset in WeekdayOfMonth.");
         }
     }

     std::optional<Range> monthday_range(const Zone& zone, int year, Month month) const {
         const Date month_start = Date(year, month);
         const Date month_end = month_start.end_of_month();
         Date date;

         if (_offset > 0) {
             date = month_start;
             for (int x = 0; x < _offset; date = date.advance_days(1)) {
                 if (date.weekday() == _weekday) {
                     x++;
                 }
             }

         } else {
             date = month_end;
             for (int x = 0; x > _offset; date = date.recede_days(1)) {
                 if (date.weekday() == _weekday) {
                     x--;
                 }
             }
         }

         if (date.year() == month_start.year() &&
             date.month() == month_start.month()) {
             return Range(
                 Datetime(zone, date),
                 Datetime(zone, date.advance_days(1))
             );
         }

         return {};
     }

     const Weekday _weekday;
     const int _offset;
};

}

#endif /* !__TIMEFILTER_WEEKDAY_OF_MONTH_H */
