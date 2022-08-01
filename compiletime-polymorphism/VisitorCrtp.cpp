/*************************************************************************
    > File Name: visitor.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-05-12 21:01
************************************************************************/
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <variant>

using namespace std;

struct VideoFile;
struct TextFile;

struct Visitor {
    virtual void visit(VideoFile&) = 0;
    virtual void visit(TextFile&) = 0;
    virtual ~Visitor() = default;
};

struct Elem {
    virtual void accept(Visitor& visit) = 0;
    virtual ~Elem() = default;
};

template<typename T>
struct dump;

template<typename Derived>
struct AutoDispatchElem: Elem {
    void accept(Visitor& visitor) override
    { visitor.visit(static_cast<Derived&>(*this)); }
};

struct VideoFile: AutoDispatchElem<VideoFile> { /* ... */ };
struct TextFile: AutoDispatchElem<TextFile> { /* ... */ };

int main(int argc, char** argv) {
    std::vector<std::unique_ptr<Elem>> elems;
    elems.emplace_back(new VideoFile);
    elems.emplace_back(new TextFile);

    struct ConcreteVisitor: Visitor {
        void visit(VideoFile&) override
        { cout << "process VideoFile ..." << endl; }
        void visit(TextFile&) override
        { cout << "process TextFile ..." << endl; }
    } visitor;

    for (auto&& e: elems) e->accept(visitor);

    return 0;
}
