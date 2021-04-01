/*
 * lex.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday March 29, 2021
 *
 * Distributed under terms of the MIT license.
 */

#include "timefilter/parser.h"
#include "moonlight/ansi.h"
#include <iostream>

using namespace moonlight;

int main() {
    auto lex = lex::Lexer().throw_on_scan_failure(false);

    auto grammar = timefilter::make_grammar();
    std::string line;

    for(;;) {
        std::cout << fg::magenta("> ");
        if (! std::getline(std::cin, line)) {
            return 0;
        }
        auto tokens = lex.lex(grammar, line);
        for (auto tk : tokens) {
            std::cout << tk << std::endl;
        }
    }
}
