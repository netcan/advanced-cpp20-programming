/*************************************************************************
    > File Name: point.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2022-01-03 14:57
************************************************************************/
#include <tinyxml2.h>
#include <optional>
#include <iostream>
#include <string_view>

struct Point {
    double x;
    double y;
};

std::optional<Point> load_point(std::string_view xml_path) {
    using namespace tinyxml2;
    XMLDocument doc;
    if (doc.LoadFile(xml_path.data()) != XML_SUCCESS) {
        return std::nullopt;
    }

    auto root = doc.FirstChildElement("point");
    if (! root) { return std::nullopt; }

    Point res{};

    if (auto x = root->FirstChildElement("x");
        x == nullptr || x->QueryDoubleText(&res.x) != XML_SUCCESS) {
        return std::nullopt;
    }

    if (auto y = root->FirstChildElement("y");
        y == nullptr || y->QueryDoubleText(&res.y) != XML_SUCCESS) {
        return std::nullopt;
    }

    return res;
}

int main(int argc, char** argv) {
    auto p = load_point("point.xml");
    if (p.has_value()) {
        std::cout << "(" << p->x << ", " << p->y << ")\n";
    } else {
        std::cout << "load faild point" << std::endl;
    }

    return 0;
}
