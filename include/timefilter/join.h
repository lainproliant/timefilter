/*
 * join.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday May 13, 2024
 */

#ifndef __TIMEFILTER_JOIN_H
#define __TIMEFILTER_JOIN_H

#include "timefilter/core.h"

namespace timefilter {

// ------------------------------------------------------------------
class JoinFilter : public Filter {
 public:
     typedef std::shared_ptr<const JoinFilter> ConstPointer;
     typedef std::shared_ptr<JoinFilter> Pointer;

     JoinFilter() : Filter(FilterType::Join) { }

     static Pointer create() {
         return std::make_shared<JoinFilter>();
     }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         std::vector<std::optional<Range>> next_ranges;
         std::transform(_stack.begin(), _stack.end(),
                        std::back_inserter(next_ranges),
                        [=](auto filter) {
                            return filter->next_range(pivot);
                        });

         std::sort(next_ranges.begin(), next_ranges.end(), [](auto a, auto b) {
             if (! a.has_value()) {
                 return false;
             }

             if (! b.has_value()) {
                 return true;
             }

             return a.value().start() < b.value().start();
         });

         return next_ranges.front();
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         std::vector<std::optional<Range>> prev_ranges;
         std::transform(_stack.begin(), _stack.end(),
                        std::back_inserter(prev_ranges),
                        [=](auto filter) {
                            return filter->prev_range(pivot);
                        });

         std::sort(prev_ranges.begin(), prev_ranges.end(), [](auto a, auto b) {
             if (! a.has_value()) {
                 return true;
             }

             if (! b.has_value()) {
                 return false;
             }

             return a.value().start() < b.value().start();
         });

         return prev_ranges.back();
     }

     JoinFilter& push(Filter::Pointer filter) {
         _stack.push_back(filter);
         return *this;
     }

     bool empty() const {
         return _stack.empty();
     }

     Filter::Pointer simplify() const override {
         if (_stack.size() == 0) {
             return shared_from_this();
         }

         if (_stack.size() == 1) {
             return _stack.at(0)->simplify();
         }

         auto joined_filter = JoinFilter::create();

         for (auto filter : _stack) {
             joined_filter->push(filter->simplify());
         }

         return joined_filter;
     }

 protected:
     std::string _repr() const override {
         std::vector<std::string> reprs;
         std::transform(_stack.begin(), _stack.end(), std::back_inserter(reprs), [](auto p) {
             return p->repr();
         });
         return moonlight::str::join(reprs, ", ");
     }


 private:
     std::vector<Filter::Pointer> _stack;
};

}


#endif /* !__TIMEFILTER_JOIN_H */
