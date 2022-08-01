/*************************************************************************
    > File Name: span.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-04-20 22:36
************************************************************************/

#include <iostream>
#include <array>
#include <vector>
#include <span>
using namespace std;

void passArrayLike(span<int> container) {
    cout << "container.size(): " << container.size() << '\n';
    for(auto elem: container) cout << elem << ' ';
    cout << "\n";
}

int main() {
    int arr[]{1, 2, 3, 4};
    passArrayLike(arr);

    vector vec{1, 2, 3, 4, 5}; // C++17 CTAD
    passArrayLike(vec);

    array arr2{1, 2, 3, 4, 5, 6}; // C++17 CTAD
    passArrayLike(arr2);
}

