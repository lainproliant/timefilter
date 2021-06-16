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
namespace gen = moonlight::gen;
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
    typedef std::shared_ptr<const Filter> Pointer;

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

// --------------------------------------------------------
typedef Filter::Pointer filter_t;

}

#endif /* !__TIMEFILTER_CORE_H */
