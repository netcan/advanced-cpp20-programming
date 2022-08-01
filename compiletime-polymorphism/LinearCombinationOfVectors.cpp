/*************************************************************************
    > File Name: LinearCombinationOfVectors.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-05-30 15:25
************************************************************************/
#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

using Vec = vector<int>;
using Scalar = int;

// ax + by
Vec LinearCombinationOfVectors(Scalar alpha, const Vec& x, Scalar beta, const Vec& y) {
    assert(x.size() == y.size());
    Vec res(x.size());
    for (size_t i = 0; i < x.size(); ++i)
        res[i] = alpha * x[i] + beta * y[i];
    return res;
}

int main() {
    vector<int> x{1, 2, 3}, y{1, 1, 1};
    Vec res = LinearCombinationOfVectors(1, x, 4, y);
    for (size_t i = 0; i < res.size(); ++i)
        cout << res[i] << " ";
    cout << endl;
}

