/*************************************************************************
    > File Name: ShapeAdhoc.h
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-02-02 21:45
************************************************************************/
#pragma once
#include <variant>
#include <cmath>
namespace Adhoc {
struct Circle { double r; };
// Circle的一系列操作
double getArea(const Circle& c)
{ return M_PI * c.r * c.r; }
double getPerimeter(const Circle& c)
{ return 2 * M_PI * c.r; };

struct Rectangle { double w; double h; };
// Rectangle的一系列操作
double getArea(const Rectangle& r)
{ return r.w * r.h; }
double getPerimeter(const Rectangle& r)
{ return 2 * (r.w + r.h); };

// 通过加法类型定义一个统一的类型Shape，其拥有不同的形状，从而实现运行时多态
using Shape = std::variant<Circle, Rectangle>;

// 统一类型Shape的一系列多态行为
double getArea(const Shape& s)
{ return std::visit([](const auto& data) { return getArea(data); }, s); }
double getPerimeter(const Shape& s)
{ return std::visit([](const auto& data) { return getPerimeter(data); }, s);  };

}

