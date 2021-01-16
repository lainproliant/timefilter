/*
 * stack_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday January 6, 2021
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_STACK_FILTER_H
#define __TIMEFILTER_STACK_FILTER_H

#include "timefilter/core.h"

namespace timefilter {

// ------------------------------------------------------------------
typedef std::vector<Filter::Pointer>::const_iterator StackIterator;

class StackViewFilter : public Filter {
public:
    StackViewFilter(StackIterator iter, StackIterator end) : Filter(FilterType::Stack), _iter(iter), _end(end) { }

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
            if (range.has_value() && ! next.empty()) {
                // TODO
            } else {
                return range;
            }

            if (range.has_value()) {
                auto next_date = std::max(date, range->start());
                if (! next.empty()) {
                    auto step_range = next.next_range(range->start());
                    if (step_range.has_value()) {
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

        }

        for (;;) {
            auto range = car()->prev_range(pivot);

            if (range.has_value() && ! next.empty() && range->contains(pivot)) {
                auto next_pivot = std::min(pivot, range->end());
                auto next_range = next.prev_range(next_pivot);
                if (next_range.has_value() && range->contains(next_range->start())) {
                    return next_range;
                }
            }

            return range;

        }
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

private:
    StackIterator _iter;
    StackIterator _end;
};

// ------------------------------------------------------------------
class StackFilter : public Filter {
public:
    StackFilter() : Filter(FilterType::Stack) { }

    std::optional<Range> next_range(const Datetime& pivot) const {
        return view().next_range(pivot);
    }

    std::optional<Range> prev_range(const Datetime& pivot) const {
        return view().prev_range(pivot);
    }

    StackFilter& push(Filter::Pointer filter) {
        filter->validate_stack(gen::wrap(_stack.begin(), _stack.end()));
        _stack.push_back(filter);
        return *this;
    }

private:
    StackViewFilter view() const {
        return StackViewFilter(_stack.begin(), _stack.end());
    }

    std::vector<Filter::Pointer> _stack;
};

}

#endif /* !__TIMEFILTER_STACK_FILTER_H */
