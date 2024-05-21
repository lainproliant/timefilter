/*
 * list.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Saturday May 18, 2024
 */

#ifndef __TIMEFILTER_LIST_H
#define __TIMEFILTER_LIST_H

#include "timefilter/filter.h"

namespace timefilter {

class FilterList : public Filter {
 public:
     typedef std::shared_ptr<FilterList> Pointer;

     FilterList() : Filter(FilterType::FilterList) { }
     FilterList(Pointer list) : Filter(FilterType::FilterList), _filters(list->_filters) { }

     static Pointer create() {
         return std::make_shared<FilterList>();
     }

     static Pointer create(Pointer list) {
         return std::make_shared<FilterList>(list);
     }

     bool empty() const {
         return _filters.empty();
     }

     size_t size() const {
         return _filters.size();
     }

     Pointer push(Filter::Pointer filter) {
         _filters.push_back(filter);
         return std::static_pointer_cast<FilterList>(shared_from_this());
     }

     Filter::Pointer pop() {
         if (empty()) {
             THROW(Error, "Attempted to pop from an empty FilterList.");
         }

         auto filter = _filters.back();
         _filters.pop_back();
         return filter;
     }

     std::optional<Range> next_range(const Datetime& dt) const override {
         std::vector<Range> ranges;

         for (auto filter : _filters) {
             auto rg = filter->next_range(dt);
             if (rg.has_value()) {
                 ranges.push_back(rg.value());
             }
         }

         if (ranges.size() == 0) {
             return {};
         }

         std::sort(ranges.begin(), ranges.end(), [](auto& rgA, auto& rgB) {
             return rgA.start() < rgB.start();
         });

         return ranges.front();
     }

     std::optional<Range> prev_range(const Datetime& dt) const override {
         std::vector<Range> ranges;

         for (auto filter : _filters) {
             auto rg = filter->prev_range(dt);
             if (rg.has_value()) {
                 ranges.push_back(rg.value());
             }
         }

         if (ranges.size() == 0) {
             return {};
         }

         std::sort(ranges.begin(), ranges.end(), [](auto& rgA, auto& rgB) {
             return rgA.start() < rgB.start();
         });

         return ranges.back();
     }

     Filter::Pointer simplify() const override {
         auto list = FilterList::create();

         for (auto filter : _filters) {
             list->push(filter->simplify());
         }

         if (list->_filters.size() == 1) {
             return list->_filters.at(0);
         }

         return list;
     }

 protected:
     std::string _repr() const override {
         std::vector<std::string> reprs;
         std::transform(_filters.begin(), _filters.end(), std::back_inserter(reprs), [](auto filter) {
             return filter->repr();
         });
         return moonlight::str::join(reprs, ",");
     }

 private:
     std::vector<Filter::Pointer> _filters;
};

}

#endif /* !__TIMEFILTER_LIST_H */
