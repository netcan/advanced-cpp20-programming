/*************************************************************************
    > File Name: PolymorphismBenmark.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-02-02 21:44
************************************************************************/
#include <benchmark/benchmark.h>
#include <cmath>
#include <vector>
#include <memory>
#include <cstdlib>
#include "ShapeAdhoc.hpp"
#include "ShapeSubtype.hpp"
using namespace std;

constexpr size_t kMaxLen = 1<<24;
static void subtypePerf() {
    using namespace Subtype;
    vector<unique_ptr<Shape>> shapes;
    shapes.reserve(kMaxLen);
    for (size_t i = 0; i < kMaxLen; ++i) {
        if (rand() % 100 > 50) {
            shapes.emplace_back(make_unique<Rectangle>(rand() % 10, rand() % 10));
        } else {
            shapes.emplace_back(make_unique<Circle>(rand() % 10));
        }
    }
    for (auto&& shape: shapes) {
        benchmark::DoNotOptimize(shape->getArea());
        benchmark::DoNotOptimize(shape->getPerimeter());
    }

}

static void adhocPerf() {
    using namespace Adhoc;
    vector<Shape> shapes;
    shapes.reserve(kMaxLen);
    for (size_t i = 0; i < kMaxLen; ++i) {
        if (rand() % 100 > 50) {
            shapes.emplace_back(Rectangle{rand() % 10 * 1.0, rand() % 10 * 1.0});
        } else {
            shapes.emplace_back(Circle{rand() % 10 * 1.0});
        }
    }
    for (auto&& shape: shapes) {
        benchmark::DoNotOptimize(getArea(shape));
        benchmark::DoNotOptimize(getPerimeter(shape));
    }

}

int main(int argc, char** argv) {
    for (int i = 0; i<10; ++i) {
        subtypePerf();
        adhocPerf();
    }

    return 0;
}
