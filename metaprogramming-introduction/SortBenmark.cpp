/*************************************************************************
    > File Name: SortBenmark.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-01-12 21:35
************************************************************************/
#include <benchmark/benchmark.h>
#include <vector>
#include <cstdlib>

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
static void qsortPerf(benchmark::State& state) {
    size_t len = state.range(0);
    for (auto _: state) {
        auto seq = generateSequence(len);
        benchmark::DoNotOptimize(seq);
        qsort(seq.data(), len, sizeof(int), cmp);
    }
}
BENCHMARK(qsortPerf)->RangeMultiplier(2)->Range(8, kMaxLen)
    ->Unit(benchmark::kMillisecond);

static void stdSortPerf(benchmark::State& state) {
    size_t len = state.range(0);
    for (auto _: state) {
        auto seq = generateSequence(len);
        benchmark::DoNotOptimize(seq);
        std::sort(seq.begin(), seq.end(), [](int lhs, int rhs) { return rhs > lhs; });
    }
}
BENCHMARK(stdSortPerf)->RangeMultiplier(2)->Range(8, kMaxLen)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
