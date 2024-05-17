/*
 * time_lab.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#include <iostream>
#include "timefilter/time.h"

using namespace timefilter;

Filter::Pointer make_filter(const std::string& zone) {
    std::string time_str;
    std::set<Time> times;

    for (;;) {
        std::cin >> time_str;

        if (moonlight::str::to_lower(time_str) == "end") {
            break;
        }

        auto hour_min_vec = moonlight::str::split(time_str, ":");
        times.insert(Time(std::stoi(hour_min_vec[0]), std::stoi(hour_min_vec[1])));
    }

    return TimeFilter::create(times);
}

#include "timefilter/_lab/repr.h"
