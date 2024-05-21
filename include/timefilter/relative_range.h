/*
 * relative_range.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Friday May 17, 2024
 */

#ifndef __TIMEFILTER_RELATIVE_RANGE_H
#define __TIMEFILTER_RELATIVE_RANGE_H

#include "timefilter/filter.h"

namespace timefilter {

class RelativeRangeFilter : public Filter {
 public:
     RelativeRangeFilter(Pointer start_filter, Pointer end_filter) : Filter(FilterType::RelativeRange), _start_filter(start_filter), _end_filter(end_filter) { }

     static Pointer create(Pointer start_filter, Pointer end_filter) {
         return std::make_shared<RelativeRangeFilter>(start_filter, end_filter);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         auto start_rg = _start_filter->next_range(dt);
         if (start_rg.has_value()) {
             auto end_rg = _end_filter->next_range(start_rg->start());
             if (end_rg.has_value()) {
                 return Range(
                     start_rg->start(),
                     end_rg->start()
                 );
             }
         }

         return {};
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         auto start_rg = _start_filter->prev_range(dt);
         if (start_rg.has_value()) {
             auto end_rg = _end_filter->next_range(start_rg->start());
             if (end_rg.has_value()) {
                 return Range(
                     start_rg->start(),
                     end_rg->start()
                 );
             }
         }

         return {};
     }

     Pointer start_filter() const {
         return _start_filter;
     }

     Pointer end_filter() const {
         return _end_filter;
     }

 protected:
     std::string _repr() const override {
         std::ostringstream sb;
         sb << _start_filter->repr() << ", " << _end_filter->repr();
         return sb.str();
     }

 private:
     Pointer _start_filter;
     Pointer _end_filter;
};

}


#endif /* !__TIMEFILTER_RELATIVE_RANGE_H */
