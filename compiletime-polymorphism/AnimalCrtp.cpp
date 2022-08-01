/*************************************************************************
    > File Name: animal-crtp.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-05-18 21:53
************************************************************************/
#include <iostream>
using namespace std;

template<typename Derived>
struct Animal {
    void bark() { static_cast<Derived&>(*this).barkImpl(); }
};

class Cat: public Animal<Cat> {
    friend Animal;
    void barkImpl() { cout << "Miaowing!" << endl; }
};

class Dog: public Animal<Dog> {
    friend Animal;
    void barkImpl() { cout << "Bow wow!" << endl; }
};

template<typename T>
void play(Animal<T>& animal) { animal.bark(); }

int main(int argc, char** argv) {
    Cat cat; play(cat); // Miaowing!
    Dog dog; play(dog); // Bow wow!

    return 0;
}
