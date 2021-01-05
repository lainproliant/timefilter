/*
 * core.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday December 10, 2020
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __TIMEFILTER_CORE_H
#define __TIMEFILTER_CORE_H

#include "tinyformat/tinyformat.h"
#include "moonlight/exceptions.h"
#include "moonlight/maps.h"
#include <map>
#include <ctime>
#include <cstdlib>
#include <cstring>

namespace timefilter {

const std::string DEFAULT_FORMAT = "%Y-%m-%d %H:%M:%S";

// --------------------------------------------------------
class ValueError : public moonlight::core::Exception {
    using Exception::Exception;
};

// --------------------------------------------------------
enum class Month {
    January,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December
};

// --------------------------------------------------------
enum class Weekday {
    Sunday,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday
};

// --------------------------------------------------------
inline int last_day_of_month(int year, Month month) {
    switch(month) {
    case Month::April:
    case Month::June:
    case Month::November:
    case Month::September:
        return 30;

    case Month::January:
    case Month::March:
    case Month::May:
    case Month::July:
    case Month::August:
    case Month::October:
    case Month::December:
        return 31;

    case Month::February:
        return ((!(year % 4) && year % 100) || !(year % 400)) ? 29 : 28;
    }
}

// --------------------------------------------------------
class Zone {
public:
    Zone(const std::string& tz_name) : _tz_name(tz_name) { }

    static Zone local() {
        return Zone();
    }

    std::string name(bool is_dst = false) const {
        auto env_tz = Zone::get_env_tz();
        set_env_tz(_tz_name);
        std::string sys_tz_name(tzname[is_dst ? 1 : 0]);
        set_env_tz(env_tz);
        return sys_tz_name;
    }

    struct tm mk_struct_tm(time_t utime) const {
        struct tm local_time;
        auto env_tz = Zone::get_env_tz();
        set_env_tz(_tz_name);
        local_time = *localtime(&utime);
        set_env_tz(env_tz);
        return local_time;
    }

    time_t mk_timestamp(struct tm local_time) const {
        auto env_tz = get_env_tz();
        set_env_tz(_tz_name);
        time_t utime = mktime(&local_time);
        set_env_tz(env_tz);
        return utime;
    }

    std::string strftime(const std::string& format, struct tm local_time) const {
        auto env_tz = get_env_tz();
        set_env_tz(_tz_name);
        const int bufsize = 1024;
        char buffer[bufsize];
        ::strftime(buffer, bufsize, format.c_str(), &local_time);
        std::string result(buffer);
        set_env_tz(env_tz);
        return result;
    }

private:
    Zone() : _tz_name({}) { }

    static std::optional<std::string> get_env_tz() {
        char* result = getenv("TZ");
        if (result == nullptr) {
            return {};
        }
        return result;
    }

    static void set_env_tz(const std::optional<std::string>& tz_name = {}) {
        if (tz_name.has_value()) {
            auto env = tfm::format("TZ=%s", tz_name.value());
            putenv(strdup(env.c_str()));
        } else {
            unsetenv("TZ");
        }
        tzset();
    }

    std::optional<std::string> _tz_name;
};

const Zone UTC = Zone("UTC");
const Zone LOCAL = Zone::local();

// --------------------------------------------------------
class Duration {
public:
    Duration() : _minutes(0) { }
    Duration(int minutes) : _minutes(minutes) { }
    Duration(int hours, int minutes) : Duration(hours * 60 + minutes) { }
    Duration(int days, int hours, int minutes) : Duration(days * 24 + hours, minutes) { }

    static Duration of_minutes(int minutes) {
        return Duration(minutes);
    }

    static Duration of_hours(int hours) {
        return Duration(hours, 0);
    }

    static Duration of_days(int days) {
        return Duration(days, 0, 0);
    }

    int days() const {
        return total_hours() / 24;
    }

    int total_hours() const {
        return total_minutes() / 60;
    }

    int hours() const {
        return total_hours() % 24;
    }

    int total_minutes() const {
        return _minutes;
    }

    int minutes() const {
        return _minutes % 60;
    }

    Duration with_days(int days) const {
        return Duration(days, hours(), minutes());
    }

    Duration with_hours(int hours) const {
        return Duration(days(), hours, minutes());
    }

    Duration with_minutes(int minutes) const {
        return Duration(days(), hours(), minutes);
    }

    Duration operator+(const Duration& rhs) const {
        return Duration(_minutes + rhs._minutes);
    }

    Duration operator-(const Duration& rhs) const {
        return Duration(_minutes - rhs._minutes);
    }

    bool operator<(const Duration& rhs) const {
        return _minutes < rhs._minutes;
    }

    bool operator>(const Duration& rhs) const {
        return (! (*this < rhs)) && (! (*this == rhs));
    }

    bool operator<=(const Duration& rhs) const {
        return (*this < rhs) || (*this == rhs);
    }

    bool operator>=(const Duration& rhs) const {
        return (! (*this < rhs) || (*this == rhs));
    }

    bool operator==(const Duration& rhs) const {
        return _minutes == rhs._minutes;
    }

    bool operator!=(const Duration& rhs) const {
        return ! (*this == rhs);
    }

    friend std::ostream& operator<<(std::ostream& out, const Duration& duration) {
        tfm::format(out, "Duration<%dd %02d:%02d>",
                    duration.days(),
                    duration.hours(),
                    duration.minutes());
        return out;
    }

private:
    int _minutes;
};

// --------------------------------------------------------
class Date {
public:
    /**
     * Create a date at the UNIX Epoch.
     */
    Date() : Date(1970, Month::January, 1) { }

    /**
     * Create a date at the start of the given month.
     */
    Date(int year, Month month) : Date(year, month, 1) { }

    /**
     * Throws ValueError if the date is invalid.
     */
    Date(int year, Month month, int day) : _year(year), _month(month), _day(day) {
        validate();
    }

    /**
     * Advance forward the given number of calendar days.
     */
    Date advance_days(int days) const {
        Date date = *this;

        for (int x = 0; x < days; x++) {
            if (date == end_of_month()) {
                date = date.next_month();
            } else {
                date = date.with_day(date.day() + 1);
            }
        }

        return date;
    }

    /**
     * Recede backward the given number of calendar days.
     */
    Date recede_days(int days) const {
        Date date = *this;

        for (int x = 0; x < days; x++) {
            if (date == start_of_month()) {
                date = date.prev_month().end_of_month();
            } else {
                date = date.with_day(date.day() - 1);
            }
        }

        return date;
    }

    /**
     * Implements Zeller's rule to determine the weekday.
     */
    Weekday weekday() const {
        int adjustment, mm, yy;

        adjustment = (14 - nmonth()) / 12;
        mm = nmonth() + 12 * adjustment - 2;
        yy = year() - adjustment;
        return static_cast<Weekday>(
            ((day() + (13 * mm - 1) / 5 +
              yy + yy / 4 - yy / 100 + yy / 400) % 7));
    }

    int nweekday() const {
        return static_cast<int>(weekday());
    }

    int year() const {
        return _year;
    }

    Month month() const {
        return _month;
    }

    int nmonth() const {
        return static_cast<int>(month()) + 1;
    }

    int day() const {
        return _day;
    }

    /**
     * Throws ValueError if the date is invalid.
     */
    Date with_year(int year) const {
        return Date(year, month(), day());
    }

    /**
     * Throws ValueError if the date is invalid.
     */
    Date with_month(Month month) const {
        return Date(year(), month, day());
    }

    /**
     * Throws ValueError if the date is invalid or the month is out of range.
     */
    Date with_nmonth(int nmonth) const {
        if (nmonth < 1 || nmonth > 12) {
            throw ValueError("Numeric month value is out of range (1-12).");
        }

        return with_month(static_cast<Month>(nmonth - 1));
    }

    /**
     * Throws ValueError if the date is invalid.
     */
    Date with_day(int day) const {
        return Date(year(), month(), day);
    }

    /**
     * Advance to the first day of this date's next month.
     */
    Date next_month() const {
        Date date = with_day(1);

        if (month() == Month::December) {
            date = date.with_year(date.year() + 1).with_month(Month::January);
        } else {
            date = date.with_nmonth(nmonth() + 1);
        }

        return date;
    }

    /**
     * Recede to the first day of this date's last month.
     */
    Date prev_month() const {
        Date date = with_day(1);

        if (month() == Month::January) {
            date = date.with_year(date.year() - 1).with_month(Month::December);
        } else {
            date = date.with_nmonth(nmonth() - 1);
        }

        return date;
    }

    /**
     * Advance to the last day of this date's month.
     */
    Date end_of_month() const {
        return with_day(last_day_of_month(year(), month()));
    }

    /**
     * Revert back to the first day of this date's month.
     */
    Date start_of_month() const {
        return with_day(1);
    }

    bool operator<(const Date& rhs) const {
        if (year() == rhs.year()) {
            if (month() == rhs.month()) {
                return day() < rhs.day();
            }
            return month() < rhs.month();
        }
        return year() < rhs.year();
    }

    bool operator>(const Date& rhs) const {
        return (! (*this < rhs)) && (! (*this == rhs));
    }

    bool operator<=(const Date& rhs) const {
        return (*this < rhs) || (*this == rhs);
    }

    bool operator>=(const Date& rhs) const {
        return (! (*this < rhs) || (*this == rhs));
    }

    bool operator==(const Date& rhs) const {
        return (year() == rhs.year() &&
                month() == rhs.month() &&
                day() == rhs.day());
    }

    bool operator!=(const Date& rhs) const {
        return ! (*this == rhs);
    }

    Date& operator++() {
        *this = advance_days(1);
        return *this;
    }

    Date operator++(int) {
        Date date = *this;
        *this = advance_days(1);
        return date;
    }

    Date& operator--() {
        *this = recede_days(1);
        return *this;
    }

    Date operator--(int) {
        Date date = *this;
        *this = recede_days(1);
        return date;
    }

    friend std::ostream& operator<<(std::ostream& out, const Date& date) {
        tfm::format(out, "Date<%04d-%02d-%02d>",
                    date.year(),
                    date.nmonth(),
                    date.day());
        return out;
    }

