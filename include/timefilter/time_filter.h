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

#include "timefilter/core.h"

namespace timefilter {

class TimeFilter : public Filter {
public:
    static std::shared_ptr<TimeFilter> create(const Time& time) {
        return std::make_shared<TimeFilter>(time);
    }

    std::optional<Range> next_range(const Datetime& pivot) const override {
        if (pivot.time() < _time) {
            return range(Datetime(pivot.date(), _time), pivot.zone());

        } else {
            return range(Datetime(pivot.date().advance_days(1), _time),
                         pivot.zone());
        }
    }

    std::optional<Range> prev_range(const Datetime& pivot) const override {
        if (pivot.time() < _time) {
            return range(Datetime(pivot.date().recede_days(1), _time),
                         pivot.zone());
        } else {
            return range(Datetime(pivot.date(), _time), pivot.zone());
        }
    }

private:
    TimeFilter(const Time& time) : Filter(FilterType::Time), _time(time) { }

    Range range(const Datetime& dt, const Zone& zone) const {
        return Range(
            dt,
            dt + Duration(1)
        ).with_zone(zone);
    }

    Time _time;
};

}

#endif /* !__TIMEFILTER_TIME_FILTER_H */
