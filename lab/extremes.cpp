/*
 * extremes.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday June 3, 2021
 *
 * Distributed under terms of the MIT license.
 */

#include "moonlight/date.h"

using namespace moonlight::date;

int main() {
    Datetime a = Datetime::min();
    Datetime b = Datetime::max();

    std::cout << "min = " << a << std::endl;
    std::cout << "max = " << b << std::endl;
}
