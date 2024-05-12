/*
 * list_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday January 6, 2021
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_LIST_FILTER_H
#define __TIMEFILTER_LIST_FILTER_H

#include <set>
#include <memory>
#include <string>
#include <algorithm>
#include <vector>
#include "moonlight/collect.h"
#include "timefilter/filter/core.h"

namespace timefilter {

// ------------------------------------------------------------------
EXCEPTION_SUBTYPE(Error, ListFilterValidationError);

// ------------------------------------------------------------------
typedef std::vector<Filter::Pointer>::const_iterator ListIterator;

class StackViewFilter : public Filter {
 public:
     StackViewFilter(ListIterator iter, ListIterator end) : Filter(FilterType::List), _iter(iter), _end(end) { }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         if (empty()) return {};

         auto next = cdr();
         auto dt = pivot;

         for (;;) {
             auto back_range = car()->prev_range(dt);

             if (back_range.has_value() && back_range->contains(dt)) {
                 if (next.empty() && back_range == Range::for_days(dt.date(), 1) && back_range->start() >= pivot) {
                     return back_range;

                 } else {
                     auto step_range = next.next_range(dt);
                     if (step_range.has_value() && back_range->contains(step_range->start())) {
                         return step_range;
                     }
                 }
             }

             auto range = car()->next_range(dt);
             if (range.has_value()) {
                 if (! next.empty()) {
                     auto step_range = next.next_range(range->start());
                     if (step_range.has_value() && range->contains(step_range->start())) {
                         return step_range;
                     }

                 } else {
                     return range;
                 }

                 dt = range->end();

             } else {
                 return {};
             }
         }
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         if (empty()) return {};

         auto next = cdr();
         auto date = pivot;
         std::optional<Range> result;

         for (;;) {
             auto range = car()->prev_range(date);

             if (range.has_value()) {
                 if (! next.empty()) {
                     auto step_date = std::min(range->end(), date);
                     auto step_range = next.prev_range(step_date);
                     if (step_range.has_value() && range->contains(step_range->start())) {
                         result = step_range;
                         break;
                     }

                 } else {
                     result = range;
                     break;
                 }

                 date = range->start() - minutes(1);

             } else {
                 result = {};
                 break;
             }
         }

         return result;
     }

     bool empty() const {
         return _iter == _end;
     }

     const Filter::Pointer car() const {
         return *_iter;
     }

     StackViewFilter cdr() const {
         return StackViewFilter(std::next(_iter), _end);
     }

 protected:
     std::string _repr() const override {
         std::vector<std::string> reprs;
         std::transform(_iter, _end, std::back_inserter(reprs), [](auto p) {
             return p->repr();
         });
         return moonlight::str::join(reprs, ", ");
     }

 private:
     ListIterator _iter;
     ListIterator _end;
};

// ------------------------------------------------------------------
class ListFilter : public Filter {
 public:
     typedef std::shared_ptr<const ListFilter> ConstPointer;
     typedef std::shared_ptr<ListFilter> Pointer;

     ListFilter() : Filter(FilterType::List) { }

     static std::shared_ptr<ListFilter> create() {
         return std::make_shared<ListFilter>();
     }

     template<class C = std::initializer_list<std::shared_ptr<const Filter>>>
     static std::shared_ptr<ListFilter> create(const C& stack) {
         std::vector<Filter::Pointer> vec;
         for (auto filter : stack) {
             vec.push_back(filter);
         }
         return std::make_shared<ListFilter>(stack);
     }

     explicit ListFilter(const std::vector<Filter::Pointer> stack)
     : Filter(FilterType::List), _stack(sort_and_validate(stack)) { }

     std::optional<Range> next_range(const Datetime& pivot) const override {
         return view().next_range(pivot);
     }

     std::optional<Range> prev_range(const Datetime& pivot) const override {
         return view().prev_range(pivot);
     }

     ListFilter& push(Filter::Pointer filter) {
         std::vector<Filter::Pointer> filters(_stack);
         if (filter->type() == FilterType::List) {
             auto list = static_pointer_cast<const ListFilter>(filter);
             std::copy(list->_stack.begin(), list->_stack.end(), std::back_inserter(filters));

         } else {
             filters.push_back(filter);
         }
         _stack = sort_and_validate(filters);
         return *this;
     }

     bool empty() const {
         return _stack.empty();
     }

     filter_t discrete() const override {
         std::vector<filter_t> filters;
         std::copy(_stack.begin(), _stack.end(), std::back_inserter(filters));
         filter_t final_filter = filters.back();
         filters.pop_back();
         filters.push_back(final_filter->discrete());
         return ListFilter::create(filters);
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
     static std::vector<Filter::Pointer> sort_and_validate(const std::vector<Filter::Pointer>& stack) {
         std::set<int> orders;
         auto sorted = moonlight::collect::sorted(
             stack,
             [](const auto& filterA, const auto& filterB) {
                 if (filterA->order() < 0 || filterB->order() < 0) {
                     throw ListFilterValidationError("Unorderable filter can't be added to a list.");
                 }
                 return filterA->order() < filterB->order();
             });
         for (auto filter : sorted) {
             if (orders.find(filter->order()) == orders.end()) {
                 orders.insert(filter->order());
             } else {
                 throw ListFilterValidationError("Multiple filters with the same order provided.");
             }
         }
         return sorted;
     }

     StackViewFilter view() const {
         return StackViewFilter(_stack.begin(), _stack.end());
     }

     bool is_discrete() const override {
         for (auto filter : _stack) {
             if (filter->is_discrete()) {
                 return true;
             }
         }

         return false;
     }

     std::vector<Filter::Pointer> _stack;
};

}  // namespace timefilter

#endif /* !__TIMEFILTER_LIST_FILTER_H */
