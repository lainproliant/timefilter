/*
 * datetime.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#ifndef __TIMEFILTER_DATETIME_H
#define __TIMEFILTER_DATETIME_H

#include "timefilter/filter.h"

namespace timefilter {

class DatetimeFilter : public Filter {
 public:
     DatetimeFilter(const Datetime& dt) : Filter(FilterType::Datetime), _dt(dt) { }

     static Pointer create(const Datetime& dt) {
         return std::make_shared<DatetimeFilter>(dt);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         if (dt < _dt) {
             return range(dt.zone());
         }
         return {};

     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         if (dt >= _dt) {
             return range(dt.zone());
         }
         return {};
     }

     const Datetime& dt() const {
         return _dt;
     }

 protected:
     std::string _repr() const override {
         return _dt.isoformat();
     }

 private:
     Range range(const Zone& zone) const {
         return Range(
             _dt.zone(zone),
             _dt.zone(zone) + Duration::of_seconds(1)
         );
     }

     const Datetime _dt;
};

}


#endif /* !__TIMEFILTER_DATETIME_H */
