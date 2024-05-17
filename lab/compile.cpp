/*
 * timefilter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday May 13, 2024
 */

#include <iostream>
#include "timefilter/compiler.h"
#include "moonlight/cli.h"
#include "moonlight/ansi.h"

using namespace moonlight;
using namespace moonlight::date;
namespace fg = moonlight::fg;

// ------------------------------------------------------------------
const std::string VERSION = "1.0";

// ------------------------------------------------------------------
struct Context {
    moonlight::cli::CommandLine cmd;
    Datetime pivot = Datetime::now();

    bool interactive() const {
        return cmd.check("I", "interactive");
    }
};

// ------------------------------------------------------------------
int repl(const moonlight::cli::CommandLine& cmd, const Datetime& starting_pivot) {
    Datetime pivot = starting_pivot;
    auto parser = timefilter::Parser();
    auto compiler = timefilter::Compiler();
    std::string line;

    std::cout << fg::green(tfm::format("Timefilter %s REPL", VERSION)) << std::endl;

    for (;;) {
        std::cout << fg::yellow(tfm::format("Pivot: %s", pivot));
        std::cout << fg::green(" > ");
        if (! std::getline(std::cin, line)) {
            return 0;
        }

        try {
            auto tokens = parser.parse(line);
            auto filter = compiler.compile_filter(tokens);
            std::cout << fg::cyan("filter: ") << filter->repr() << std::endl;
            auto next = filter->next_range(pivot);
            auto prev = filter->prev_range(pivot);

            std::cout << fg::cyan("next: ");
            if (next.has_value()) {
                std::cout << next.value();
            } else {
                std::cout << "NEVER";
            }
            std::cout << std::endl;

            std::cout << fg::cyan("prev: ");
            if (prev.has_value()) {
                std::cout << prev.value();
            } else {
                std::cout << "NEVER";
            }
            std::cout << std::endl;

        } catch (const core::Exception& e) {
            std::cout << "ERROR: " << e.full_message() << std::endl;
        }
    }

}

// ------------------------------------------------------------------
int main(int argc, char** argv) {
    auto pivot = Datetime::now();

    auto cmd = moonlight::cli::parse(argc, argv,
         { "I", "interactive",
           "m", "min",
           "M", "max",
           "D", "debug" },
         { "p", "pivot",
           "i", "input" }
    );

    if (cmd.check("I", "interactive")) {
        return repl(cmd, pivot);
    }
}
