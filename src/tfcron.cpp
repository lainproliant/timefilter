/*
 * tfcron.cpp - The `tfcron` scheduling tool.
 *
 * Reads a `tfcron` formatted crontab-like file, then prints output
 * providing information about upcoming, active, and past time ranges.
 *
 * The file format is very basic and looks like this:
 *
 * ```
 * MTWHF 6:30am; SU 9:00am -> alarm Wake Up!
 * TW 8:45am -> alarm Catch the Morning Ferry
 * ...
 * ```
 *
 * Each line is a "filter context", beginning with a `timefilter` expression,
 * then an arrow delimiter, then arbitrary text providing context to the
 * potentially repeating time range specified in the filter.
 *
 * Unlike most cron systems, `tfcron` doesn't execute any tasks on its own, and
 * it doesn't run as a daemon.  It is meant to be used with another program
 * which reads the past, current, and future ranges calculated from this tool
 * and schedules or runs tasks or triggers reminders based on the output, or
 * just on the command line as a calendaring tool using the `tfcron` crontab as
 * a source of calendar data.
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Tuesday April 15, 2025
 */

#include <vector>

#include "timefilter/compiler.h"
#include "moonlight/rx.h"
#include "moonlight/cli.h"
#include "moonlight/file.h"

using namespace timefilter;
using namespace moonlight;

// ------------------------------------------------------------------
const rx::Expression FILTER_CONTEXT_RX = rx::def("^(.*)\\s*->\\s*(.*)$");
const std::string USAGE =
"Usage: tfcron [flags] [options]\n"
"Flags:\n"
"  -C, --current      Print events that are happening now.\n"
"  -F, --future       Print events that will happen in the future.\n"
"  -P, --past         Print events that happened in the past/are over.\n"
"  -J, --json         Print events in JSON format for other programs to use.\n"
"  -d, --duration     Print the duration of the event, e.g. '1d5h'.\n"
"  -r, --range        Print the start and end instead of just the start of events.\n"
"  -D, --debug        Enable debug printing to stderr.\n"
"  -h, --help         Prints this help message and exits.\n"
"\n"
"Options:\n"
"  -c, --cron         Provide a filename to read crontab from, defaults to stdin.\n"
"  -f, --format       Provide a format string for datetimes, defaults to '%Y-%m-%d %H:%M %Z'.\n"
"  -p, --pivot        Provide a datetime or offset reference point, defaults to now.\n"
"  -w, --window       Provide a threshold for printing past/future events, defaults to '1w'.\n"
"  -z, --zone         Provide a timezone to use, defaults to your local time zone.\n"
"\n"
"To file a bug report, please visit https://github.com/lainproliant/timefilter/issues.\n";

// ------------------------------------------------------------------
struct FilterContext {
    Filter::Pointer filter;
    std::string expr;
    std::string context;
};

// ------------------------------------------------------------------
struct Config {
    Datetime pivot;
    std::string filename;
    std::string time_format;
    Duration window;
    bool print_json = false;
    bool print_future = false;
    bool print_current = false;
    bool print_past = false;
    bool print_range = false;
    bool print_duration = false;
    bool debug = false;
    Zone tz = Zone::local();
};

// ------------------------------------------------------------------
struct InstanceContext {
    Range range;
    FilterContext context;

    json::Object to_json(const Config& config) const {
        return json::Object()
        .set("range", json::Object().set("start", range.start().format(config.time_format))
                                    .set("end", range.end().format(config.time_format)))
        .set("filter", context.expr)
        .set("context", context.context);
    }
};

// ------------------------------------------------------------------
struct InstanceReport {
    Datetime pivot;
    std::vector<InstanceContext> past;
    std::vector<InstanceContext> current;
    std::vector<InstanceContext> future;

    json::Object to_json(const Config& config) const {
        const auto transformer = [=](const InstanceContext& ctx) {
            return ctx.to_json(config);
        };

        return json::Object()
        .set("pivot", pivot.format(config.time_format))
        .set("past", gen::stream(past).transform<json::Object>(transformer).collect())
        .set("current", gen::stream(current).transform<json::Object>(transformer).collect())
        .set("future", gen::stream(future).transform<json::Object>(transformer).collect());
    }
};

// ------------------------------------------------------------------
void die(const std::string& message) {
    std::cerr << "fatal: " << message << std::endl;
    std::exit(1);
}

// ------------------------------------------------------------------
void error(const file::Location& loc, const std::string& message) {
    std::cerr << "error: " << loc << " : " << message << std::endl;
}

// ------------------------------------------------------------------
std::string format_duration(const Duration& d) {
    std::ostringstream sb;

    if (d.factor() < 0) {
        sb << "-";
    }

    if (d.days() > 0) {
        sb << d.days() << "d";
    }

    if (d.hours() > 0) {
        sb << d.hours() << "h";
    }

    if (d.minutes() > 0) {
        sb << d.minutes() << "m";
    }

    if (d.seconds() > 0) {
        sb << d.seconds() << "s";
    }

    return sb.str();
}

// ------------------------------------------------------------------
std::vector<FilterContext> read_crontab(const Config& config,
                                        std::istream& infile) {
    std::vector<FilterContext> filters;

    auto input = file::BufferedInput(infile, config.filename);

    while (! input.is_exhausted()) {
        const auto loc = input.location();
        const auto line = str::trim(input.getline());
        const auto capture = rx::capture(FILTER_CONTEXT_RX, line);

        if (line.size() == 0) {
            continue;
        }

        if (capture) {
            auto filter_expr = str::trim(capture.group(1));
            auto context = str::trim(capture.group(2));

            auto filter = compile_filter(filter_expr);
            filters.push_back(FilterContext{filter, filter_expr, context});

        } else {
            error(loc, "Failed to parse cron line.");
        }
    }

    return filters;
}

