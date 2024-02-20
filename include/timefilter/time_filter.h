/*
 * time_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Friday December 25, 2020
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_TIME_FILTER_H
#define __TIMEFILTER_TIME_FILTER_H

#include <set>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include "timefilter/core.h"
#include "moonlight/variadic.h"

namespace timefilter {

class TimeFilter : public Filter {
 public:
     explicit TimeFilter(const std::set<Time>& times) : Filter(FilterType::Time), _times(times) { }

     template<class... TD>
     static std::shared_ptr<TimeFilter> create(const Time& first, TD... params) {
         std::set<Time> times;
         times.insert(first);
         moonlight::variadic::pass{times.insert(params)...};
         return create(times);
     }

     static std::shared_ptr<TimeFilter> create(const Time& time) {
         std::set<Time> times;
         times.insert(time);
         return create(times);
     }

     static std::shared_ptr<TimeFilter> create(const std::set<Time>& times) {
         return std::make_shared<TimeFilter>(times);
     }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         return range(get_next_daytime(pivot), pivot.zone());
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         return range(get_prev_daytime(pivot), pivot.zone());
     }

 protected:
     std::string _repr() const override {
         std::vector<Time> times(_times.begin(), _times.end());
         std::sort(times.begin(), times.end());
         return moonlight::str::join(times, ", ");
     }

 private:
     std::vector<Datetime> get_times_for_day(const Date& pivot) const {
         std::vector<Datetime> daytimes;
         std::transform(_times.begin(), _times.end(), std::back_inserter(daytimes), [&](const Time& time) {
             return Datetime(pivot, time);
         });
         std::sort(daytimes.begin(), daytimes.end());
         return daytimes;
     }

     Datetime get_next_daytime(const Datetime& pivot) const {
         Datetime date = pivot + minutes(1);

         for (;;) {
             auto daytimes = get_times_for_day(date.date());
             auto iter = std::find_if(daytimes.begin(), daytimes.end(), [&](const Datetime& dt) {
                 return dt > (date - minutes(1));
             });
             if (iter != daytimes.end()) {
                 return *iter;
             }
             date = Datetime(pivot.zone(), date.date().advance_days(1));
         }
     }

     Datetime get_prev_daytime(const Datetime& pivot) const {
         Datetime date = pivot;

         for (;;) {
             auto daytimes = get_times_for_day(date.date());
             auto iter = std::find_if(daytimes.begin(), daytimes.end(), [&](const Datetime& dt) {
                 return dt <= date;
             });
             if (iter != daytimes.end()) {
                 return *iter;
             }
             date = Datetime(pivot.zone(), date.date().recede_days(1), Time::end_of_day());
         }
     }

     Range range(const Datetime& dt, const Zone& zone) const {
         return Range(
             dt,
             dt + minutes(1)).zone(zone);
     }

     std::set<Time> _times;
};

}  // namespace timefilter

#endif /* !__TIMEFILTER_TIME_FILTER_H */
