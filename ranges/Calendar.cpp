/*************************************************************************
    > File Name: Calendar.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-07-25 11:47
************************************************************************/
// g++-11 -std=c++2a Calendar.cpp
#include <chrono>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <string>
#include <fmt/core.h> // <format>
#include "Views.h"

namespace chrono = std::chrono;
///////////////////////////////////////////////////////////////////////////////
// Date
struct Date {
    using difference_type = std::ptrdiff_t;
    Date() = default;
    Date(uint16_t year, uint16_t month, uint16_t day):
        days_{ chrono::year(year) / chrono::month(month) / chrono::day(day) } { }

    bool operator==(const Date&) const = default;
    Date& operator++()            { ++days_; return *this;                                               }
    Date operator++(int)          { Date tmp(*this); ++*this; return tmp;                                }

    uint16_t day() const          { return static_cast<unsigned>(chrono::year_month_day(days_).day());   }
    uint16_t month() const        { return static_cast<unsigned>(chrono::year_month_day(days_).month()); }
    uint16_t year() const         { return static_cast<int>(chrono::year_month_day(days_).year());       }
    uint16_t dayOfWeek() const { // 得到当前日期星期数，值域[0,7)，0表示星期天
        return chrono::weekday(days_).c_encoding();
    }
    bool weekdaylessThan(const Date& rhs) {
        return dayOfWeek() < rhs.dayOfWeek();
    }
    const char* monthName() const { return MONTH_NAME[month()]; }

    friend std::ostream& operator<<(std::ostream& out, const Date& d) {
        out << d.year() << "-" << d.month() << "-" << d.day();
        return out;
    }

private:
    static constexpr const char* MONTH_NAME[] = {
        "", "January", "February", "March",
        "April", "May", "June", "July",
        "August", "September", "October",
        "November", "December",
    };
    chrono::sys_days days_;
};
static_assert(std::weakly_incrementable<Date>);

auto dates_between(uint16_t start, uint16_t stop) {
    return views::iota(Date{start, 1, 1}, Date{stop, 1, 1});
}

auto by_month() {
    return group_by([](Date a, Date b) { return a.month() == b.month(); });
}

auto by_week() {
    return group_by([](Date a, Date b) { return a.weekdaylessThan(b); });
}

// TODO: c++20 std::format
std::string month_title(const Date& d) {
    return fmt::format("{:^22}", d.monthName());
}

// TODO: c++20 std::format
std::string format_day(const Date& d) {
    return fmt::format("{:>3}", d.day());
}

// in: range<range<Date>>
// out: range<std::string>
auto format_weeks() {
    return views::transform([](/*range<Date>*/ auto&& week) {
        auto ws = week | views::transform(format_day) | views::join | views::common;
        std::string weeks(ws.begin(), ws.end());
        auto align_size = (*ranges::begin(week)).dayOfWeek() * 3;
        return fmt::format("{}{:<{}}",
                std::string(align_size, ' '),
                weeks, 22 - align_size);
    });
}

// in: range<range<Date>>
// out: range<range<std::string>>
auto layout_months() {
    return views::transform([](/*range<Date>*/ auto&& month) {
        auto week_count = ranges::distance(month | by_week());
        return concat(
            views::single(month_title(*ranges::begin(month))),
            views::single(std::string(" Su Mo Tu We Th Fr Sa ")),
            month | by_week() | format_weeks(),
            repeat_n(std::string(22, ' '), 6 - week_count)
        );
    });
}

// In:  range<range<range<string>>>
// Out: range<range<range<string>>>, transposing months.
auto transpose_months() {
    return views::transform([](/*range<range<string>>*/ auto&& rng) {
        return rng | transpose;
    });
}

// In:  range<range<string>>
// Out: range<join_view<char>>, joining the strings of the inner ranges
auto join_months() {
    return views::transform([](/*range<string>*/ auto&& rng) {
        return views::join(rng);
    });
}

int main(int argc, char** argv) {
    auto formattedCalendar
        = dates_between(2022, 2023) // range<Date>: 356
        | by_month()               // range<range<Date>>: 12 x month
        | layout_months()          // range<range<std::string>>: 12 x 8 x 22
        | chunk(4)                 // range<range<range<std::string>>>: 3 x 4 x 8 x 22
        | transpose_months()       // range<range<range<std::string>>>: 3 x 8 x 4 x 22
        | views::join              // range<range<std::string>>: 24 x 4 x 22
        | join_months()            // range<join_view<char>>: 24 x 88
        ;

    // print_range(formattedCalendar);

    for (auto&& line: formattedCalendar) {
        ranges::copy(line, std::ostream_iterator<char>(std::cout));
        std::cout << std::endl;
    }

    return 0;
}
