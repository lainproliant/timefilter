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
    std::shared_ptr<MonthdayFilter> create(int monthday) {
        return std::make_shared<MonthdayFilter>(monthday);
    }

    std::optional<Range> next_range(const Datetime& pivot) const override {
        Date date = pivot.date().advance_days(1);
        auto monthday = get_monthday(date);

        while (!monthday.has_value() || monthday < date || monthday == pivot.date()) {
            date = date.next_month();
            monthday = get_monthday(date);
        }

        return Range::for_days(monthday.value(), 1);
    }

    std::optional<Range> prev_range(const Datetime& pivot) const override {
        Date date = pivot.date();
        auto monthday = get_monthday(date);

        while (!monthday.has_value() || monthday > pivot.date()) {
            date = date.prev_month();
            monthday = get_monthday(date);
        }

        return Range::for_days(monthday.value(), 1);
    }

private:
    MonthdayFilter(int day) : Filter(FilterType::Monthday), _day(day) {
        validate();
    }

    void validate() {
        if (_day == 0 || _day < -31 || _day > 31) {
            throw ValueError("Offset x must be: '-31 <= x <= 31' and can't be 0 for offset in MonthdayFilter.");
        }
    }

    std::optional<Date> get_monthday(const Date& pivot) const {
        Date slid;

        if (_day > 0) {
            slid = pivot.start_of_month().advance_days(_day - 1);

        } else {
            slid = pivot.end_of_month().recede_days((_day * -1) - 1);
        }

        if (slid.month() == pivot.month()) {
            return slid;
        } else {
            return {};
        }
    }

    int _day;
};

}

#endif /* !__TIMEFILTER_MONTHDAY_FILTER_H */
