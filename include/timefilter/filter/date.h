/*
 * date.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday April 11, 2024
 */

#ifndef __TIMEFILTER_DATE_FILTER_H
#define __TIMEFILTER_DATE_FILTER_H

#include "timefilter/filter/core.h"

namespace timefilter {

class DateFilter : public Filter {
 public:
     explicit DateFilter(const Date& dt)
     : Filter(FilterType::Date), _date(dt) { }

     static std::shared_ptr<DateFilter> create(const Date& dt) {
         return std::make_shared<DateFilter>(dt);
     }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         if (pivot < _range().start()) {
             return _range();
         }
         return {};
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         if (pivot > _range().start()) {
             return _range();
         }
         return {};
     }

 protected:
     std::string _repr() const override {
         return _date.isoformat();
     }

 private:
     Range _range() const {
         auto dt_start = Datetime(_date, Time::start_of_day());
         auto dt_end = Datetime(_date.advance_days(1), Time::start_of_day());
         return Range(dt_start, dt_end);
     }
     const Date _date;
};

}

#endif /* !__TIMEFILTER_DATE_FILTER_H */
