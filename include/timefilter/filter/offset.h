/*
 * offset_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday August 2, 2021
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_OFFSET_FILTER_H
#define __TIMEFILTER_OFFSET_FILTER_H

#include <memory>
#include <algorithm>
#include <vector>
#include "timefilter/filter/core.h"
#include "moonlight/collect.h"

namespace timefilter {

// ------------------------------------------------------------------
class OffsetFilter : public Filter {
 public:
     OffsetFilter(filter_t base_filter, const std::vector<Duration>& durations)
     : Filter(FilterType::Duration), _base_filter(base_filter), _durations(durations) { }

     static std::shared_ptr<OffsetFilter> create(filter_t base_filter, const std::vector<Duration>& durations) {
         return std::make_shared<OffsetFilter>(base_filter, durations);
     }

     static std::shared_ptr<OffsetFilter> create(filter_t base_filter, const Duration& duration) {
         return create(base_filter, (const std::vector<Duration>){duration});
     }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         auto ranges = next_relative_ranges(pivot);

         for (auto range : ranges) {
             if (range.start() > pivot) {
                 return range;
             }
         }

         return {};
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         auto ranges = prev_relative_ranges(pivot);
         for (auto range : ranges) {
             if (pivot <= range.start()) {
                 return range;
             }
         }

         return {};
     }

 private:
     std::vector<Range> prev_relative_ranges(const Datetime& pivot) const {
         auto base_range = _base_filter->prev_range(pivot);
         if (! base_range.has_value()) {
             return {};
         }

         std::vector<Range> results;
         std::transform(_durations.begin(), _durations.end(), std::back_inserter(results), [&](auto d) {
             return Range(base_range.value().start(), d);
         });

         std::sort(results.begin(), results.end(), [&](const auto& a, const auto& b) {
             return b.start() < a.start();
         });

         return results;
     }

     std::vector<Range> next_relative_ranges(const Datetime& pivot) const {
         auto base_range = _base_filter->next_range(pivot);
         if (! base_range.has_value()) {
             return {};
         }

         std::vector<Range> results;
         std::transform(_durations.begin(), _durations.end(), std::back_inserter(results), [&](auto d) {
             return Range(base_range.value().start(), d);
         });

         std::sort(results.begin(), results.end(), [&](const auto& a, const auto& b) {
             return a.start() < b.start();
         });

         return results;
     }

     filter_t _base_filter;
     std::vector<Duration> _durations;
};

}  // namespace timefilter


#endif /* !__TIMEFILTER_OFFSET_FILTER_H */
