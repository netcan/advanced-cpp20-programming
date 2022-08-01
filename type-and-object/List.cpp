/*************************************************************************
    > File Name: List.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-01-23 14:37
************************************************************************/
#include <iostream>
#include <utility>
using namespace std;

template<typename T, size_t N = 0>
struct List {
    T head;
    List<T, N-1> tail;
};

template<typename T>
struct List<T, 0> {};

template<typename T> List(T) -> List<T, 1>;
template<typename T, size_t N> List(T, List<T, N>) -> List<T, N+1>;

template<size_t N>
using IntList = List<int, N>;

int main(int argc, char** argv) {
    auto list1 = IntList<2>{0, IntList<1>{1, IntList<0>{}}};
    auto list2 = List{0, List{1, List{2}}};

    return 0;
}
