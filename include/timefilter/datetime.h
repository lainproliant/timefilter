/*
 * datetime.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday April 11, 2024
 */

#ifndef __TIMEFILTER_DATETIME_FILTER_H
#define __TIMEFILTER_DATETIME_FILTER_H

#include "timefilter/core.h"

namespace timefilter {

class DatetimeFilter : public Filter {
 public:
     explicit DatetimeFilter(const Datetime& dt)
     : Filter(FilterType::Datetime), _dt(dt) { }

     static std::shared_ptr<DatetimeFilter> create(const Datetime& dt) {
         return std::make_shared<DatetimeFilter>(dt);
     }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         if (pivot < _dt) {
             return Range(_dt, seconds(1));
         }
         return {};
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         if (pivot > _dt) {
             return Range(_dt, seconds(1));
         }
         return {};
     }

 protected:
     std::string _repr() const override {
         return _dt.isoformat();
     }

 private:
     const Datetime _dt;
};

}

#endif /* !__TIMEFILTER_DATETIME_FILTER_H */
