/*
 * repr.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#include <iostream>
#include "moonlight/date.h"

int main() {
    std::string zone;
    int year, month, day, hour, minute;

    std::cin >> zone;
    auto filter = make_filter(zone);

    std::cout << "filter: " << *filter << std::endl;

    while (std::cin >> year >> month >> day >> hour >> minute) {
        auto pivot_dt = moonlight::date::Datetime(zone, moonlight::date::Date(year, month, day), moonlight::date::Time(hour, minute));


        auto next_range = filter->next_range(pivot_dt);
        if (next_range.has_value()) {
            std::cout << "next: " << *next_range << std::endl;
        } else {
            std::cout << "next: (none)" << std::endl;
        }

        auto prev_range = filter->prev_range(pivot_dt);
        if (prev_range.has_value()) {
            std::cout << "prev: " << *prev_range << std::endl;
        } else {
            std::cout << "prev: (none)" << std::endl;
        }
    }

    return 0;
}
