#include "moonlight/test.h"
#include "timefilter/core.h"
#include <csignal>
#include <iostream>

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;

int main() {
    return TestSuite("timefilter core tests")
    .test("last_day_of_month()", [&]() {
        assert_equal(last_day_of_month(2048, Month::February), 29);
        assert_equal(last_day_of_month(2049, Month::February), 28);
        assert_equal(last_day_of_month(2048, Month::November), 30);
        assert_equal(last_day_of_month(2049, Month::December), 31);
    })
    .test("class Duration", [&]() {
        Duration dA(8670);
        std::cout << "dA = " << dA << std::endl;
        assert_equal(dA.days(), 6, "dA days");
        assert_equal(dA.hours(), 0, "dA hours");
        assert_equal(dA.minutes(), 30, "dA minutes");

        Duration dB(50, 30);
        std::cout << "dB = "<< dB << std::endl;
        assert_equal(dB.days(), 2, "dB days");
        assert_equal(dB.hours(), 2, "dB hours");
        assert_equal(dB.minutes(), 30, "dB minutes");

        Duration dC(21, 22, 23);
        std::cout << "dC = " << dC << std::endl;
        assert_equal(dC.days(), 21, "dC days");
        assert_equal(dC.hours(), 22, "dC hours");
        assert_equal(dC.minutes(), 23, "dC minutes");

        Duration dD = dA + dB;
        std::cout << "dD = dA + dB = " << dD << std::endl;
        assert_equal(dD.days(), 8, "dD days");
        assert_equal(dD.hours(), 3, "dD hours");
        assert_equal(dD.minutes(), 0, "dD minutes");

        Duration dE = dB + dC;
        std::cout << "dE = dB = dC = " << dE << std::endl;
        assert_equal(dE.days(), 24, "dE days");
        assert_equal(dE.hours(), 0, "dE hours");
        assert_equal(dE.minutes(), 53, "dE minutes");

        Duration dF = dA - dC;
        std::cout << "dF = dA - dC = " << dF << std::endl;
        assert_equal(dF.days(), -15, "dF days");
        assert_equal(dF.hours(), -21, "dF hours");
        assert_equal(dF.minutes(), -53, "dF minutes");
    })
    .test("class Date", [&]() {
        Date dateA;

        std::cout << "dateA = " << dateA << std::endl;
        std::cout << "dateA.to_utc_date() = " << dateA.to_utc_date() << std::endl;
        assert_equal(dateA.year(), 1970, "dateA year");
        assert_equal(dateA.month(), Month::January, "dateA month");
        assert_equal(dateA.day(), 1, "dateA day");
        assert_equal(dateA.to_utc_date(), (time_t)0ul, "dateA to_utc_date()");

        Date dateB(1988, Month::June, 8);
        std::cout << "dateB = " << dateB << std::endl;
        std::cout << "dateB.weekday() = " << static_cast<int>(dateB.weekday()) << std::endl;
        assert_equal(dateB.year(), 1988, "dateB year");
        assert_equal(dateB.month(), Month::June, "dateB month");
        assert_equal(dateB.day(), 8, "dateB day");
        assert_equal(dateB.to_utc_date(), (time_t)581731200ul, "dateB to_utc_date()");
        assert_equal(dateB.weekday(), Weekday::Wednesday, "dateB weekday()");
        assert_equal(dateB.end_of_month(), Date(1988, Month::June, 30), "dateB end_of_month()");
        assert_equal(dateB.start_of_month(), Date(1988, Month::June, 1), "dateB start_of_month()");
        assert_equal(dateB.advance_days(1), Date(1988, Month::June, 9), "dateB advance_days(1)");
        assert_equal(dateB.advance_days(30), Date(1988, Month::July, 8), "dateB advance_days(30)");
        assert_equal(dateB.recede_days(1), Date(1988, Month::June, 7), "dateB recede_days(1)");
        assert_equal(dateB.recede_days(30), Date(1988, Month::May, 9), "dateB recede_days(30)");

        Date dateC(2020, Month::December, 15);
        assert_equal(dateC.weekday(), Weekday::Tuesday, "dateC weekday()");
        assert_equal(dateC.weekday(), Weekday::Tuesday, "dateC weekday()");
        assert_equal(dateC.next_month(), Date(2021, Month::January, 1), "dateC next_month()");
        assert_equal(dateC.prev_month(), Date(2020, Month::November, 1), "dateC prev_month()");
        assert_true(dateA < dateB && dateB < dateC, "dateA < dateB < dateC");
        assert_true(dateC > dateB && dateB > dateA, "dateC > dateB > dateA");

        try {
            Date dateZZ(9595, Month::April, 31);
            assert_true(false, "dateZZ should have thrown ValueError.");

        } catch (const ValueError& e) {
            std::cout << "dateZZ -> Caught expected ValueError." << std::endl;
        }
    })
    .die_on_signal(SIGSEGV)
    .run();
}
