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

    ListViewFilter cdr() const {
        return ListViewFilter(std::next(_iter), _end);
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
    ListFilter() : Filter(FilterType::List) { }

    std::optional<Range> next_range(const Datetime& pivot) const override {
        return view().next_range(pivot);
    }

    std::optional<Range> prev_range(const Datetime& pivot) const override {
        return view().prev_range(pivot);
    }

    ListFilter& push(Filter::Pointer filter) {
        filter->validate_stack(gen::wrap(_stack.begin(), _stack.end()));
        _stack.push_back(filter);
        return *this;
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
    ListViewFilter view() const {
        return ListViewFilter(_stack.begin(), _stack.end());
    }

    std::vector<Filter::Pointer> _stack;
};

}

#endif /* !__TIMEFILTER_LIST_FILTER_H */
