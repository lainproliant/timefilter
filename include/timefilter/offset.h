/*
 * offset.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday May 20, 2024
 */

#ifndef __TIMEFILTER_OFFSET_H
#define __TIMEFILTER_OFFSET_H

#include "timefilter/filter.h"

namespace timefilter {

class FilterOffset : public Filter {
 public:
     FilterOffset(Pointer filter, const Duration& offset) :
     Filter(FilterType::FilterOffset), _filter(filter), _offset(offset) { }

     static Pointer create(Pointer filter, const Duration& offset) {
         return std::make_shared<FilterOffset>(filter, offset);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         auto range = _filter->next_range(dt);

         if (range.has_value()) {
             range = Range(range->start() + _offset, Duration::of_millis(1));

             if (dt < range->start()) {
                 return range;
             }
         }

         return {};
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         auto range = _filter->prev_range(dt);

         if (range.has_value()) {
             range = Range(range->start() + _offset, Duration::of_millis(1));

             if (dt >= range->start()) {
                 return range;
             }
         }

         return {};
     }

     Pointer filter() const {
         return _filter;
     }

     const Duration& offset() const {
         return _offset;
     }

 protected:
     std::string _repr() const override {
         std::ostringstream sb;
         sb << _filter->repr() << " + " << _offset;
         return sb.str();
     }

 private:
     Pointer _filter;
     Duration _offset;
};

}


#endif /* !__TIMEFILTER_OFFSET_H */
