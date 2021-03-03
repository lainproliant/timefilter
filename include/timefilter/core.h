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
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <memory>

namespace timefilter {

namespace gen = moonlight::gen;

// --------------------------------------------------------
const std::string DEFAULT_FORMAT = "%Y-%m-%d %H:%M:%S";
typedef std::chrono::seconds Duration;
typedef date::days Days;
typedef date::month Month;
typedef date::zoned_time<Duration, const date::time_zone*> ZonedDuration;
typedef date::hh_mm_ss<Duration> Time;
typedef date::year_month_day Date;

// --------------------------------------------------------
class ValueError : public moonlight::core::Exception {
    using Exception::Exception;
};

// --------------------------------------------------------
inline const date::time_zone* zone_or_utc(const date::time_zone* z) {
    static auto tz_utc = date::locate_zone("UTC");
    return z == nullptr ? tz_utc : z;
}

// --------------------------------------------------------
inline ZonedDuration now(const date::time_zone* tz_in = nullptr) {
    auto tz = zone_or_utc(tz_in);
    return date::make_zoned(tz, date::floor<Duration>(std::chrono::system_clock::now()));
}

// --------------------------------------------------------
inline unsigned int timestamp(const ZonedDuration& d) {
    return d.get_local_time().time_since_epoch().count();
}

// --------------------------------------------------------
inline unsigned int timestamp() {
    return timestamp(now());
}

// --------------------------------------------------------
class Datetime {
public:
    Datetime(const date::time_zone* zone = nullptr) :
    _dt(now(zone)) { }

    Datetime(const ZonedDuration& dt) : _dt(dt) { }

    Datetime(const Datetime& datetime) : _dt(datetime._dt) { }

    Date date() const {
        return Date{date::floor<Days>(_dt.get_local_time())};
    }

    Time time() const {
        auto local_time = _dt.get_local_time();
        auto d = date::floor<Days>(local_time);
        auto t = date::floor<Duration>(local_time) - d;
        return Time{t};
    }

    int year() const {
        return static_cast<int>(date().year());
    }

    date::month month() const {
        return date().month();
    }

    unsigned int nmonth() const {
        return static_cast<unsigned>(date().month());
    }

    unsigned int day() const {
        return static_cast<unsigned>(date().day());
    }

    int hour() const {
        return time().hours().count();
    }

    int minute() const {
        return time().minutes().count();
    }

    const date::time_zone* zone() const {
        return _dt.get_time_zone();
    }

    Datetime with_date(const Date& date) const {
        auto tp = date::floor<Duration>(
            date::local_days{date} + time().to_duration());
        auto local_date = date::make_zoned(_dt.get_time_zone(), tp);
        return Datetime(local_date);
    }

    Datetime with_year(int year) const {
        auto cd = date();
        auto d = date::year_month_day(date::year(year), cd.month(), cd.day());
        if (! d.ok()) {
            throw ValueError(tfm::format("%s is an invalid date.", d));
        }
        return with_date(d);
    }

    Datetime with_month(Month month) const {
        auto cd = date();
        auto d = date::year_month_day(cd.year(), month, cd.day());
        if (! d.ok()) {
            throw ValueError(tfm::format("%s is an invalid date.", d));
        }
        return with_date(d);
    }

    Datetime with_day(unsigned int day) const {
        auto cd = date();
        auto d = date::year_month_day(cd.year(), cd.month(), date::day{day});
        if (! d.ok()) {
            throw ValueError(tfm::format("%s is an invalid date.", d));
        }
        return with_date(d);
    }

    Datetime with_time(const Time& time) const {
    }

    Datetime with_hour(int hour) const {
    }

    Datetime with_minute(int minute) const {
    }

    Datetime with_zone(const Zone& zone) const {
    }

    Datetime at_zone(const Zone& zone) const {
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
    const ZonedDuration _dt;
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
