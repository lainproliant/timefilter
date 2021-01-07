/*
 * select_filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday January 7, 2021
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_SELECT_FILTER_H
#define __TIMEFILTER_SELECT_FILTER_H

#include "timefilter/core.h"
#include "moonlight/variadic.h"

namespace timefilter {

class SelectFilter : public Filter {
public:
    template<class... TD>
    static std::shared_ptr<SelectFilter> create(TD... params) {
        std::vector<FilterPointer> filters;
        moonlight::variadic::pass{filters.push_back(params)...};
        return std::make_shared<SelectFilter>(filters);
    }

private:
    SelectFilter(const std::vector<FilterPointer>& filters) : Filter(FilterType::Select), _filters(filters) { }

    std::vector<FilterPointer> _filters;
};

}

#endif /* !__TIMEFILTER_SELECT_FILTER_H */
