/*
 * tokens.h
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday April 29, 2024
 */

#ifndef __TIMEFILTER_TOKENS_H
#define __TIMEFILTER_TOKENS_H

#include <string>
#include <map>
#include "moonlight/lex.h"

namespace timefilter {

namespace lex = moonlight::lex;

// ------------------------------------------------------------------
enum class TokenType {
    DAY_MONTH,
    DAY_MONTH_YEAR,
    DAY_OF_MONTH,
    DURATION,
    H12_TIME,
    H24_TIME,
    ISO_DATE,
    MIL_TIME,
    MONTH,
    MONTH_DAY,
    MONTH_DAY_YEAR,
    MONTH_YEAR,
    OP_RANGE,
    OP_DURATION,
    OP_JOIN,
    US_DATE,
    WEEKDAY,
    WEEKDAYS,
    WEEKDAY_MONTHDAY,
    YEAR,
    YEAR_MONTH,
    YEAR_MONTH_DAY,
};

inline const std::string& token_type_name(TokenType type) {
    static const std::string _unknown = "UNKNOWN";
    static const std::map<TokenType, std::string> _names = {
        {TokenType::DAY_MONTH, "DAY_MONTH"},
        {TokenType::DAY_OF_MONTH, "DAY_OF_MONTH"},
        {TokenType::DAY_MONTH_YEAR, "DAY_MONTH_YEAR"},
        {TokenType::DURATION, "DURATION"},
        {TokenType::H12_TIME, "H12_TIME"},
        {TokenType::H24_TIME, "H24_TIME"},
        {TokenType::ISO_DATE, "ISO_DATE"},
        {TokenType::MIL_TIME, "MIL_TIME"},
        {TokenType::MONTH, "MONTH"},
        {TokenType::MONTH_DAY, "MONTH_DAY"},
        {TokenType::MONTH_DAY_YEAR, "MONTH_DAY_YEAR"},
        {TokenType::MONTH_YEAR, "MONTH_YEAR"},
        {TokenType::OP_RANGE, "OP_RANGE"},
        {TokenType::OP_DURATION, "OP_DURATION"},
        {TokenType::OP_JOIN, "OP_JOIN"},
        {TokenType::US_DATE, "US_DATE"},
        {TokenType::WEEKDAY, "WEEKDAY"},
        {TokenType::WEEKDAYS, "WEEKDAYS"},
        {TokenType::WEEKDAY_MONTHDAY, "WEEKDAY_MONTHDAY"},
        {TokenType::YEAR, "YEAR"},
        {TokenType::YEAR_MONTH, "YEAR_MONTH"},
        {TokenType::YEAR_MONTH_DAY, "YEAR_MONTH_DAY"}
    };

    auto iter = _names.find(type);
    if (iter != _names.end()) {
        return iter->second;
    }

    return _unknown;
}

inline std::ostream& operator<<(std::ostream& out, TokenType type) {
    out << token_type_name(type);
    return out;
}

using Grammar = lex::Grammar<TokenType>;
using Token = Grammar::Token;

}

#endif /* !__TIMEFILTER_TOKENS_H */
