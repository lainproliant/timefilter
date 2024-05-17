/*
 * month_lab.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#include <iostream>
#include "timefilter/month.h"

using namespace timefilter;

Filter::Pointer make_filter(const std::string& zone) {
    int month;
    std::set<Month> months;

    for (;;) {
        std::cin >> month;

        if (month <= 0) {
            break;
        }

        months.insert(static_cast<Month>(month - 1));
    }

    return MonthFilter::create(months);
}

#include "timefilter/_lab/repr.h"
