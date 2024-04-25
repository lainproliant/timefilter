/*
 * duration_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday August 2, 2021
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_DURATION_FILTER_H
#define __TIMEFILTER_DURATION_FILTER_H

#include <memory>
#include "timefilter/filter/core.h"

namespace timefilter {

EXCEPTION_SUBTYPE(Error, DurationFilterValidationError);

// ------------------------------------------------------------------
class DurationFilter : public Filter {
 public:
     DurationFilter(filter_t base_filter, const Duration& duration)
     : Filter(FilterType::Duration), _base_filter(base_filter), _duration(duration) {
         validate();
     }

     static std::shared_ptr<DurationFilter> create(filter_t base_filter, const Duration& duration) {
         return std::make_shared<DurationFilter>(base_filter, duration);
     }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         auto base_range = _base_filter->next_range(pivot);
         if (base_range.has_value()) {
             return Range(base_range.value().start(), _duration);
         }

         return {};
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         auto base_range = _base_filter->prev_range(pivot);
         if (base_range.has_value()) {
             return Range(base_range.value().start(), _duration);
         }

         return {};
     }

 private:
     void validate() {
         if (_duration <= Duration::zero()) {
             throw DurationFilterValidationError(
                 "Duration must be greater than zero.");
         }
     }

     filter_t _base_filter;
     Duration _duration;
};

}  // namespace timefilter


#endif /* !__TIMEFILTER_DURATION_FILTER_H */
