/*
 * weekday_lab.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#include <iostream>
#include "timefilter/weekday.h"

using namespace timefilter;

Filter::Pointer make_filter(const std::string& zone) {
    const std::string WEEKDAY_CHRS = "UMTWHFS";
    std::string weekday_str;
    std::set<Weekday> weekdays;

    std::cin >> weekday_str;

    for (auto c : weekday_str) {
        auto upper_c = ::toupper(c);
        auto iter = std::find(WEEKDAY_CHRS.begin(), WEEKDAY_CHRS.end(), upper_c);
        if (iter == WEEKDAY_CHRS.end()) {
            THROW(Error, "Invalid weekday spec: " + moonlight::str::chr(c));
        }
        auto weekday_id = std::distance(WEEKDAY_CHRS.begin(), iter);
        weekdays.insert(static_cast<Weekday>(weekday_id));
    }

    return WeekdayFilter::create(weekdays);
}

#include "timefilter/_lab/repr.h"