private:
    void validate() {
        if (day() < 1) {
            throw ValueError("Day is out of range (less than 1).");
        }

        if (day() > last_day_of_month(year(), month())) {
            throw ValueError("Day is out of range (greater than last day of month).");
        }
    }

    int _year;
    Month _month;
    int _day;
};

// --------------------------------------------------------
class Time {
public:
    Time() : Time(0, 0) { }
    Time(int hour, int minute) : _minutes(hour * 60 + minute) {
        validate();
    }

    int hour() const {
        return _minutes / 60;
    }

    int minute() const {
        return _minutes % 60;
    }

    Time with_hour(int hour) const {
        return Time(hour, minute());
    }

    Time with_minute(int minute) const {
        return Time(hour(), minute);
    }

    bool operator<(const Time& rhs) const {
        return _minutes < rhs._minutes;
    }

    bool operator>(const Time& rhs) const {
        return (! (*this < rhs)) && (! (*this == rhs));
    }

    bool operator<=(const Time& rhs) const {
        return (*this < rhs) || (*this == rhs);
    }

    bool operator>=(const Time& rhs) const {
        return (! (*this < rhs) || (*this == rhs));
    }

    bool operator==(const Time& rhs) const {
        return _minutes == rhs._minutes;
    }

    bool operator!=(const Time& rhs) const {
        return ! (*this == rhs);
    }

