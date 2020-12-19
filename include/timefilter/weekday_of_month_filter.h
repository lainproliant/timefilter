/*
 * weekday_of_month_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday December 17, 2020
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_WEEKDAY_OF_MONTH_FILTER_H
#define __TIMEFILTER_WEEKDAY_OF_MONTH_FILTER_H

#include "timefilter/core.h"

namespace timefilter {

class WeekdayOfMonthFilter : public Filter {
public:
    WeekdayOfMonthFilter(Weekday weekday, int offset) : Filter(FilterType::WeekdayOfMonth), _weekday(weekday), _offset(offset) {
        validate();
    }

    std::optional<Range> next_range(const Datetime& pivot) const override {
        Date pivot_date = pivot.date();
        std::optional<Date> date;

        for (date = find(pivot_date);
             !date.has_value() || date <= pivot_date;
             pivot_date = pivot_date.next_month());

        return Range::for_days(date.value(), 1);
    }

    std::optional<Range> prev_range(const Datetime& pivot) const override {
        Date pivot_date = pivot.date();
        std::optional<Date> date;

        for (date = find(pivot_date);
             !date.has_value() || date > pivot_date;
             pivot_date = pivot_date.prev_month());

        return Range::for_days(date.value(), 1);
    }

private:
    void validate() const {
        if (_offset < -5 || _offset > 5 || _offset == 0) {
            throw ValueError("Offset x must be: '-5 <= x <= 5' and can't be 00 for offset in WeekdayOfMonthFilter.");
        }
    }

    std::optional<Date> find(const Date& pivot) const {
        if (_offset > 0) {
            Date date = pivot.start_of_month();

            for (int x = 0;
                 x < _offset && date.month() == pivot.month();
                 date++) {

                if (date.weekday() == _weekday) {
                    x++;
                }
            }

            if (date.month() != pivot.month()) {
                return {};
            }
            return date;

        } else {
            Date date = pivot.end_of_month();

            for (int x = 0;
                 x < _offset && date.month() == pivot.month();
                 date--) {

                if (date.weekday() == _weekday) {
                    x++;
                }
            }

            if (date.month() != pivot.month()) {
                return {};
            }

            return date;
        }
    }

    Weekday _weekday;
    int _offset;
};

}

#endif /* !__TIMEFILTER_WEEKDAY_OF_MONTH_FILTER_H */
