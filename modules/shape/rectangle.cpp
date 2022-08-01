/*************************************************************************
    > File Name: rectangle.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-12-29 22:16
************************************************************************/
module;
#include <cmath>
module shape;

int Rectangle::width() {
    return abs(bottomRight.x - topLeft.x);
}

int Rectangle::height() {
    return abs(bottomRight.y - topLeft.y);
}

int Rectangle::area() {
    return width() * height();
}
