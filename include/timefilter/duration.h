/*
 * duration.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Saturday May 18, 2024
 */

#ifndef __TIMEFILTER_DURATION_H
#define __TIMEFILTER_DURATION_H

#include "timefilter/filter.h"

namespace timefilter {

class FilterDuration : public Filter {
 public:
     FilterDuration(Pointer filter, const Duration& duration) :
     Filter(FilterType::Duration), _filter(filter), _duration(duration) {
         validate();
     }

     static Pointer create(Pointer filter, const Duration& duration) {
         return std::make_shared<FilterDuration>(filter, duration);
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         auto range = _filter->next_range(dt);

         if (range.has_value()) {
             range = Range(range->start(), _duration);
         }

         return range;
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         auto range = _filter->prev_range(dt);

         if (range.has_value()) {
             range = Range(range->start(), _duration);
         }

         return range;
     }

     Pointer filter() const {
         return _filter;
     }

     const Duration& duration() const {
         return _duration;
     }

 protected:
     std::string _repr() const override {
         std::ostringstream sb;
         sb << _filter->repr() << " + " << _duration;
         return sb.str();
     }

 private:
     void validate() const {
         if (_duration <= Duration::zero()) {
             THROW(Error, "Duration must be greater than zero for FilterDuration.");
         }
     }

     Pointer _filter;
     const Duration _duration;
};

}


#endif /* !__TIMEFILTER_DURATION_H */
