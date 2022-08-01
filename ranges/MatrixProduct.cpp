/*************************************************************************
    > File Name: MatrixProduct.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-08-16 20:24
************************************************************************/
#include <vector>
#include <numeric>
#include <iostream>
#include "Views.h"

using namespace std;

auto product(ranges::viewable_range auto&& lhs, ranges::viewable_range auto&& rhs) {
    return lhs | views::transform([=](auto&& xrow) mutable {
        return rhs | transpose | views::transform([=](auto&& wcol) {
            return std::inner_product(xrow.begin(), xrow.end(),
                                      wcol.begin(), 0);
        });
    });
};

int main(int argc, char** argv) {
    {
        auto X = views::iota(1, 2 * 3 + 1) | chunk(3);
        auto W = views::iota(1, 2 * 3 + 1) | chunk(2);
        // X * W
        print_range(product(X, W));
        cout << "\n";
    }

    {
        vector X { // 3 x 4
            vector{3, 1,  1, 4},
            vector{5, -3, 2, 1},
            vector{6, 2, -9, 5},
        };
        vector W { // 4 x 2
            vector{4,  9}, vector{6, -8},
            vector{9,  7}, vector{7,  6},
        };
        print_range(product(X, W), true);
        cout << "\n";
    }


    return 0;
}
