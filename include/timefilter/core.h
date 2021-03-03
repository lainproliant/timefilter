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
#include "moonlight/generator.h"
#include "moonlight/exceptions.h"
#include "moonlight/maps.h"
#include "date/date.h"
#include "date/tz.h"
#include <map>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <memory>

namespace timefilter {

namespace gen = moonlight::gen;

const std::string DEFAULT_FORMAT = "%Y-%m-%d %H:%M:%S";

// --------------------------------------------------------
typedef const date::time_zone* Zone;
typedef std::chrono::seconds Duration;
typedef std::chrono::time_point<std::chrono::system_clock, Duration> TimePoint;
typedef date::zoned_time<Duration, Zone> ZonedDuration;
typedef date::month Month;
typedef date::weekday Weekday;

// --------------------------------------------------------
class ValueError : public moonlight::core::Exception {
    using Exception::Exception;
};

// --------------------------------------------------------
inline unsigned int last_day_of_month(int year, Month month) {
    auto last_day = date::year_month_day_last(
        date::year{year}, date::month_day_last(month)
    );
    return static_cast<unsigned>(last_day.day());
}

// --------------------------------------------------------
inline date::sys_time<Duration> as_sys(const Duration& d) {
    return date::sys_time<Duration>(d);
}

// --------------------------------------------------------
inline date::local_time<Duration> as_local(const Duration& d) {
    return date::local_time<Duration>(d);
}

// --------------------------------------------------------
inline Duration sys_now() {
    return std::chrono::floor<Duration>(std::chrono::system_clock::now().time_since_epoch());
}

// --------------------------------------------------------
class Date {
public:
    Date()
    : Date(0, date::January, 1) { }

    Date(const date::year_month_day& ymd)
    : _ymd(ymd) { }

    Date(int year, Month month, unsigned int day)
    : _ymd(date::year_month_day(date::year{year}, month, date::day{day})) { }

    Date(int year, unsigned int month, unsigned int day)
    : _ymd(date::year_month_day(date::year{year}, date::month{month}, date::day{day})) { }

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

    Weekday weekday() const {
        auto ymw = date::year_month_weekday(date::sys_days{_ymd});
        return ymw.weekday();
    }

    unsigned int nweekday() const {
        return weekday().c_encoding();
    }

    int year() const {
        return static_cast<int>(_ymd.year());
    }

    Month month() const {
        return _ymd.month();
    }

    int nmonth() const {
        return static_cast<unsigned>(month()) + 1;
    }

