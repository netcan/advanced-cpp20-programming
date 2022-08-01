/*************************************************************************
    > File Name: ShapeSubtype.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-01-28 22:01
************************************************************************/
#include <iostream>
#include <memory>
#include "ShapeSubtype.hpp"
using namespace Subtype;

int main(int argc, char** argv) {
    std::unique_ptr<Shape> shape = std::make_unique<Circle>(2);
    // shape area: 12.5664 perimeter: 12.5664
    std::cout << "shape area: " << shape->getArea()
        << " perimeter: " << shape->getPerimeter() << std::endl;

    shape = std::make_unique<Rectangle>(2, 3);
    // shape area: 6 perimeter: 10
    std::cout << "shape area: " << shape->getArea()
        << " perimeter: " << shape->getPerimeter() << std::endl;

    return 0;
}
