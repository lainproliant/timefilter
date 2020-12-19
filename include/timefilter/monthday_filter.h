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

#include "timefilter/core.h"

namespace timefilter {

class MonthdayFilter : public Filter {
public:
    MonthdayFilter(int day) : Filter(FilterType::Monthday), _day(day) { }


    std::optional<Range> next_range(const Datetime& pivot) const override {
        Date date = pivot.date().advance_days(1);
        auto monthday = get_monthday(date);

        while (!monthday.has_value() || monthday <= date) {
            date = date.next_month();
        }

        return Range::for_days(monthday.value(), 1);
    }

    std::optional<Range> prev_range(const Datetime& pivot) const override {
        Date date = pivot.date();
        auto monthday = get_monthday(date);

        while (!monthday.has_value() || monthday > date) {
            date = date.prev_month();
        }

        return Range::for_days(monthday.value(), 1);
    }

private:
    std::optional<Date> get_monthday(const Date& pivot) const {
        try {
            return pivot.with_day(_day);
        } catch (const ValueError& e) {
            return {};
        }
    }

    int _day;
};

}

#endif /* !__TIMEFILTER_MONTHDAY_FILTER_H */
