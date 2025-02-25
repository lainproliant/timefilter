/*
 * filter.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday May 16, 2024
 */

#ifndef __TIMEFILTER_FILTER_H
#define __TIMEFILTER_FILTER_H

#include "moonlight/exceptions.h"
#include "moonlight/date.h"

namespace timefilter {

// --------------------------------------------------------
using namespace moonlight::date;
EXCEPTION_TYPE(Error);

// --------------------------------------------------------
enum class FilterType {
    Date,
    Datetime,
    Duration,
    FilterList,
    FilterOffset,
    FilterSet,
    Month,
    Monthday,
    RelativeRange,
    StaticRange,
    Time,
    Weekday,
    WeekdayMonthday,
    WeekdayOfMonth,
    Year
};

inline bool operator<(FilterType lhs, FilterType rhs) {
    return static_cast<std::underlying_type_t<FilterType>>(lhs)
           < static_cast<std::underlying_type_t<FilterType>>(rhs);
}

inline std::set<FilterType>& absolute_filter_types() {
    static std::set<FilterType> types = {
        FilterType::Date,
        FilterType::Datetime,
        FilterType::StaticRange,
        FilterType::Year
    };
    return types;
}

inline std::set<FilterType>& relative_filter_types() {
    static std::set<FilterType> types = {
        FilterType::Duration,
        FilterType::FilterList,
        FilterType::FilterOffset,
        FilterType::FilterSet,
        FilterType::RelativeRange
    };
    return types;
}

inline const std::string& filter_type_name(FilterType type) {
    static std::string UNKNOWN = "???";
    static std::vector<std::string> names = {
        "Date",
        "Datetime",
        "Duration",
        "FilterList",
        "FilterOffset",
        "FilterSet",
        "Month",
        "Monthday",
        "RelativeRange",
        "StaticRange",
        "Time",
        "Weekday",
        "WeekdayMonthday",
        "WeekdayOfMonth",
        "Year"
    };

    auto offset = static_cast<int>(type);
    if (offset < 0 || offset >= names.size()) {
        return UNKNOWN;
    }

    return names.at(offset);
}

// --------------------------------------------------------
class Filter : public std::enable_shared_from_this<Filter> {
 public:
     typedef std::shared_ptr<const Filter> Pointer;

     explicit Filter(FilterType type) : _type(type) { }
     virtual ~Filter() { }

     FilterType type() const {
         return _type;
     }

     virtual std::optional<Range> next_range(const Datetime& dt) const = 0;
     virtual std::optional<Range> prev_range(const Datetime& dt) const = 0;

     std::optional<Range> current_range(const Datetime& dt) const {
         auto prev_rg = prev_range(dt);

         if (prev_rg.has_value() && prev_rg->contains(dt)) {
             return prev_rg;
         }

         return {};
     }

     std::string repr() const {
         std::ostringstream sb;
         sb << *this;
         return sb.str();
     }

     const std::string& type_name() const {
         return filter_type_name(type());
     }

     friend std::ostream& operator<<(std::ostream& out, const Filter& filter) {
         if (filter.type() == FilterType::FilterList) {
             out << "[" << filter._repr() <<  "]";
         } else if (filter.type() == FilterType::FilterSet) {
             out << "{" << filter._repr() << "}";
         } else {
             out << filter.type_name() << "<" << filter._repr() << ">";
         }
         return out;
     }

     virtual bool is_absolute() const {
         return absolute_filter_types().contains(type());
     }

     bool is_relative() const {
         return relative_filter_types().contains(type());
     }

     virtual Pointer simplify() const {
         return shared_from_this();
     }

 protected:
     virtual std::string _repr() const {
         return "";
     }

 private:
     const FilterType _type;
};

}


#endif /* !__TIMEFILTER_FILTER_H */