    friend std::ostream& operator<<(std::ostream& out, const Time& time) {
        tfm::format(out, "Time<%02d:%02d>", time.hour(), time.minute());
        return out;
    }

private:
    void validate() {
        if (_minutes < 0 || _minutes >= 60 * 24) {
            throw ValueError("Time is out of range.");
        }
    }

    int _minutes;
};

// --------------------------------------------------------
class Datetime {
public:
    Datetime() : _date(), _time(), _zone(UTC) { }
    Datetime(const Zone& zone) : _date(), _time(), _zone(zone) { }
    Datetime(const Date& date) : _date(date), _time(), _zone(UTC) { }
    Datetime(const Date& date, const Zone& zone) : _date(date), _time(), _zone(zone) { }
    Datetime(const Date& date, const Time& time) : _date(date), _time(time), _zone(UTC) { }
    Datetime(const Date& date, const Time& time, const Zone& zone) : _date(date), _time(time), _zone(zone) { }
    Datetime(int year, Month month) : _date(year, month), _time(), _zone(UTC) { }
    Datetime(int year, Month month, const Zone& zone) : _date(year, month), _time(), _zone(zone) { }
    Datetime(int year, Month month, int day) : _date(year, month, day), _time(), _zone(UTC) { }
    Datetime(int year, Month month, int day, const Zone& zone) : _date(year, month, day), _time(), _zone(zone) { }
    Datetime(int year, Month month, int day, int hour, int minute) : _date(year, month, day), _time(hour, minute), _zone(UTC) { }
    Datetime(int year, Month month, int day, int hour, int minute, const Zone& zone) : _date(year, month, day), _time(hour, minute), _zone(zone) { }

