/*************************************************************************
    > File Name: GenPrimeTable.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-11-11 21:10
************************************************************************/
#include <vector>
#include <array>
using namespace std;

consteval vector<int> sievePrime(int n) {
    vector<bool> marked(n + 1, true);
    for (int p = 2; p * p <= n; p++) {
        if (marked[p]) {
            for (int i = p * p; i <= n; i += p)
                marked[i] = false;
        }
    }

    vector<int> result;
    for (int p = 2; p <= n; p++)
        if (marked[p]) result.push_back(p);
    return result;
}

consteval size_t primeCount(int n) {
    return sievePrime(n).size();
}
static_assert(primeCount(100) == 25);

template<int n>
consteval auto savePrimeToArray() {
    array<int, primeCount(n)> result;
    auto primes = sievePrime(n);
    copy(primes.begin(), primes.end(), result.data());
    return result;
}

constexpr auto primes100 = savePrimeToArray<10000>();

int main() {
    for (auto p: primes100) {
        printf("%d\n", p);
    }
    return 0;
}
