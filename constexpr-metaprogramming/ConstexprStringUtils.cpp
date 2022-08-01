/*************************************************************************
    > File Name: ConstexprStringUtils.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-10-27 20:03
************************************************************************/
#include <iostream>
#include <array>
#include <type_traits>
#include <algorithm>

template<typename T> constexpr auto strLength =
        strLength<std::remove_cvref_t<T>>;

template<size_t N>
constexpr size_t strLength<char[N]> = N - 1;

template<size_t N>
constexpr size_t strLength<std::array<char, N>> = N - 1;

namespace detail {
template<typename DelimType, size_t N>
struct JoinStringFold {
    consteval JoinStringFold(DelimType delimiter): delimiter(delimiter) {}
    template<typename STR>
    friend decltype(auto) consteval operator+(JoinStringFold&& self, STR&& str) {
        self.pstr = std::copy_n(std::begin(std::forward<STR>(str)), strLength<STR>, self.pstr);
        if (self.joinedStr.end() - self.pstr > strLength<DelimType>) {
            self.pstr = std::copy_n(self.delimiter, strLength<DelimType>, self.pstr);
        }
        return std::forward<JoinStringFold>(self);
    }
    std::array<char, N + 1> joinedStr{};
    DelimType delimiter;
    decltype(joinedStr.begin()) pstr = joinedStr.begin();
};
};

template<typename DelimType, typename... STRs>
consteval auto join(DelimType&& delimiter, STRs&&... strs) {
    constexpr size_t strNum = sizeof...(STRs);
    constexpr size_t len = (strLength<STRs> + ... + 0) +
        (strNum >= 1 ? strNum - 1 : 0) * strLength<DelimType>;

    return (detail::JoinStringFold<DelimType, len>{std::forward<DelimType>(delimiter)}
            + ... + std::forward<STRs>(strs)).joinedStr;
}

template<typename... STRs>
consteval auto concat(STRs&&... strs) {
    return join("", std::forward<STRs>(strs)...); // left fold
}

template<typename T>
struct dump;

int main(int argc, char** argv) {
    {
        constexpr auto helloworld = concat("hello", "world");
        printf("size=%zu %s\n", helloworld.size(), helloworld.data());
    }

    {
        constexpr auto one_two_three = join(", ", "one", "two", "three");
        printf("size=%zu %s\n", one_two_three.size(), one_two_three.data());
    }

    {
        constexpr auto one_two = concat("one", "two");
        constexpr auto one_two_three = concat(one_two, "three", concat(".", ".", "."));
        printf("size=%zu %s\n", one_two_three.size(), one_two_three.data());
    }

    {
        static_assert(strLength<decltype("hello")> == 5);
    }

    return 0;
}
