/*
 * static_range.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Friday May 17, 2024
 */

#ifndef __TIMEFILTER_STATIC_RANGE_H
#define __TIMEFILTER_STATIC_RANGE_H

#include "timefilter/filter.h"

namespace timefilter {

class StaticRangeFilter : public Filter {
 public:
     StaticRangeFilter(const Range& range) : Filter(FilterType::StaticRange), _range(range) { }

     static Pointer create(const Range& range) {
         return std::make_shared<StaticRangeFilter>(range);
     }

     static Pointer create(const Datetime& dt, const Duration& duration) {
         return create(Range(dt, duration));
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         if (dt < _range.start()) {
             return _range.zone(dt.zone());
         }
         return {};
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         if (dt >= _range.start()) {
             return _range.zone(dt.zone());
         }
         return {};
     }

     const Range& range() const {
         return _range;
     }

 protected:
     std::string _repr() const override {
         std::ostringstream sb;
         sb << _range;
         return sb.str();
     }

 private:
     const Range _range;
};

}


#endif /* !__TIMEFILTER_STATIC_RANGE_H */
