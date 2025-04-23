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

    bool interactive() const {
        return cmd.check("I", "interactive");
    }
};

// ------------------------------------------------------------------
int repl(const moonlight::cli::CommandLine& cmd) {
    auto parser = timefilter::Parser();
    auto compiler = timefilter::Compiler();
    std::string line;

    std::cout << fg::green(tfm::format("Timefilter %s Duration REPL", VERSION)) << std::endl;

    for (;;) {
        std::cout << fg::green(" > ");
        if (! std::getline(std::cin, line)) {
            return 0;
        }

        try {
            auto tokens = parser.parse(line);
            if (cmd.check("D", "debug")) {
                gen::stream(tokens).for_each([](auto& tk) { std::cout << tk << std::endl; });
            }

            auto duration = compiler.compile_duration(tokens);
            std::cout << fg::cyan("duration: ") << duration << std::endl;

        } catch (const core::Exception& e) {
            std::cout << "ERROR: " << e.full_message() << std::endl;
        }
    }
}

// ------------------------------------------------------------------
int main(int argc, char** argv) {
    auto cmd = moonlight::cli::parse(argc, argv,
         { "I", "interactive",
           "m", "min",
           "M", "max",
           "D", "debug" },
         { "i", "input" }
    );

    if (cmd.check("I", "interactive")) {
        return repl(cmd);
    }
}
