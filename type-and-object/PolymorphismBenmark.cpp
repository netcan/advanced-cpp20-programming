/*************************************************************************
    > File Name: PolymorphismBenmark.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-02-02 21:44
************************************************************************/
#include <benchmark/benchmark.h>
#include <vector>
#include <memory>
#include <cstdlib>
#include "ShapeAdhoc.hpp"
#include "ShapeSubtype.hpp"
using namespace std;

constexpr size_t kMaxLen = 1<<24;
static void subtypePerf(benchmark::State& state) {
    using namespace Subtype;
    size_t len = state.range(0);
    for (auto _: state) {
        vector<unique_ptr<Shape>> shapes;
        shapes.reserve(len);
        for (size_t i = 0; i < len; ++i) {
            if (rand() % 100 > 50) {
                shapes.emplace_back(make_unique<Rectangle>(rand() % 10, rand() % 10));
            } else {
                shapes.emplace_back(make_unique<Circle>(rand() % 10));
            }
        }
        for (const auto& shape: shapes) {
            benchmark::DoNotOptimize(shape->getArea());
            benchmark::DoNotOptimize(shape->getPerimeter());
        }
    }
}
BENCHMARK(subtypePerf)->RangeMultiplier(2)->Range(8, kMaxLen)
    ->Unit(benchmark::kMillisecond);

static void adhocPerf(benchmark::State& state) {
    using namespace Adhoc;
    size_t len = state.range(0);
    for (auto _: state) {
        vector<Shape> shapes;
        shapes.reserve(len);
        for (size_t i = 0; i < len; ++i) {
            if (rand() % 100 > 50) {
                shapes.emplace_back(Rectangle{rand() % 10 * 1.0, rand() % 10 * 1.0});
            } else {
                shapes.emplace_back(Circle{rand() % 10 * 1.0});
            }
        }
        for (const auto& shape: shapes) {
            benchmark::DoNotOptimize(getArea(shape));
            benchmark::DoNotOptimize(getPerimeter(shape));
        }
    }
}
BENCHMARK(adhocPerf)->RangeMultiplier(2)->Range(8, kMaxLen)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
