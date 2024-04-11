/*
 * year_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday December 16, 2020
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_YEAR_FILTER_H
#define __TIMEFILTER_YEAR_FILTER_H

#include <memory>
#include <string>
#include "timefilter/filter/core.h"
#include "timefilter/filter/month.h"
#include "timefilter/filter/monthday.h"
#include "timefilter/filter/time.h"

namespace timefilter {

class YearFilter : public Filter {
 public:
     explicit YearFilter(int year) : Filter(FilterType::Year), _year(year) { }

     static std::shared_ptr<YearFilter> create(int year) {
         return std::make_shared<YearFilter>(year);
     }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         if (pivot >= Datetime(pivot.zone(), _year, Month::January, 1)) {
             return {};
         }

         return range(pivot.zone());
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         if (pivot.date().year() < _year) {
             return {};
         }

         return range(pivot.zone());
     }

     filter_t discrete() const override {
         return ListFilter::create({
             this->shared_from_this(),
             MonthFilter::create(Month::January),
             MonthdayFilter::create(1),
             TimeFilter::create(Time(0, 0))
         });
     }

 protected:
     std::string _repr() const override {
         return tfm::format("%d", _year);
     }

 private:
     Range range(const Zone& zone) const {
         return Range(
             Datetime(_year, Month::January, 1),
             Datetime(_year + 1, Month::January, 1)).zone(zone);
     }

     int _year;
};


}  // namespace timefilter

#endif /* !__TIMEFILTER_YEAR_FILTER_H */
