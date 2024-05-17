/*
 * date_lab.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#include <iostream>
#include "timefilter/date.h"

using namespace timefilter;

Filter::Pointer make_filter(const std::string& zone) {
    int year, month, day;

    std::cin >> year >> month >> day;
    auto filter_date = Date(year, month, day);
    return DateFilter::create(filter_date);
}

#include "timefilter/_lab/repr.h"
