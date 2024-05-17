/*
 * year.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#ifndef __TIMEFILTER_YEAR_H
#define __TIMEFILTER_YEAR_H

#include "timefilter/filter.h"

namespace timefilter {

class YearFilter : public Filter {
 public:
     YearFilter(int year) : Filter(FilterType::Year), _year(year) { }

     static Pointer create(int year) {
         return std::make_shared<YearFilter>(year);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         auto range = year_range(dt.zone());

         if (dt < range.start()) {
             return range;
         }

         return {};
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         auto range = year_range(dt.zone());

         if (dt >= range.start()) {
             return range;
         }

         return {};
     }

     int year() const {
         return _year;
     }

 protected:
     std::string _repr() const override {
         std::ostringstream sb;
         sb << _year;
         return sb.str();
     }

 private:
     Range year_range(const Zone& zone) const {
         const Date this_year = Date(_year, Month::January);
         const Date next_year = Date(_year + 1, Month::January);
         return Range(
             Datetime(zone, this_year),
             Datetime(zone, next_year)
         );
     }

     const int _year;
};

}

#endif /* !__TIMEFILTER_YEAR_H */
