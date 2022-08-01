/*************************************************************************
    > File Name: comparable-crtp.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-05-19 20:47
************************************************************************/
#include <iostream>
#include <tuple>
#include <cassert>
// using namespace std;

namespace crtp {
template<typename Derived>
struct Comparable {
    friend bool operator==(const Derived& lhs, const Derived& rhs)
    { return lhs.tie() == rhs.tie(); }
    friend bool operator!=(const Derived& lhs, const Derived& rhs)
    { return !(lhs == rhs); }

    friend bool operator< (const Derived& lhs, const Derived& rhs)
    { return lhs.tie() < rhs.tie(); }
    friend bool operator> (const Derived& lhs, const Derived& rhs) { return rhs < lhs; }
    friend bool operator<=(const Derived& lhs, const Derived& rhs) { return !(lhs > rhs); }
    friend bool operator>=(const Derived& lhs, const Derived& rhs) { return !(lhs < rhs); }
};

struct Point: Comparable<Point> {
    Point(int x, int y): x(x), y(y) { }
    int x; int y;
    auto tie() const { return std::tie(x, y); }
};
}

namespace classical {
struct Point {
    int x; int y;
    friend bool operator==(const Point& lhs, const Point& rhs)
    { return std::tie(lhs.x, lhs.y) == std::tie(rhs.x, rhs.y); }
    friend bool operator<(const Point& lhs, const Point& rhs)
    { return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y); }

    friend bool operator!=(const Point& lhs, const Point& rhs) { return !(lhs == rhs); }
    friend bool operator> (const Point& lhs, const Point& rhs) { return rhs < lhs;     }
    friend bool operator<=(const Point& lhs, const Point& rhs) { return !(lhs > rhs);  }
    friend bool operator>=(const Point& lhs, const Point& rhs) { return !(lhs < rhs);  }
};
}

namespace modern {
struct Point { // C++20
    int x; int y;
    friend auto operator<=>(const Point& lhs, const Point& rhs) = default;
};
}

#define PointTest() assert((Point{1,2} >= Point{1,0})); \
                    assert((Point{1,2} <= Point{5,0})); \
                    assert((Point{1,2} == Point{1,2})); \
                    assert((Point{1,2} != Point{1,3})); \
                    assert((Point{1,2} <  Point{1,3})); \
                    assert((Point{3,2} >  Point{1,3}))

int main(int argc, char** argv) {
    {
        using crtp::Point;
        PointTest();
    }

    {
        using classical::Point;
        PointTest();
    }

    {
        using modern::Point;
        PointTest();
    }

    return 0;
}
