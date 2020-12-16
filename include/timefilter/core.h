/*
 * core.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday December 10, 2020
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __CORE_H
#define __CORE_H

#include "tinyformat/tinyformat.h"
#include "moonlight/exceptions.h"
#include "moonlight/maps.h"
#include <map>
#include <ctime>

namespace timefilter {

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
class Duration {
public:
    Duration() : _minutes(0) { }
    Duration(int minutes) : _minutes(minutes) { }
    Duration(int hours, int minutes) : Duration(hours * 60 + minutes) { }
    Duration(int days, int hours, int minutes) : Duration(days * 24 + hours, minutes) { }

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
    Date advance_days(int days) {
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
    Date recede_days(int days) {
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
    Weekday weekday() {
        int adjustment, mm, yy;

        adjustment = (14 - nmonth()) / 12;
        mm = nmonth() + 12 * adjustment - 2;
        yy = year() - adjustment;
        return static_cast<Weekday>(
            ((day() + (13 * mm - 1) / 5 +
              yy + yy / 4 - yy / 100 + yy / 400) % 7));
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

    struct tm to_struct_tm() const {
        return (struct tm) {
            .tm_sec = 0,
            .tm_min = 0,
            .tm_hour = 0,
            .tm_mday = day(),
            .tm_mon = nmonth() - 1,
            .tm_year = year() - 1900
        };
    }

    time_t to_utc_date() const {
        struct tm utime = to_struct_tm();
        return timegm(&utime);
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

    Duration operator-(const Date& rhs) {
        double diff = difftime(to_utc_date(), rhs.to_utc_date());
        return Duration(diff / 60);
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
    Time(int hour, int minute) : _minutes(hour * 60 + minute) { }

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
    Datetime() : _date(), _time() { }
    Datetime(const Date& date) : _date(date), _time() { }
    Datetime(const Date& date, const Time& time) : _date(date), _time(time) { }
    Datetime(int year, Month month) : _date(year, month), _time() { }
    Datetime(int year, Month month, int day) : _date(year, month, day), _time() { }
    Datetime(int year, Month month, int day, int hour, int minute) : _date(year, month, day), _time(hour, minute) { }

    Datetime(const struct tm& utime) {
        load_struct_tm(utime);
    }

    Datetime(time_t unix_time, bool is_local_time = false) {
        struct tm utime;

        if (is_local_time) {
            utime = *localtime(&unix_time);
        } else {
            utime = *gmtime(&unix_time);
        }

        load_struct_tm(utime);
    }

    struct tm to_struct_tm() const {
        return (struct tm) {
            .tm_sec = 0,
            .tm_min = time().minute(),
            .tm_hour = time().hour(),
            .tm_mday = date().day(),
            .tm_mon = date().nmonth(),
            .tm_year = date().year()
        };
    }

    time_t to_utc_time() const {
        struct tm utime = to_struct_tm();
        return timegm(&utime);
    }

    time_t to_local_time() const {
        struct tm utime = to_struct_tm();
        return mktime(&utime);
    }

    Datetime local_to_utc() const {
        time_t local_time = to_local_time();
        time_t utc_time = local_time + timezone - (daylight ? 3600 : 0);
        return Datetime(utc_time);
    }

    Datetime utc_to_local() const {
        time_t utc_time = to_utc_time();
        time_t local_time = utc_time - timezone - (daylight ? 3600 : 0);
        return Datetime(local_time);
    }

    const Date& date() const {
        return _date;
    }

    Date& date() {
        return _date;
    }

    const Time& time() const {
        return _time;
    }

    Time& time() {
        return _time;
    }

private:
    void load_struct_tm(const struct tm& utime) {
        _date = Date().with_year(utime.tm_year)
            .with_nmonth(utime.tm_mon + 1)
            .with_day(utime.tm_mday);
        _time = Time(utime.tm_hour, utime.tm_min);
    }

    Date _date;
    Time _time;
};

// --------------------------------------------------------
enum class FilterType {
    Year,
    Month,
    WeekdayOfMonth,
    Weekday,
    Monthday,
    Hour,
    Minute,
    Duration
};

// --------------------------------------------------------
class Filter {
public:
    Filter(FilterType type) : _type(type) { }

    const std::string& type_name() const {
        static std::map<FilterType, std::string> NAME_TABLE = {
            {FilterType::Year, "Year"},
            {FilterType::Month, "Month"},
            {FilterType::WeekdayOfMonth, "WeekdayOfMonth"},
            {FilterType::Weekday, "Weekday"},
            {FilterType::Monthday, "Month"},
            {FilterType::Hour, "Hour"},
            {FilterType::Minute, "Minute"},
            {FilterType::Duration, "Duration"}
        };

        return moonlight::maps::get(NAME_TABLE, type(), "???");
    }

    FilterType type() const {
        return _type;
    }

private:
    FilterType _type;
};

}

#endif /* !__CORE_H */
