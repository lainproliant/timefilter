/*
 * weekday.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#ifndef __TIMEFILTER_WEEKDAY_H
#define __TIMEFILTER_WEEKDAY_H

#include "timefilter/filter.h"

namespace timefilter {

class WeekdayFilter : public Filter {
 public:
     WeekdayFilter(Weekday weekday) : Filter(FilterType::Weekday), _weekdays({weekday}) { }

     WeekdayFilter(const std::set<Weekday>& weekdays) : Filter(FilterType::Weekday), _weekdays(weekdays) {
         validate();
     }

     template<class V>
     static Pointer create(const V& value) {
         return std::make_shared<WeekdayFilter>(value);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         auto ranges = weekday_ranges(dt.zone(), dt.date());

         for (auto iter = ranges.begin(); iter != ranges.end(); iter++) {
             if (dt < iter->start()) {
                 return *iter;
             }
         }

        THROW(Error, "Weekday filter could not find a next range.");
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         auto ranges = weekday_ranges(dt.zone(), dt.date().recede_days(6));

         for (auto iter = ranges.rbegin(); iter != ranges.rend(); iter++) {
             if (dt >= iter->start()) {
                 return *iter;
             }
         }

        THROW(Error, "Weekday filter could not find a prev range.");
     }

     const std::set<Weekday>& weekdays() const {
         return _weekdays;
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

         return moonlight::str::join(weekday_strs);
     }


 private:
     void validate() const {
         if (_weekdays.size() == 0) {
             THROW(Error, "At least one weekday must be provided for WeekdayFilter.");
         }
     }

     std::vector<Range> weekday_ranges(const Zone& zone, const Date& date) const {
         std::vector<Range> ranges;

         for (int x = 0; x <= 7; x++) {
             auto new_date = date.advance_days(x);
             if (_weekdays.contains(new_date.weekday())) {
                 ranges.push_back(Range(
                         Datetime(zone, new_date),
                         Datetime(zone, new_date.advance_days(1))
                 ));
             }
         }

         return ranges;
     }

     const std::set<Weekday> _weekdays;
};

}

#endif /* !__TIMEFILTER_WEEKDAY_H */
