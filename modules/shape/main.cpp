/*************************************************************************
    > File Name: main.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2021-12-29 22:13
************************************************************************/
import <iostream>;
import shape;

int main() {
    Rectangle r{{1, 2}, {3, 4}};

    std::cout << "area: " << r.area() << '\n';
    std::cout << "width: " << r.width() << '\n';
    std::cout << "height: " << r.height() << '\n';

    return 0;
}
