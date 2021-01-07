/*
 * weekday_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday December 17, 2020
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_WEEKDAY_FILTER_H
#define __TIMEFILTER_WEEKDAY_FILTER_H

#include "timefilter/core.h"
#include "moonlight/variadic.h"
#include <set>

namespace timefilter {

class WeekdayFilter : public Filter {
public:
    template<class... TD>
    static std::shared_ptr<WeekdayFilter> for_days(TD... params) {
        std::set<Weekday> weekdays;
        moonlight::variadic::pass{weekdays.insert(params)...};
        return create(weekdays);
    }

    static std::shared_ptr<WeekdayFilter> create(const std::set<Weekday> weekdays) {
        return std::make_shared<WeekdayFilter>(weekdays);
    }

    std::optional<Range> next_range(const Datetime& pivot) const override {
        Date date = pivot.date().advance_days(1);
        while (_weekdays.find(date.weekday()) == _weekdays.end()) {
            date++;
        }
        return Range::for_days(date, 1);
    }

    std::optional<Range> prev_range(const Datetime& pivot) const override {
        Date date = pivot.date();
        while (_weekdays.find(date.weekday()) == _weekdays.end()) {
            date--;
        }
        return Range::for_days(date, 1);
    }

private:
    WeekdayFilter(const std::set<Weekday>& weekdays) : Filter(FilterType::Weekday), _weekdays(weekdays) { }

    std::set<Weekday> _weekdays;
};

}

#endif /* !__TIMEFILTER_WEEKDAY_FILTER_H */
