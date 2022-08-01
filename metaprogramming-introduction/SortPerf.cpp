/*************************************************************************
    > File Name: SortBenmark.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-01-12 21:35
************************************************************************/
#include <cstddef>
#include <benchmark/benchmark.h>
#include <vector>

constexpr size_t kMaxLen = 1<<24;
std::vector<int> generateSequence(size_t n) {
    std::vector<int> res(n);
    for (size_t i = 0; i < n; ++i)
        res[i] = rand();
    return res;
}

int cmp(const void* lhs, const void* rhs) {
    return *(int*)rhs - *(int*)lhs;
}
static void qsortPerf() {
    auto seq = generateSequence(kMaxLen);
    benchmark::DoNotOptimize(seq);
    qsort(seq.data(), kMaxLen, sizeof(int), cmp);
}

static void stdSortPerf() {
    auto seq = generateSequence(kMaxLen);
    benchmark::DoNotOptimize(seq);
    std::sort(seq.begin(), seq.end(), [](int lhs, int rhs) { return rhs > lhs; });
}

int main(int argc, char** argv) {
    qsortPerf();
    stdSortPerf();

    return 0;
}
