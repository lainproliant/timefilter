/*
 * date.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#ifndef __TIMEFILTER_DATE_H
#define __TIMEFILTER_DATE_H

#include "timefilter/filter.h"

namespace timefilter {

class DateFilter : public Filter {
 public:
     DateFilter(const Date& date) : Filter(FilterType::Date), _date(date) { }

     static Pointer create(const Date& date) {
         return std::make_shared<DateFilter>(date);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         auto rg = range(dt.zone());
         if (dt < rg.start()) {
             return rg;
         }
         return {};
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         auto rg = range(dt.zone());
         if (dt >= rg.start()) {
             return rg;
         }
         return {};
     }

     const Date& date() const {
         return _date;
     }

 protected:
     std::string _repr() const override {
         return _date.isoformat();
     }

 private:
     Range range(const Zone& zone) const {
         return Range(
             Datetime(zone, _date),
             Datetime(zone, _date.advance_days(1))
         );
     }

     const Date _date;

};

}

#endif /* !__TIMEFILTER_DATE_H */
