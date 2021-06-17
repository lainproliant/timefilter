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
#include "moonlight/variadic.h"
#include <set>

namespace timefilter {

class MonthdayFilter : public Filter {
public:
    MonthdayFilter(const std::set<int>& days) : Filter(FilterType::Monthday), _days(days) { }

    template<class... TD>
    static std::shared_ptr<MonthdayFilter> create(int first, TD... params) {
        std::set<int> days;
        days.insert(first);
        moonlight::variadic::pass{days.insert(params)...};
        return create(days);
    }

    static std::shared_ptr<MonthdayFilter> create(int day) {
        std::set<int> days;
        days.insert(day);
        return create(days);
    }

    static std::shared_ptr<MonthdayFilter> create(const std::set<int>& days) {
        return std::make_shared<MonthdayFilter>(days);
    }

    std::optional<Range> next_range(const Datetime& pivot) const override {
        Date date = pivot.date();
        auto monthday = get_next_monthday(date);
        return Range::for_days(monthday, 1);
    }

    std::optional<Range> prev_range(const Datetime& pivot) const override {
        Date date = pivot.date();
        auto monthday = get_prev_monthday(date);
        return Range::for_days(monthday, 1);
    }

protected:
    std::string _repr() const override {
        std::vector<int> monthdays(_days.begin(), _days.end());
        std::sort(monthdays.begin(), monthdays.end());
        return moonlight::str::join(monthdays, ", ");
    }

private:
    void validate() {
        for (int day : _days) {
            if (day == 0 || day < -31 || day > 31) {
                throw ValueError("Offset x must be: '-31 <= x <= 31' and can't be 0 for offset in MonthdayFilter.");
            }
        }
    }

    Date get_next_monthday(const Date& pivot) const {
        Date date = pivot.advance_days(1);

        for (;;) {
            auto monthdays = get_days_for_month(date);
            auto iter = std::find_if(monthdays.begin(), monthdays.end(), [&](const Date& monthday) {
                return monthday > date.recede_days(1);
            });
            if (iter != monthdays.end()) {
                return *iter;
            }
            date = date.next_month().start_of_month();
        }
    }

    Date get_prev_monthday(const Date& pivot) const {
        Date date = pivot;

        for (;;) {
            auto monthdays = get_days_for_month(date);
            auto iter = std::find_if(monthdays.begin(), monthdays.end(), [&](const Date& monthday) {
                return monthday <= date;
            });
            if (iter != monthdays.end()) {
                return *iter;
            }
            date = date.prev_month().end_of_month();
        }

    }

    std::vector<Date> get_days_for_month(const Date& pivot) const {
        std::vector<Date> monthdays;
        std::set<Date> dates;

        for (int day : _days) {
            if (day > 0) {
                auto date = pivot.start_of_month().advance_days(day - 1);
                if (date.month() == pivot.month() && dates.find(date) == dates.end()) {
                    monthdays.push_back(date);
                    dates.insert(date);
                }

            } else {
                auto date = pivot.end_of_month().recede_days((day * -1) - 1);
                if (date.month() == pivot.month() && dates.find(date) == dates.end()) {
                    monthdays.push_back(date);
                    dates.insert(date);
                }
            }
        }

        std::sort(monthdays.begin(), monthdays.end());
        return monthdays;
    }

    std::set<int> _days;
};

}

#endif /* !__TIMEFILTER_MONTHDAY_FILTER_H */