    Datetime(const struct tm& local_time, const Zone& zone) : _zone(zone) {
        load_struct_tm(local_time);
    }

    Datetime(const struct tm& local_time) : Datetime(local_time, UTC) {}

    Datetime(time_t timestamp) : _zone(UTC) {
        struct tm local_time = zone().mk_struct_tm(timestamp);
        load_struct_tm(local_time);
    }

    static Datetime now() {
        return Datetime(::time(nullptr));
    }

    static Datetime now(const Zone& zone) {
        return now().at_zone(zone);
    }

    struct tm to_struct_tm() const {
        return (struct tm) {
            .tm_sec = 0,
            .tm_min = time().minute(),
            .tm_hour = time().hour(),
            .tm_mday = date().day(),
            .tm_mon = date().nmonth() - 1,
            .tm_year = date().year() - 1900,
            .tm_wday = date().nweekday(),
            .tm_isdst = -1,
        };
    }

    time_t to_timestamp() const {
        return zone().mk_timestamp(to_struct_tm());
    }

    const Date& date() const {
        return _date;
    }

    int year() const {
        return date().year();
    }

    Month month() const {
        return date().month();
    }

    int day() const {
        return date().day();
    }

    const Time& time() const {
        return _time;
    }

    int hour() const {
        return time().hour();
    }

    int minute() const {
        return time().minute();
    }

    const Zone& zone() const {
        return _zone;
    }

    Datetime with_date(const Date& date) const {
        return Datetime(date, time(), zone());
    }

    Datetime with_year(int year) const {
        return with_date(
            date().with_year(year)
        );
    }

    Datetime with_month(Month month) const {
        return with_date(
            date().with_month(month)
        );
    }

    Datetime with_day(int day) const {
        return with_date(
            date().with_day(day)
        );
    }

    Datetime with_time(const Time& time) const {
        return Datetime(date(), time, zone());
    }

    Datetime with_hour(int hour) const {
        return with_time(
            time().with_hour(hour)
        );
    }

    Datetime with_minute(int minute) const {
        return with_time(
            time().with_minute(minute)
        );
    }

    Datetime with_zone(const Zone& zone) const {
        return Datetime(date(), time(), zone);
    }

    Datetime at_zone(const Zone& zone) const {
        struct tm zoned_time = zone.mk_struct_tm(to_timestamp());
        return Datetime(zoned_time, zone);
    }

    bool is_dst() const {
        struct tm zoned_time = zone().mk_struct_tm(to_timestamp());
        return zoned_time.tm_isdst > 0;
    }

    bool operator<(const Datetime& rhs) const {
        return to_timestamp() < rhs.to_timestamp();
    }

    bool operator>(const Datetime& rhs) const {
        return to_timestamp() > rhs.to_timestamp();
    }

    bool operator==(const Datetime& rhs) const {
        return to_timestamp() == rhs.to_timestamp();
    }

    bool operator!=(const Datetime& rhs) const {
        return !(*this == rhs);
    }

    bool operator>=(const Datetime& rhs) const {
        return *this > rhs || *this == rhs;
    }

    bool operator<=(const Datetime& rhs) const {
        return *this < rhs || *this == rhs;
    }

    Datetime operator+(const Duration& rhs) const {
        return Datetime(to_timestamp() + rhs.total_minutes() * 60).at_zone(zone());
    }

    Datetime operator-(const Duration& rhs) const {
        return Datetime(to_timestamp() - rhs.total_minutes() * 60).at_zone(zone());
    }

    Datetime& operator+=(const Duration& rhs) {
        Datetime lhs = *this + rhs;
        *this = lhs;
        return *this;
    }

