/*
 * date_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Friday December 25, 2020
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_DATE_FILTER_H
#define __TIMEFILTER_DATE_FILTER_H

#include "timefilter/core.h"

namespace timefilter {

class DateFilter : public Filter {
public:
    DateFilter(const Date& date) : Filter(FilterType::Date), _date(date) { }

    std::optional<Range> next_range(const Datetime& pivot) const override {
        if (pivot.date() >= _date) {
            return {};

        } else {
            return range(pivot.zone());
        }
    }

    std::optional<Range> prev_range(const Datetime& pivot) const override {
        if (pivot.date() >= _date) {
            return range(pivot.zone());
        } else {
            return {};
        }
    }

private:
    Range range(const Zone& zone) const {
        return Range::for_days(_date, 1).with_zone(zone);
    }

    Date _date;
};

}


#endif /* !__TIMEFILTER_DATE_FILTER_H */
