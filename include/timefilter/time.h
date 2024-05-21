/*
 * time.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#ifndef __TIMEFILTER_TIME_H
#define __TIMEFILTER_TIME_H

#include "timefilter/filter.h"

namespace timefilter {

class TimeFilter : public Filter {
 public:
     TimeFilter(const Time& time) : Filter(FilterType::Time), _times({time}) { }
     TimeFilter(const std::set<Time>& times) : Filter(FilterType::Time), _times(times) {
         validate();
     }

     template<class V>
     static Pointer create(const V& param) {
         return std::make_shared<TimeFilter>(param);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         for (Date date = dt.date();
              Datetime(dt.zone(), date) - dt <= Duration::of_days(2);
              date = date.advance_days(1)) {
             auto ranges = time_ranges(dt.zone(), date);
             for (auto iter = ranges.begin(); iter != ranges.end(); iter++) {
                 if (dt < iter->start()) {
                     return *iter;
                 }
             }
         }

         THROW(Error, "Time filter could not find a next range.");
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         for (Date date = dt.date();
              dt - Datetime(dt.zone(), date) <= Duration::of_days(2);
              date = date.recede_days(1)) {
             auto ranges = time_ranges(dt.zone(), date);
             for (auto iter = ranges.rbegin(); iter != ranges.rend(); iter++) {
                 if (dt >= iter->start()) {
                     return *iter;
                 }
             }
         }

         THROW(Error, "Time filter could not find a prev range.");
     }

     const std::set<Time>& times() const {
         return _times;
     }

 protected:
     std::string _repr() const override {
         std::vector<Time> times;
         std::vector<std::string> iso_times;

         std::copy(_times.begin(), _times.end(), std::back_inserter(times));
         std::transform(times.begin(), times.end(), std::back_inserter(iso_times), [](const Time& t) {
             return t.isoformat();
         });
         return moonlight::str::join(iso_times, ",");
     }

 private:
     void validate() const {
         if (_times.size() == 0) {
             THROW(Error, "At least one time must be provided for TimeFilter.");
         }
     }

     std::vector<Range> time_ranges(const Zone& zone, const Date& date) const {
         std::vector<Range> ranges;

         for (auto time : sorted_times()) {
             auto dt = Datetime(zone, date, time);
             ranges.push_back(Range(dt, dt + Duration::of_minutes(1)));
         }

         return ranges;
     }

     std::vector<Time> sorted_times() const {
         std::vector<Time> times;
         std::copy(_times.begin(), _times.end(), std::back_inserter(times));
         std::sort(times.begin(), times.end());
         return times;
     }

     const std::set<Time> _times;
};

}


#endif /* !__TIMEFILTER_TIME_H */
