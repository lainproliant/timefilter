/*
 * date_filter.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Thursday April 11, 2024
 */


#include <csignal>
#include <iostream>
#include "moonlight/test.h"
#include "timefilter/filters.h"

using namespace timefilter;
using namespace moonlight;
using namespace moonlight::test;


int main() {
    Date dateA(1988, Month::February, 29);
    Date dateB(1988, Month::June, 8);
    Date dateC(1997, Month::February, 28);
    Date dateD(2015, Month::January, 1);

    Datetime dt1980 = Datetime(1980, Month::January, 1);
    Datetime dt1990 = Datetime(1990, Month::January, 1);
    Datetime dt2000 = Datetime(2000, Month::January, 1);
    Datetime dt2010 = Datetime(2010, Month::January, 1);

    auto filterA = DateFilter::create(dateA);
    auto filterB = DateFilter::create(dateB);
    auto filterC = DateFilter::create(dateC);
    auto filterD = DateFilter::create(dateD);

    std::cout << "dateA = " << dateA << std::endl;
    std::cout << "dateB = " << dateB << std::endl;
    std::cout << "dateC = " << dateC << std::endl;
    std::cout << "dateD = " << dateD << std::endl;

    std::cout << "dt1980 = " << dt1980 << std::endl;
    std::cout << "dt1990 = " << dt1990 << std::endl;
    std::cout << "dt2000 = " << dt2000 << std::endl;
    std::cout << "dt2010 = " << dt2010 << std::endl;

    return TestSuite("timefilter date_filter tests")
    .test("next_range()", [&]() {
        auto rangeA_1980 = filterA->next_range(dt1980);
        auto rangeA_1990 = filterA->next_range(dt1990);
        auto rangeA_2000 = filterA->next_range(dt2000);
        auto rangeA_2010 = filterA->next_range(dt2010);
        auto rangeB_1980 = filterB->next_range(dt1980);
        auto rangeB_1990 = filterB->next_range(dt1990);
        auto rangeB_2000 = filterB->next_range(dt2000);
        auto rangeB_2010 = filterB->next_range(dt2010);
        auto rangeC_1980 = filterC->next_range(dt1980);
        auto rangeC_1990 = filterC->next_range(dt1990);
        auto rangeC_2000 = filterC->next_range(dt2000);
        auto rangeC_2010 = filterC->next_range(dt2010);
        auto rangeD_1980 = filterD->next_range(dt1980);
        auto rangeD_1990 = filterD->next_range(dt1990);
        auto rangeD_2000 = filterD->next_range(dt2000);
        auto rangeD_2010 = filterD->next_range(dt2010);

        ASSERT_TRUE(rangeA_1980.has_value());
        ASSERT_EQUAL(*rangeA_1980, Range(Datetime(1988, Month::February, 29),
                                     Datetime(1988, Month::March, 1)));

        ASSERT_FALSE(rangeA_1990.has_value());
        ASSERT_FALSE(rangeA_2000.has_value());
        ASSERT_FALSE(rangeA_2010.has_value());

        ASSERT_TRUE(rangeB_1980.has_value());
        ASSERT_EQUAL(*rangeB_1980, Range(Datetime(1988, Month::June, 8),
                                     Datetime(1988, Month::June, 9)));

        ASSERT_FALSE(rangeB_1990.has_value());
        ASSERT_FALSE(rangeB_2000.has_value());
        ASSERT_FALSE(rangeB_2010.has_value());

        ASSERT_TRUE(rangeC_1980.has_value());
        ASSERT_EQUAL(*rangeC_1980, Range(Datetime(1997, Month::February, 28),
                                     Datetime(1997, Month::March, 1)));

        ASSERT_TRUE(rangeC_1990.has_value());
        ASSERT_EQUAL(*rangeC_1990, Range(Datetime(1997, Month::February, 28),
                                     Datetime(1997, Month::March, 1)));

        ASSERT_FALSE(rangeC_2000.has_value());
        ASSERT_FALSE(rangeC_2010.has_value());

        ASSERT_TRUE(rangeD_1980.has_value());
        ASSERT_EQUAL(*rangeD_1980, Range(Datetime(2015, Month::January, 1),
                                     Datetime(2015, Month::January, 2)));

        ASSERT_TRUE(rangeD_1990.has_value());
        ASSERT_EQUAL(*rangeD_1990, Range(Datetime(2015, Month::January, 1),
                                     Datetime(2015, Month::January, 2)));

        ASSERT_TRUE(rangeD_2000.has_value());
        ASSERT_EQUAL(*rangeD_2000, Range(Datetime(2015, Month::January, 1),
                                     Datetime(2015, Month::January, 2)));

        ASSERT_TRUE(rangeD_2010.has_value());
        ASSERT_EQUAL(*rangeD_2010, Range(Datetime(2015, Month::January, 1),
                                     Datetime(2015, Month::January, 2)));
    })
    .test("prev_range()", [&]() {
        auto rangeA_1980 = filterA->prev_range(dt1980);
        auto rangeA_1990 = filterA->prev_range(dt1990);
        auto rangeA_2000 = filterA->prev_range(dt2000);
        auto rangeA_2010 = filterA->prev_range(dt2010);
        auto rangeB_1980 = filterB->prev_range(dt1980);
        auto rangeB_1990 = filterB->prev_range(dt1990);
        auto rangeB_2000 = filterB->prev_range(dt2000);
        auto rangeB_2010 = filterB->prev_range(dt2010);
        auto rangeC_1980 = filterC->prev_range(dt1980);
        auto rangeC_1990 = filterC->prev_range(dt1990);
        auto rangeC_2000 = filterC->prev_range(dt2000);
        auto rangeC_2010 = filterC->prev_range(dt2010);
        auto rangeD_1980 = filterD->prev_range(dt1980);
        auto rangeD_1990 = filterD->prev_range(dt1990);
        auto rangeD_2000 = filterD->prev_range(dt2000);
        auto rangeD_2010 = filterD->prev_range(dt2010);

        ASSERT_FALSE(rangeA_1980.has_value());
        ASSERT_TRUE(rangeA_1990.has_value());
        ASSERT_EQUAL(*rangeA_1990, Range(Datetime(1988, Month::February, 29),
                                     Datetime(1988, Month::March, 1)));
        ASSERT_TRUE(rangeA_2000.has_value());
        ASSERT_EQUAL(*rangeA_2000, Range(Datetime(1988, Month::February, 29),
                                     Datetime(1988, Month::March, 1)));
        ASSERT_TRUE(rangeA_2010.has_value());
        ASSERT_EQUAL(*rangeA_2010, Range(Datetime(1988, Month::February, 29),
                                     Datetime(1988, Month::March, 1)));

        ASSERT_FALSE(rangeB_1980.has_value());
        ASSERT_TRUE(rangeB_1990.has_value());
        ASSERT_EQUAL(*rangeB_1990, Range(Datetime(1988, Month::June, 8),
                                     Datetime(1988, Month::June, 9)));
        ASSERT_TRUE(rangeB_2000.has_value());
        ASSERT_EQUAL(*rangeB_2000, Range(Datetime(1988, Month::June, 8),
                                     Datetime(1988, Month::June, 9)));
        ASSERT_TRUE(rangeB_2010.has_value());
        ASSERT_EQUAL(*rangeB_2010, Range(Datetime(1988, Month::June, 8),
                                     Datetime(1988, Month::June, 9)));

        ASSERT_FALSE(rangeC_1980.has_value());
        ASSERT_FALSE(rangeC_1990.has_value());
        ASSERT_TRUE(rangeC_2000.has_value());
        ASSERT_EQUAL(*rangeC_2000, Range(Datetime(1997, Month::February, 28),
                                     Datetime(1997, Month::March, 1)));
        ASSERT_TRUE(rangeC_2010.has_value());
        ASSERT_EQUAL(*rangeC_2010, Range(Datetime(1997, Month::February, 28),
                                     Datetime(1997, Month::March, 1)));

        ASSERT_FALSE(rangeD_1980.has_value());
        ASSERT_FALSE(rangeD_1990.has_value());
        ASSERT_FALSE(rangeD_2000.has_value());
        ASSERT_FALSE(rangeD_2010.has_value());
    })
    .die_on_signal(SIGSEGV)
    .run();
}