    Datetime& operator-=(const Duration& rhs) {
        Datetime lhs = *this + rhs;
        *this = lhs;
        return *this;
    }

    Duration operator-(const Datetime& rhs) const {
        return Duration(labs(to_timestamp() / 60 - rhs.to_timestamp() / 60));
    }

    std::string format(const std::string& format = DEFAULT_FORMAT) const {
        return zone().strftime(format, to_struct_tm());
    }

    friend std::ostream& operator<<(std::ostream& out, const Datetime& dt) {
        tfm::format(out, "Datetime<%04d-%02d-%02d %02d:%02d %s>",
                    dt.date().year(),
                    dt.date().nmonth(),
                    dt.date().day(),
                    dt.time().hour(),
                    dt.time().minute(),
                    dt.zone().name(dt.is_dst()));
        return out;
    }

private:
    void load_struct_tm(struct tm local_time) {
        _date = Date().with_year(local_time.tm_year + 1900)
            .with_nmonth(local_time.tm_mon + 1)
            .with_day(local_time.tm_mday);
        _time = Time(local_time.tm_hour, local_time.tm_min);
    }

    Date _date;
    Time _time;
    Zone _zone;
};

// --------------------------------------------------------
class Range {
public:
    Range(const Datetime& start, const Datetime& end) : _start(start), _end(end) {
        validate();
    }

    Range(const Datetime& start, const Duration& duration) : Range(start, start + duration) { }

    static Range for_days(const Date& date, int days) {
        return Range(Datetime(date), Datetime(date.advance_days(days)));
    }

    const Datetime& start() const {
        return _start;
    }

    const Datetime& end() const {
        return _end;
    }

    Duration duration() const {
        return end() - start();
    }

    bool contains(const Datetime& dt) const {
        return dt >= start() && dt < end();
    }

    bool contains(const Range& other) const {
        return other.start() >= start() && other.end() <= end();
    }

    bool intersects(const Range& other) const {
        return contains(other)
        || (other.start() >= start() && other.start() < end())
        || (other.end() > start() && other.end() < end());
    }

    std::optional<Range> clip_to(const Range& clipping_range) {
        if (! intersects(clipping_range)) {
            return {};
        }

        return Range(std::max(start(), clipping_range.start()),
                     std::min(end(), clipping_range.end()));
    }

    Range with_zone(const Zone& zone) const {
        return Range(
            start().with_zone(zone),
            end().with_zone(zone));
    }

    bool operator==(const Range& rhs) const {
        return start() == rhs.start() && end() == rhs.end();
    }

    bool operator!=(const Range& rhs) const {
        return ! (*this == rhs);
    }

    friend std::ostream& operator<<(std::ostream& out, const Range& range) {
        tfm::format(out, "Range<%s - %s>", range.start().format(), range.end().format());
        return out;
    }

private:
    void validate() {
        if (start() >= end()) {
            throw ValueError("End must be after start for Range.");
        }
    }

    Datetime _start;
    Datetime _end;
};

// --------------------------------------------------------
enum class FilterType {
    Stack,
    Year,
    Month,
    WeekdayOfMonth,
    Weekday,
    Monthday,
    Time,
    Range
};

// --------------------------------------------------------
class Filter {
public:
    Filter(FilterType type) : _type(type) { }
    virtual ~Filter() { }

    const std::string& type_name() const {
        static std::map<FilterType, std::string> NAME_TABLE = {
            {FilterType::Stack, "Stack"},
            {FilterType::Year, "Year"},
            {FilterType::Month, "Month"},
            {FilterType::WeekdayOfMonth, "WeekdayOfMonth"},
            {FilterType::Weekday, "Weekday"},
            {FilterType::Monthday, "Month"},
            {FilterType::Time, "Time"},
            {FilterType::Range, "Range"},
        };

        return moonlight::maps::get(NAME_TABLE, type(), "???");
    }

    FilterType type() const {
        return _type;
    }

    virtual std::optional<Range> next_range(const Datetime& pivot) const = 0;
    virtual std::optional<Range> prev_range(const Datetime& pivot) const = 0;

    virtual bool should_clip() const {
        return true;
    }

private:
    FilterType _type;
};

}

#endif /* !__TIMEFILTER_CORE_H */
