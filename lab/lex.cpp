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
using namespace moonlight::date;

int main() {
    auto lex = lex::Lexer().throw_on_scan_failure(false);
    timefilter::I18nStrings i18n;
    auto grammar = timefilter::make_grammar(i18n);
    auto compiler = timefilter::Compiler(i18n, timefilter::make_factories());
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
        auto filter = compiler.compile(tokens);
        std::cout << *filter << std::endl;

        if (filter->is_absolute()) {
            auto abs_range = filter->absolute_range().value();
            std::cout << "abs range " << abs_range << std::endl;

        } else if (filter->is_never()) {
            std::cout << "NEVER" << std::endl;

        } else {
            auto now = Datetime::now();
            auto prev_range = filter->prev_range(now);

            if (! prev_range.has_value()) {
                std::cout << "prev NEVER" << std::endl;
            } else {
                std::cout << "prev " << prev_range.value() << std::endl;
            }

            auto next_range = filter->next_range(now);

            if (! next_range.has_value()) {
                std::cout << "next NEVER" << std::endl;
            } else {
                std::cout << "next " << next_range.value() << std::endl;
            }
        }
    }
}
