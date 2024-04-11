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
#include "timefilter/filter/core.h"

namespace timefilter {

EXCEPTION_SUBTYPE(Error, RangeFilterValidationError);

// ------------------------------------------------------------------
class RangeFilter : public Filter {
 public:
     explicit RangeFilter(filter_t start_filter, filter_t end_filter)
     : Filter(FilterType::Range), _start_filter(start_filter), _end_filter(end_filter) {
         validate();
     }

     static std::shared_ptr<RangeFilter> create(filter_t start, filter_t end) {
         return std::make_shared<RangeFilter>(start, end);
     }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         std::optional<Range> next_start_range = _start_filter->next_range(pivot);
         if (! next_start_range.has_value()) {
             return {};
         }

         std::optional<Range> next_end_range = _end_filter->next_range(next_start_range->start());
         if (! next_end_range.has_value()) {
             return {};
         }

         if (_end_filter->is_discrete()) {
             return Range(next_start_range->start(), next_end_range->start());
         }

         return Range(next_start_range->start(), next_end_range->end());
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         std::optional<Range> prev_start_range = _start_filter->prev_range(pivot);
         if (! prev_start_range.has_value()) {
             return {};
         }

         std::optional<Range> prev_end_range = _end_filter->next_range(prev_start_range->start());
         if (! prev_end_range.has_value()) {
             return {};
         }

         if (_end_filter->is_discrete()) {
             return Range(prev_start_range->start(), prev_end_range->end());
         }

         return Range(prev_start_range->start(), prev_end_range->end());
     }

 private:
     void validate() {
         if (_start_filter->is_absolute() && _end_filter->is_absolute()) {
             Datetime abs_start = _start_filter->absolute_range()->start();
             Datetime abs_end;

             if (_end_filter->is_discrete()) {
                 abs_end = _end_filter->absolute_range()->start();
             } else {
                 abs_end = _end_filter->absolute_range()->end();
             }

             if (abs_end <= abs_start) {
                 THROW(RangeFilterValidationError, "End must be after start.");
             }
         }
     }

     filter_t _start_filter;
     filter_t _end_filter;
};

class StaticRangeFilter : public Filter {
 public:
     explicit StaticRangeFilter(const Range& range) : Filter(FilterType::StaticRange), _range(range) { }

     static std::shared_ptr<StaticRangeFilter> create(const Range& range) {
         return std::make_shared<StaticRangeFilter>(range);
     }

     static std::shared_ptr<StaticRangeFilter> create(const Datetime& start, const Datetime& end) {
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
