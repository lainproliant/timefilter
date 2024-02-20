/*
 * compile.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday May 13, 2024
 */

#include <iostream>
#include "timefilter/compiler.h"
#include "moonlight/ansi.h"

using namespace moonlight;
using namespace moonlight::date;
namespace fg = moonlight::fg;

int main() {
    auto parser = timefilter::Parser();
    auto compiler = timefilter::Compiler();
    std::string line;

    for (;;) {
        std::cout << fg::green("> ");
        if (! std::getline(std::cin, line)) {
            return 0;
        }

        try {
            auto tokens = parser.parse(line);
            auto filter = compiler.compile_filter(tokens);
            std::cout << filter->repr() << std::endl;

        } catch (const core::Exception& e) {
            std::cout << "ERROR: " << e.full_message() << std::endl;
        }
    }
}
