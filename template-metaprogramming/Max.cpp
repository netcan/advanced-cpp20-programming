/*************************************************************************
    > File Name: Max.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-01-12 21:33
************************************************************************/

#include <type_traits>
#include <concepts>

template<typename T>
concept Compareable = requires(T a, T b) {
    { a > b } -> std::same_as<bool>;
};

template<typename T>
requires Compareable<T>
T max(T a, T b) { return a > b ? a : b; }

struct Foo {};

int main(int argc, char** argv) {
    max(1, 2);
    // max(Foo{}, Foo{});

    return 0;
}
