/*
 * weekday_monthday.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Saturday May 18, 2024
 */

#ifndef __TIMEFILTER_WEEKDAY_MONTHDAY_H
#define __TIMEFILTER_WEEKDAY_MONTHDAY_H

#include "timefilter/filter.h"

namespace timefilter {

class WeekdayMonthdayFilter : public Filter {
 public:
     WeekdayMonthdayFilter(Weekday weekday, int monthday) :
     Filter(FilterType::WeekdayMonthday),
     _weekdays({weekday}),
     _monthdays({monthday}) {
         validate();
     }

     WeekdayMonthdayFilter(const std::set<Weekday> weekdays, int monthday) :
     Filter(FilterType::WeekdayMonthday),
     _weekdays(weekdays),
     _monthdays({monthday}) {
         validate();
     }

     WeekdayMonthdayFilter(Weekday weekday, const std::set<int>& monthdays) :
     Filter(FilterType::WeekdayMonthday),
     _weekdays({weekday}),
     _monthdays(monthdays) {
         validate();
     }

     WeekdayMonthdayFilter(const std::set<Weekday> weekdays, const std::set<int>& monthdays) :
     Filter(FilterType::WeekdayMonthday),
     _weekdays(weekdays),
     _monthdays(monthdays) {
         validate();
     }

     template<class V>
     static Pointer create(const V& value) {
         return std::make_shared<WeekdayMonthdayFilter>(value);
     }

     template<class V1, class V2>
     static Pointer create(const V1& value1, const V2& value2) {
         return std::make_shared<WeekdayMonthdayFilter>(value1, value2);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         auto date = dt.date().advance_days(1);
         int year = date.year();
         Month month = date.month();
         auto monthdays = monthdays_for_month(date.year(), date.month());

         for (;;) {
             if (monthdays.contains(date.day()) &&
                 _weekdays.contains(date.weekday())) {
                 return Range(
                     Datetime(dt.zone(), date),
                     Datetime(dt.zone(), date.advance_days(1))
                 );
             }

             date = date.advance_days(1);
             if (year != date.year() || month != date.month()) {
                 monthdays = monthdays_for_month(date.year(), date.month());
                 year = date.year();
                 month = date.month();
             }
         }
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         auto date = dt.date();
         int year = date.year();
         Month month = date.month();
         auto monthdays = monthdays_for_month(date.year(), date.month());

         for (;;) {
             if (monthdays.contains(date.day()) &&
                 _weekdays.contains(date.weekday())) {
                 return Range(
                     Datetime(dt.zone(), date),
                     Datetime(dt.zone(), date.advance_days(1))
                 );
             }

             date = date.recede_days(1);
             if (year != date.year() || month != date.month()) {
                 monthdays = monthdays_for_month(date.year(), date.month());
                 year = date.year();
                 month = date.month();
             }
         }
     }

     const std::set<Weekday>& weekdays() const {
         return _weekdays;
     }

     const std::set<int>& monthdays() const {
         return _monthdays;
     }

 protected:
     std::string _repr() const override {
         static const std::string weekday_chrs = "UMTWHFS";
         std::vector<std::string> weekday_strs;
         std::vector<int> weekday_ids;

         std::transform(_weekdays.begin(), _weekdays.end(), std::back_inserter(weekday_ids), [](Weekday weekday) {
             return static_cast<std::underlying_type_t<Weekday>>(weekday);
         });
         std::sort(weekday_ids.begin(), weekday_ids.end());
         std::transform(weekday_ids.begin(), weekday_ids.end(), std::back_inserter(weekday_strs), [&](int weekday_id) {
             return moonlight::str::chr(weekday_chrs[weekday_id]);
         });

         std::vector<int> monthdays;
         std::copy(_monthdays.begin(), _monthdays.end(), std::back_inserter(monthdays));
         std::sort(monthdays.begin(), monthdays.end());

         std::vector<std::string> reprs;
         reprs.push_back(moonlight::str::join(weekday_strs));
         std::transform(monthdays.begin(), monthdays.end(), std::back_inserter(reprs), [](int day) {
             return std::to_string(day);
         });

         return moonlight::str::join(reprs, ",");
     }

 private:
     std::set<int> monthdays_for_month(int year, Month month) const {
         std::set<int> monthdays;
         const int last_day = last_day_of_month(year, month);

         for (int day : _monthdays) {
             if (day < 0) {
                 day = last_day - (day - 1);

                 if (day > 0) {
                     monthdays.insert(day);
                 }

             } else if (day <= last_day) {
                 monthdays.insert(day);
             }
         }

         return monthdays;
     }

     void validate() {
         if (_weekdays.size() == 0) {
             THROW(Error, "At least one weekday must be provided for WeekdayMonthdayFilter.");
         }

         if (_monthdays.size() == 0) {
             THROW(Error, "At least one monthday must be provided for WeekdayMonthdayFilter.");
         }

         for (int day : _monthdays) {
             if (day == 0 || day < -31 || day > 31) {
                 THROW(Error, "Offset x must be: '-31 <= x <= 31' and can't be 0 for offset in WeekdayMonthdayFilter.");
             }
         }
     }

     std::set<Weekday> _weekdays;
     std::set<int> _monthdays;
};

}

#endif /* !__TIMEFILTER_WEEKDAY_MONTHDAY_H */
