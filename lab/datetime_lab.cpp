/*
 * datetime_lab.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#include <iostream>
#include "timefilter/datetime.h"

using namespace timefilter;

Filter::Pointer make_filter(const std::string& zone) {
    int year, month, day, hour, minute;

    std::cin >> year >> month >> day >> hour >> minute;
    auto filter_dt = Datetime(zone, Date(year, month, day), Time(hour, minute));
    return DatetimeFilter::create(filter_dt);
}

#include "timefilter/_lab/repr.h"
