/*************************************************************************
    > File Name: ShapeAdhoc.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-01-28 22:26
************************************************************************/
#include "ShapeAdhoc.hpp"
#include <iostream>
using namespace Adhoc;

int main(int argc, char** argv) {
    Shape shape = Circle{2};
    // shape area: 12.5664 perimeter: 12.5664
    std::cout << "shape area: " << getArea(shape)
        << " perimeter: " << getPerimeter(shape) << std::endl;

    shape = Rectangle{2, 3};
    // shape area: 6 perimeter: 10
    std::cout << "shape area: " << getArea(shape)
        << " perimeter: " << getPerimeter(shape) << std::endl;
    return 0;
}