// ------------------------------------------------------------------
InstanceReport make_instance_report(const std::vector<FilterContext>& filters,
                                    const Config& config) {
    InstanceReport report{.pivot=config.pivot};

    for (auto filter : filters) {
        auto prev_range = filter.filter->prev_range(config.pivot);
        auto next_range = filter.filter->next_range(config.pivot);

        if (next_range.has_value() && config.pivot - next_range->start() <= config.window) {
            report.future.push_back(InstanceContext{*next_range, filter});
        }

        if (prev_range.has_value() && config.pivot < prev_range->end()) {
            report.current.push_back(InstanceContext{*prev_range, filter});
            prev_range = filter.filter->prev_range(prev_range->start());
        }

        if (prev_range.has_value() && config.pivot - prev_range->end() <= config.window) {
            report.past.push_back(InstanceContext{*prev_range, filter});
        }
    }

    return report;
}

// ------------------------------------------------------------------
void print_instance(const InstanceContext& instance, const Config& config) {
    const std::string start_repr = instance.range.start().format(config.time_format);
    const std::string end_repr = instance.range.end().format(config.time_format);
    const std::string duration_repr = format_duration(instance.range.duration());

    std::cout << "- " << start_repr;

    if (config.print_range) {
        std::cout << " - " << end_repr;
    }

    if (config.print_duration) {
        std::cout << " [" << duration_repr << "]";
    }

    std::cout << ": " << instance.context.context << std::endl;
}

// ------------------------------------------------------------------
void print_instance_report(const InstanceReport& report,
                           const Config& config) {
    if (config.print_json) {
        json::write(std::cout, report.to_json(config), json::FormatOptions{.pretty=true});
        std::cout << std::endl;
        return;
    }

    if (config.print_future && report.future.size() > 0) {
        std::cout << "# Upcoming Events" << std::endl;
        gen::stream(report.future).for_each([config](auto& i) { print_instance(i, config); });
    }

    if (config.print_current && report.current.size() > 0) {
        std::cout << "# Current Events" << std::endl;
        gen::stream(report.current).for_each([config](auto& i) { print_instance(i, config); });
    }

    if (config.print_past && report.past.size() > 0) {
        std::cout << "# Past Events" << std::endl;
        gen::stream(report.past).for_each([config](auto& i) { print_instance(i, config); });
    }
}

// ------------------------------------------------------------------
Datetime parse_pivot(const std::string& pivot_expr, const Config& config) {
    Parser parser;
    Compiler compiler;
    Datetime pivot = Datetime::now(config.tz);
    int factor = 1;

    auto tokens = parser.parse(pivot_expr);

    if (tokens.size() == 0) {
        die("Pivot expression contained no datetime or duration.");
    }

    if (tokens[0].type() == TokenType::DURATION) {
        auto duration = compiler.compile_duration(tokens);
        pivot = Datetime::now(config.tz) + duration * factor;

    } else {
        auto filter = compiler.compile_filter(tokens);
        if (filter->is_absolute()) {
            pivot = filter->next_range(Datetime::min().zone(config.tz))->start();
        } else {
            auto maybe = filter->next_range(Datetime::now(config.tz));

            if (maybe.has_value()) {
                pivot = maybe->start();
            } else {
                die("Pivot filter has no next instance.");
            }
        }
    }

    return pivot;
}

// ------------------------------------------------------------------
int main(int argc, char** argv) {
    auto cmd = cli::parse(
        argc, argv,
        { "C", "current",
          "D", "debug",
          "d", "duration",
          "F", "future",
          "J", "json",
          "P", "past",
          "r", "range",
          "h", "help" },
        { "c", "cron",
          "f", "format",
          "p", "pivot",
          "w", "window",
          "z", "zone" });

    if (cmd.check("h", "help")) {
        std::cout << USAGE << std::endl;
        return 0;
    }

    Config config {
        .filename = cmd.get("c", "cron").value_or("-"),
        .time_format = cmd.get("f", "format").value_or("%Y-%m-%d %H:%M %Z"),
        .window = compile_duration(cmd.get("w", "window").value_or("3w")),
        .print_json = cmd.check("J", "json"),
        .print_future = cmd.check("F", "future"),
        .print_current = cmd.check("C", "current"),
        .print_past = cmd.check("P", "past"),
        .print_range = cmd.check("r", "range"),
        .print_duration = cmd.check("d", "duration"),
        .debug = cmd.check("D", "debug"),
    };

    config.pivot = Datetime::now(config.tz);

    if (cmd.get("z", "zone").has_value()) {
        config.tz = Zone(cmd.require("z", "zone"));
    }

    if (cmd.get("p", "pivot").has_value()) {
        auto pivot_expr = cmd.require("p", "pivot");
        config.pivot = parse_pivot(pivot_expr, config);
    }

    if (config.debug) {
        std::cerr << "pivot: " << config.pivot << std::endl;
    }

    std::vector<FilterContext> filters;

    if (config.filename == "-") {
        config.filename = "<stdin>";
        filters = read_crontab(config, std::cin);

    } else {
        auto infile = file::open_r(config.filename);
        filters = read_crontab(config, infile);
    }

    auto report = make_instance_report(filters, config);
    print_instance_report(report, config);

    return 0;
}
