/*
 * lex.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday March 29, 2021
 *
 * Distributed under terms of the MIT license.
 */

#include <iostream>
#include "timefilter/parser.h"
#include "moonlight/ansi.h"

using namespace moonlight;
using namespace moonlight::date;
namespace fg = moonlight::fg;

int main() {
    auto parser = timefilter::Parser();
    std::string line;

    for (;;) {
        std::cout << fg::magenta("> ");
        if (! std::getline(std::cin, line)) {
            return 0;
        }
        try {
            auto tokens = parser.parse(line);
            for (auto tk : tokens) {
                std::cout << token_type_to_str(tk.type())
                          << " " << tk << std::endl;
            }

        } catch (const lex::NoMatchError& e) {
            std::cout << "no match" << std::endl;
        }
    }
}
