/*
 * weekday_count.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#include "moonlight/date.h"

using namespace moonlight::date;

int main() {
    static const std::vector<std::string> weekday_names = {
        "sun", "mon", "tue", "wed", "thu", "fri", "sat"
    };
    std::string syntax_str;
    int count_years;

    std::cin >> syntax_str;
    std::cin >> count_years;
    auto syntax_vec = moonlight::str::split(syntax_str, "/");
    auto iter = std::find(weekday_names.begin(), weekday_names.end(), moonlight::str::to_lower(syntax_vec[0]));
    if (iter == weekday_names.end()) {
        std::cerr << "Invalid weekday spec: " << syntax_vec[0];
        return 1;
    }

    const Weekday weekday = static_cast<Weekday>(std::distance(weekday_names.begin(), iter));
    const int offset = std::stoi(syntax_vec[1]);
    Date month_start = Date::today().start_of_month();
    const Date last_day = Date(month_start.year() + count_years, month_start.month());
    std::vector<Datetime> dts;

    for (; month_start < last_day; month_start = month_start.next_month()) {
        Date d = month_start;

        for (int x = 0; x < offset; d = d.advance_days(1)) {
            if (d.weekday() == weekday) {
                x++;
            }
        }

        if (d.month() == month_start.month() && d.year() == month_start.year()) {
            Datetime dt = Datetime(d);
            dts.push_back(dt);
        }
    }

    std::cout << "Number of " << syntax_vec[0] << "/" << offset << " in the next " << count_years << " years: " << dts.size() << std::endl;

    Duration longest_distance = Duration::zero();
    Datetime lhs = Datetime::min();
    Datetime rhs = Datetime::min();

    for (size_t x = 0; dts.size() > 0 && x < dts.size() - 1; x++) {
        Duration diff = dts[x+1] - dts[x];
        if (diff > longest_distance) {
            longest_distance = diff;
            lhs = dts[x];
            rhs = dts[x+1];
        }
    }

    std::cout << "Longest distance between: " << longest_distance << std::endl;
    std::cout << lhs << " --> " << rhs << std::endl;
    return 0;
}
