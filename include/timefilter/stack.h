/*
 * stack.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Friday December 25, 2020
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_STACK_H
#define __TIMEFILTER_STACK_H

#include "timefilter/core.h"

namespace timefilter {

// --------------------------------------------------------
class Stack : public Filter {
public:
    Stack() : Filter(FilterType::Stack) { }

    template<class T, class... TD>
    Stack& push(TD&&... params) {
        T* filter = new T(std::forward<TD>(params)...);
        _stack.push_back(filter);
        return *this;
    }

    virtual ~Stack() {
        while (_stack.size() > 0) {
            delete _stack.back();
            _stack.pop_back();
        }
    }

    virtual std::optional<Range> next_range(const Datetime& pivot) const override {
        Range pivot_range = Range::for_days(pivot.date(), 1).with_zone(pivot.zone());

        for (auto iter = _stack.begin(); iter != _stack.end(); iter++) {
            auto range = (*iter)->next_range(pivot_range.start());
            auto result = digest_clip_range(**iter, pivot_range, range);
            if (result.has_value()) {
                pivot_range = *result;
            } else {
                return {};
            }
        }

        return pivot_range;
    }

    virtual std::optional<Range> prev_range(const Datetime& pivot) const override {
        Range pivot_range = Range::for_days(pivot.date(), 1).with_zone(pivot.zone());

        for (auto iter = _stack.begin(); iter != _stack.end(); iter++) {
            auto range = (*iter)->prev_range(pivot_range.end() + Duration(1));
            auto result = digest_clip_range(**iter, pivot_range, range);
            if (result.has_value()) {
                pivot_range = *result;
            } else {
                return {};
            }
        }

        return pivot_range;
    }

private:
    std::optional<Range> digest_clip_range(const Filter& filter, const Range& pivot_range, std::optional<Range> range) const {
        if (! range.has_value()) {
            return {};
        }

        if (filter.should_clip()) {
            return range->clip_to(pivot_range);
        }

        return range;
    }

    std::vector<Filter*> _stack;
};

}

#endif /* !__TIMEFILTER_STACK_H */
