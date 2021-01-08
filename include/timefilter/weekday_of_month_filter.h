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

    static std::shared_ptr<WeekdayOfMonthFilter> create(Weekday weekday, int offset) {
        return std::make_shared<WeekdayOfMonthFilter>(weekday, offset);
    }

    std::optional<Range> next_range(const Datetime& pivot) const override {
        Date date = pivot.date();
        std::optional<Date> month_weekday;

        for (month_weekday = find(date);
             !month_weekday.has_value() || month_weekday <= pivot.date();
             date = date.next_month(), month_weekday = find(date));

        return Range::for_days(month_weekday.value(), 1);
    }

    std::optional<Range> prev_range(const Datetime& pivot) const override {
        Date date = pivot.date();
        std::optional<Date> month_weekday;

        for (month_weekday = find(date);
             !month_weekday.has_value() || month_weekday > pivot.date();
             date = date.prev_month(), month_weekday = find(date));

        return Range::for_days(month_weekday.value(), 1);
    }

private:
    void validate() const {
        if (_offset < -5 || _offset > 5 || _offset == 0) {
            throw ValueError("Offset x must be: '-5 <= x <= 5' and can't be 00 for offset in WeekdayOfMonthFilter.");
        }
    }

    std::optional<Date> find(const Date& pivot) const {
        if (_offset > 0) {
            Date date = pivot.start_of_month().recede_days(1);

            for (int x = 0; x < _offset;) {
                date++;
                if (date.weekday() == _weekday) {
                    x++;
                }
            }

            if (date.month() != pivot.month()) {
                return {};
            }
            return date;

        } else {
            int offset = -1 * _offset;
            Date date = pivot.end_of_month().advance_days(1);

            for (int x = 0; x < offset;) {
                date--;
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
