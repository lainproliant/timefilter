/*
 * unparser.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Wednesday April 16, 2025
 */

#ifndef __TIMEFILTER_UNPARSER_H
#define __TIMEFILTER_UNPARSER_H

#include "moonlight/classify.h"
#include "timefilter/date.h"
#include "timefilter/parser.h"

namespace timefilter {

// ------------------------------------------------------------------
class Unparser {
public:
    explicit Unparser(const I18nStrings& i18n = I18nStrings::defaults())
    : _i18n(i18n) { }

    std::string unparse(Filter::Pointer filter) const {
        moonlight::Classifier<Filter::Pointer, std::string> classify;
        std::ostringstream sb;

        const auto is_type = [](FilterType type) {
            return [=](Filter::Pointer p) {
                return p->type() == type;
            };
        };

        classify(is_type(FilterType::Date)) = [](Filter::Pointer p) {
            std::ostringstream sb;
            DateFilter::Pointer dp = std::static_pointer_cast<DateFilter>(p);

            return sb.str();
        };

        return classify.apply(filter).value();
    }

private:
    const I18nStrings _i18n;
};

}

#endif /* !__TIMEFILTER_UNPARSER_H */
