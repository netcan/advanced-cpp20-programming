/*************************************************************************
    > File Name: ShapeSubtype.hpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-02-02 21:47
************************************************************************/
#pragma once
#include <cmath>

namespace Subtype {
struct Shape {
    virtual ~Shape() = default;
    virtual double getArea() const = 0;
    virtual double getPerimeter() const = 0;
};

struct Circle: Shape {
    Circle(double r): r_(r) {}
    double getArea() const override
    { return M_PI * r_ * r_; }
    double getPerimeter() const override
    { return 2 * M_PI * r_; }
private:
    double r_;
};

struct Rectangle: Shape {
    Rectangle(double w, double h): w_(w), h_(h) {}
    double getArea() const override
    { return w_ * h_; }
    double getPerimeter() const override
    { return 2 * (w_ + h_); }
private:
    double w_;
    double h_;
};
}
