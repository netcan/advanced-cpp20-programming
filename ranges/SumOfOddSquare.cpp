/*************************************************************************
    > File Name: SumOfOddSquare.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-07-21 22:05
************************************************************************/
#include <iostream>
#include <ranges>

namespace ranges = std::ranges;
namespace views = std::views;

template<ranges::input_range R>
auto sum(R&& r) {
    ranges::range_value_t<R> value{};
    for (auto e: r) { value += e; }
    return value;
}

int main() {
    auto res = views::iota(1) // 生成[1,)的无穷数列
        | views::transform ([](auto n) { return n * n;      })
        | views::filter    ([](auto n) { return n % 2 == 1; })
        | views::take_while([](auto n) { return n < 10000;  });
    std::cout << sum(res) << std::endl; // 166650
}

