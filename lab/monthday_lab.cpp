/*
 * month_lab.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#include <iostream>
#include "timefilter/monthday.h"

using namespace timefilter;

Filter::Pointer make_filter(const std::string& zone) {
    int day;
    std::set<int> days;

    for (;;) {
        std::cin >> day;

        if (day == 0) {
            break;
        }

        days.insert(day);
    }

    return MonthdayFilter::create(days);
}

#include "timefilter/_lab/repr.h"
