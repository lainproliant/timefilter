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

#include <map>
#include <string>
#include <memory>

#include "tinyformat/tinyformat.h"
#include "moonlight/exceptions.h"
#include "moonlight/date.h"

#ifndef TIMEFILTER_TZ_UNSAFE
#include <thread>
#endif

namespace timefilter {

using namespace moonlight::date;

// --------------------------------------------------------
EXCEPTION_TYPE(Error);
EXCEPTION_SUBTYPE(Error, CannotMakeDiscreteError);

// --------------------------------------------------------
enum class FilterType {
    Custom,
    Date,
    Datetime,
    Duration,
    Join,
    List,
    Month,
    Monthday,
    Range,
    StaticRange,
    Time,
    Weekday,
    WeekdayOfMonth,
    Year,
};

// --------------------------------------------------------
enum class FilterOrder {
    Absolute,
    Year,
    Month,
    Monthday,
    Weekday,
    TimeOfDay,
    Last
};

// --------------------------------------------------------
inline static const std::string& filter_order_name(FilterOrder order) {
    static std::string UNKNOWN = "???";
    static std::map<FilterOrder, std::string> names = {
        {FilterOrder::Absolute, "Absolute"},
        {FilterOrder::Year, "Year"},
        {FilterOrder::Month, "Month"},
        {FilterOrder::Monthday, "Monthday"},
        {FilterOrder::Weekday, "Weekday"},
        {FilterOrder::TimeOfDay, "TimeOfDay"},
        {FilterOrder::Last, "Last"}
    };
    auto iter = names.find(order);
    if (iter == names.end()) {
        return UNKNOWN;
    }
    return iter->second;
}

// --------------------------------------------------------
class Filter : public std::enable_shared_from_this<Filter> {
 public:
    typedef std::shared_ptr<const Filter> Pointer;

    explicit Filter(FilterType type) : _type(type) { }
    virtual ~Filter() {  }

    const std::string& type_name() const {
        static std::string UNKNOWN = "???";
        static std::map<FilterType, std::string> NAME_TABLE = {
            {FilterType::Custom, "Custom"},
            {FilterType::Datetime, "Datetime"},
            {FilterType::Date, "Date"},
            {FilterType::Duration, "Duration"},
            {FilterType::Join, "Join"},
            {FilterType::List, "List"},
            {FilterType::Month, "Month"},
            {FilterType::Monthday, "Monthday"},
            {FilterType::Range, "Range"},
            {FilterType::StaticRange, "StaticRange"},
            {FilterType::Time, "Time"},
            {FilterType::Weekday, "Weekday"},
            {FilterType::WeekdayOfMonth, "WeekdayOfMonth"},
            {FilterType::Year, "Year"}
        };

        return NAME_TABLE.contains(type()) ? NAME_TABLE[type()] : UNKNOWN;
    }

    FilterType type() const {
        return _type;
    }

    virtual std::optional<Range> next_range(const Datetime& pivot) const = 0;
    virtual std::optional<Range> prev_range(const Datetime& pivot) const = 0;

    std::optional<Range> absolute_range() const {
        auto start = Datetime::min();
        auto range = next_range(start);
        if (! range.has_value()) {
            return {};
        }

        if (next_range(range->end()).has_value()) {
            return {};
        } else {
            return range;
        }
    }

    virtual FilterOrder order() const {
        static const std::map<FilterType, FilterOrder> orders = {
            {FilterType::Custom, FilterOrder::Absolute},
            {FilterType::Date, FilterOrder::Absolute},
            {FilterType::Datetime, FilterOrder::Absolute},
            {FilterType::Duration, FilterOrder::Last},
            {FilterType::List, FilterOrder::Absolute},
            {FilterType::Join, FilterOrder::Absolute},
            {FilterType::Month, FilterOrder::Month},
            {FilterType::Monthday, FilterOrder::Monthday},
            {FilterType::Range, FilterOrder::Absolute},
            {FilterType::StaticRange, FilterOrder::Absolute},
            {FilterType::Time, FilterOrder::TimeOfDay},
            {FilterType::Weekday, FilterOrder::Weekday},
            {FilterType::WeekdayOfMonth, FilterOrder::Weekday},
            {FilterType::Year, FilterOrder::Year},
        };
        auto iter = orders.find(type());
        assert(iter != orders.end());
        return iter->second;
    }

    int order_int() const {
        return static_cast<int>(order());
    }

    virtual bool should_clip() const {
        return true;
    }

    std::string repr() const {
        if (type() == FilterType::List) {
            return tfm::format("[%s]", _repr());

        } else {
            return tfm::format("%s<%s>", type_name(), _repr());
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const Filter& filter) {
        out << filter.repr();
        return out;
    }

    bool is_absolute() const {
        return absolute_range().has_value();
    }

    virtual bool is_discrete() const {
        return false;
    }

    virtual Pointer simplify() const {
        return shared_from_this();
    }

    bool is_never() const {
        Datetime dt;
        return (!prev_range(dt).has_value() && !next_range(dt).has_value());
    }

 protected:
    virtual std::string _repr() const {
        return "";
    }

 private:
    FilterType _type;
};

// --------------------------------------------------------
typedef Filter::Pointer filter_t;

}  // namespace timefilter

#endif /* !__TIMEFILTER_CORE_H */