    unsigned int day() const {
        return static_cast<unsigned>(_ymd.day());
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

        if (month() == date::December) {
            date = date.with_year(date.year() + 1).with_month(date::January);
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

        if (month() == date::January) {
            date = date.with_year(date.year() - 1).with_month(date::December);
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

    Duration to_duration() const {
        return date::sys_days{_ymd}.time_since_epoch();
    }

    bool operator<(const Date& rhs) const {
        return _ymd < rhs._ymd;
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
        return _ymd == rhs._ymd;
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
        if (! _ymd.ok()) {
            throw new ValueError("Date is invalid.");
        }
    }

    date::year_month_day _ymd;
};

// --------------------------------------------------------
class Time {
public:
    Time(int hour, int minute)
    : Time(std::chrono::hours{hour} + std::chrono::minutes{minute}) { }

    Time()
    : Time(0, 0) { }

    Time(const Duration duration)
    : Time(date::hh_mm_ss(duration)) { }

    Time(const date::hh_mm_ss<Duration>& hms)
    : _hms(hms) {
        validate();
    }

    static const Time& start_of_day() {
        static Time value = Time();
        return value;
    }

    static const Time& end_of_day() {
        static Time value = Time(23, 59);
        return value;
    }

    Time with_hour(int hour) const {
        return Time(hour, minute());
    }

    Time with_minute(int minute) const {
        return Time(hour(), minute);
    }

    int hour() const {
        return _hms.hours().count();
    }

    int minute() const {
        return _hms.minutes().count();
    }

    int seconds() const {
        return _hms.seconds().count();
    }

    Duration to_duration() const {
        return _hms.to_duration();
    }

    bool operator<(const Time& rhs) const {
        return _hms.to_duration() < rhs._hms.to_duration();
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
        return _hms.to_duration() == rhs._hms.to_duration();
    }

    bool operator!=(const Time& rhs) const {
        return ! (*this == rhs);
    }

    friend std::ostream& operator<<(std::ostream& out, const Time& time) {
        tfm::format(out, "Time<%02d:%02d>", time.hour(), time.minute());
        return out;
    }

private:
    void validate() const {
        if (! _hms.in_conventional_range()) {
            throw ValueError("Time is not in conventional range.");
        }
    }

    date::hh_mm_ss<Duration> _hms;
};

// --------------------------------------------------------
class Datetime {
public:
    Datetime(const Duration& sys_duration)
    : Datetime(default_zone(), sys_duration) { }

    Datetime(const ZonedDuration& local_duration)
    : _zd(local_duration) { }

    Datetime(const Zone& tz, const Duration& local_duration)
    : _zd(date::make_zoned(tz, as_local(local_duration))) { }


    Datetime(const Date& date, const Time& time)
    : Datetime(default_zone(), date, time) { }

    Datetime(const Date& date)
    : Datetime(date, Time::start_of_day()) { }

    Datetime(int year, Month month, unsigned int day)
    : Datetime(Date(year, month, day)) { }

    Datetime(int year, unsigned int month, unsigned int day)
    : Datetime(Date(year, month, day)) { }

    Datetime(const Zone& tz, const Date& date, const Time& time) {
        auto zoned_day = date::make_zoned(tz, as_local(date.to_duration()));
        _zd = zoned_day.get_local_time() + time.to_duration();
    }

    static Datetime now() {
        return Datetime(sys_now());
    }

    struct tm to_struct_tm() const {
        return (struct tm) {
            .tm_sec = 0,
            .tm_min = time().minute(),
            .tm_hour = time().hour(),
            .tm_mday = (int)date().day(),
            .tm_mon = date().nmonth() - 1,
            .tm_year = date().year() - 1900,
            .tm_wday = (int)date().nweekday(),
            .tm_isdst = -1,
        };

    }

    time_t to_timestamp() const {
        return _zd.get_sys_time().time_since_epoch().count();
    }

    Duration to_duration() const {
        return _zd.get_sys_time().time_since_epoch();
    }

    Date date() const {
        auto ymd = date::year_month_day(date::floor<date::days>(_zd.get_local_time()));
        return Date(ymd);
    }

    Time time() const {
        auto days = date::floor<date::days>(_zd.get_local_time());
        return Time(_zd.get_local_time() - days);
    }

    Datetime with_date(const Date& date) const {
        auto zoned_day = date::make_zoned(_zd.get_time_zone(), as_local(date.to_duration()));
        auto sum = zoned_day.get_sys_time() + time().to_duration();
        return Datetime(_zd.get_time_zone(), sum.time_since_epoch());
    }

    Datetime with_time(const Time& time) const {
        auto zoned_day = date::make_zoned(_zd.get_time_zone(), as_local(date().to_duration()));
        auto sum = zoned_day.get_sys_time() + time.to_duration();
        return Datetime(_zd.get_time_zone(), sum.time_since_epoch());
    }

    Datetime with_zone(const Zone& tz) const {
        return Datetime(tz, _zd.get_local_time().time_since_epoch());
    }

    Datetime as_zone(const Zone& tz) const {
        return Datetime(tz, _zd.get_sys_time().time_since_epoch());
    }

    bool is_dst() const {
        return _zd.get_info().save != std::chrono::minutes::zero();
    }

    Zone zone() const {
        return _zd.get_time_zone();
    }

    bool operator<(const Datetime& rhs) const {
        return to_duration() < rhs.to_duration();
    }

    bool operator>(const Datetime& rhs) const {
        return to_duration() > rhs.to_duration();
    }

    bool operator==(const Datetime& rhs) const {
        return to_duration() == rhs.to_duration();
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
        return Datetime(zone(), to_duration() + rhs);
    }

    Datetime operator-(const Duration& rhs) const {
        return Datetime(zone(), to_duration() - rhs);
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
        return to_duration() - rhs.to_duration();
    }

    std::string format(const std::string& format = DEFAULT_FORMAT) const {
        return date::format(format, make_zoned(zone(), as_sys(to_duration())));
    }

    friend std::ostream& operator<<(std::ostream& out, const Datetime& dt) {
        tfm::format(out, "Datetime<%s>", dt.format(DEFAULT_FORMAT));
        return out;
    }

private:
    static Zone default_zone() {
        static Zone tz = date::locate_zone("UTC");
        return tz;
    }

    ZonedDuration _zd;
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

    Range with_zone(Zone zone) const {
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
    Month,
    Monthday,
    Range,
    Select,
    List,
    Time,
    Weekday,
    WeekdayOfMonth,
    Year,
};

// --------------------------------------------------------
class Filter : public std::enable_shared_from_this<Filter> {
public:
    typedef std::shared_ptr<Filter> Pointer;

    Filter(FilterType type) : _type(type) { }
    virtual ~Filter() {  }

    const std::string& type_name() const {
        static std::map<FilterType, std::string> NAME_TABLE = {
            {FilterType::Month, "Month"},
            {FilterType::Monthday, "Month"},
            {FilterType::Range, "Range"},
            {FilterType::Select, "Select"},
            {FilterType::List, "List"},
            {FilterType::Time, "Time"},
            {FilterType::Weekday, "Weekday"},
            {FilterType::WeekdayOfMonth, "WeekdayOfMonth"},
            {FilterType::Year, "Year"},
        };

        return moonlight::maps::get(NAME_TABLE, type(), "???");
    }

    FilterType type() const {
        return _type;
    }

    virtual std::optional<Range> next_range(const Datetime& pivot) const = 0;
    virtual std::optional<Range> prev_range(const Datetime& pivot) const = 0;

    virtual int order() const {
        return 0;
    }

    virtual void validate_stack(const gen::Iterator<Pointer>& iter) const {
        for (auto it = iter; it != gen::end<Pointer>(); it++) {
            if ((*it)->order() >= order()) {
                throw ValueError(tfm::format("%s is of equal or higher order than %s and can't be included in the same filter stack.", (*it)->type_name(), type_name()));
            }
        }
    }

    virtual bool should_clip() const {
        return true;
    }

    std::string repr() const {
        return tfm::format("%s<%s>", type_name(), _repr());
    }

    friend std::ostream& operator<<(std::ostream& out, const Filter& filter) {
        out << filter.repr();
        return out;
    }

protected:
    virtual std::string _repr() const {
        return "";
    }

private:
    FilterType _type;
};

}

#endif /* !__TIMEFILTER_CORE_H */
