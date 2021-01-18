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

#include "timefilter/core.h"

namespace timefilter {

// ------------------------------------------------------------------
typedef std::vector<Filter::Pointer>::const_iterator ListIterator;

class ListViewFilter : public Filter {
public:
    ListViewFilter(ListIterator iter, ListIterator end) : Filter(FilterType::List), _iter(iter), _end(end) { }

    std::optional<Range> next_range(const Datetime& pivot) const override {
        if (empty()) return {};

        auto next = cdr();
        auto date = pivot;

        for (;;) {
            auto back_range = car()->prev_range(date);
            if (back_range.has_value() && back_range->contains(date) && ! next.empty()) {
                auto step_range = next.next_range(date);
                if (step_range.has_value() && back_range->contains(step_range->start())) {
                    return step_range;
                }
            }

            auto range = car()->next_range(date);
            if (range.has_value()) {
                if (! next.empty()) {
                    auto step_range = next.next_range(range->start());
                    if (step_range.has_value() && range->contains(step_range->start())) {
                        return step_range;
                    }

                } else {
                    return range;
                }

                date = range->end();

            } else {
                return {};
            }
        }
    }

    std::optional<Range> prev_range(const Datetime& pivot) const override {
        if (empty()) return {};

        auto next = cdr();
        auto date = pivot;

        for (;;) {
            auto range = car()->prev_range(date);
            if (range.has_value()) {
                if (! next.empty()) {
                    auto back_date = std::max(range->start(), date);
                    auto step_range = next.prev_range(back_date);
                    if (step_range.has_value() && range->contains(step_range->start())) {
                        return step_range;
                    }

                } else {
                    return range;
                }

                date = range->start() - Duration(1);

            } else {
                return {};
            }
        }
    }

    bool empty() const {
        return _iter == _end;
    }

    const Filter::Pointer car() const {
        return *_iter;
    }

    ListViewFilter cdr() const {
        return ListViewFilter(std::next(_iter), _end);
    }

private:
    ListIterator _iter;
    ListIterator _end;
};

// ------------------------------------------------------------------
class ListFilter : public Filter {
public:
    ListFilter() : Filter(FilterType::List) { }

    std::optional<Range> next_range(const Datetime& pivot) const {
        return view().next_range(pivot);
    }

    std::optional<Range> prev_range(const Datetime& pivot) const {
        return view().prev_range(pivot);
    }

    ListFilter& push(Filter::Pointer filter) {
        filter->validate_stack(gen::wrap(_stack.begin(), _stack.end()));
        _stack.push_back(filter);
        return *this;
    }

private:
    ListViewFilter view() const {
        return ListViewFilter(_stack.begin(), _stack.end());
    }

    std::vector<Filter::Pointer> _stack;
};

}

#endif /* !__TIMEFILTER_LIST_FILTER_H */
