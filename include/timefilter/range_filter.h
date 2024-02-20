/*
 * range_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday January 4, 2021
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_RANGE_FILTER_H
#define __TIMEFILTER_RANGE_FILTER_H

#include <memory>
#include <string>
#include "timefilter/core.h"

namespace timefilter {

class RangeFilter : public Filter {
 public:
     explicit RangeFilter(const Range& range) : Filter(FilterType::Range), _range(range) { }

     static std::shared_ptr<RangeFilter> create(const Range& range) {
         return std::make_shared<RangeFilter>(range);
     }

     static std::shared_ptr<RangeFilter> create(const Datetime& start, const Datetime& end) {
         return create(Range(start, end));
     }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         if (pivot < _range.start()) {
             return _range;
         }
         return {};
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         if (pivot >= _range.start()) {
             return _range;
         }
         return {};
     }

 protected:
     std::string _repr() const override {
         return tfm::format("%s", _range);
     }

 private:
     Range _range;
};

}  // namespace timefilter

#endif /* !__TIMEFILTER_RANGE_FILTER_H */
