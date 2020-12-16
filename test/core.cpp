#include "moonlight/test.h"
#include "timefilter/core.h"
#include <csignal>

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter core tests")
    .test("last day of month", [&]() {
        assert_equal(last_day_of_month(2048, Month::February), 29);
        assert_equal(last_day_of_month(2049, Month::February), 28);
        assert_equal(last_day_of_month(2048, Month::November), 30);
        assert_equal(last_day_of_month(2049, Month::December), 31);
    })
    .die_on_signal(SIGSEGV)
    .run();
}
