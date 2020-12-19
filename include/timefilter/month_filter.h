/*
 * month_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday December 17, 2020
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_MONTH_FILTER_H
#define __TIMEFILTER_MONTH_FILTER_H

#include "timefilter/core.h"

namespace timefilter {

class MonthFilter : public Filter {
public:
    MonthFilter(Month month) : Filter(FilterType::Month), _month(month) { }

    std::optional<Range> next_range(const Datetime& pivot) const override {
        Date month_start = pivot.date().with_day(1).with_month(_month);

        if (month_start <= pivot.date()) {
            month_start = month_start.with_year(month_start.year() + 1);
        }

        return range(month_start, pivot.zone());
    }

    std::optional<Range> prev_range(const Datetime& pivot) const override {
        Date month_start = pivot.date().with_day(1).with_month(_month);

        if (month_start > pivot.date()) {
            month_start = month_start.with_year(month_start.year() - 1);
        }

        return range(month_start, pivot.zone());
    }

private:
    Range range(const Date& month_start, const Zone& zone) const {
        return Range(
            Datetime(month_start),
            Datetime(month_start.next_month())).with_zone(zone);
    }

    Month _month;
};

}

#endif /* !__TIMEFILTER_MONTH_FILTER_H */
