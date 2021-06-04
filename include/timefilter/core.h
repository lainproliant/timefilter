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
#include "moonlight/date.h"
#include <map>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <chrono>

#ifndef TIMEFILTER_TZ_UNSAFE
#include <thread>
#endif

namespace timefilter {

using namespace moonlight::date;

// --------------------------------------------------------
class Error : public moonlight::core::Exception {
    using Exception::Exception;
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
            {FilterType::Monthday, "Monthday"},
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

    virtual int order() const {
        return 0;
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

    bool is_absolute() const {
        return absolute_range().has_value();
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
