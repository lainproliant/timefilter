/*
 * weekday_of_month_lab.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#include <iostream>
#include "timefilter/weekday_of_month.h"

using namespace timefilter;

Filter::Pointer make_filter(const std::string& zone) {
    static const std::vector<std::string> weekday_names = {
        "sun", "mon", "tue", "wed", "thu", "fri", "sat"
    };
    std::string syntax_str;

    std::cin >> syntax_str;
    auto syntax_vec = moonlight::str::split(syntax_str, "/");
    auto iter = std::find(weekday_names.begin(), weekday_names.end(), moonlight::str::to_lower(syntax_vec[0]));
    if (iter == weekday_names.end()) {
        THROW(Error, "Invalid weekday spec: " + syntax_vec[0]);
    }

    const Weekday weekday = static_cast<Weekday>(std::distance(weekday_names.begin(), iter));
    const int offset = std::stoi(syntax_vec[1]);

    return WeekdayOfMonthFilter::create(weekday, offset);
}

#include "timefilter/_lab/repr.h"
