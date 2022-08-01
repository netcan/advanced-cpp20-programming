/*************************************************************************
    > File Name: TypeCalculation.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-11-11 23:01
************************************************************************/
#include "TypeList.hpp"

using In = TypeList<int, char, long, char, short, float, double>;

template<typename E>
using TypeSizeLess4 = bool_constant<sizeof(E) < 4>;
using Res = Unique_t<Map_t<Filter_t<In, TypeSizeLess4>, add_pointer>>::to<variant>;
static_assert(is_same_v<Res, variant<char*, short*>>);

int main(int argc, char** argv) {

    return 0;
}

