/*************************************************************************
    > File Name: PromiseCtor.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-12-16 21:32
************************************************************************/
#include <coroutine>
#include <exception>
#include <iostream>

struct Future {
    struct promise_type {
        template<typename Obj>
        promise_type(Obj&&, int v) { // 匹配成员函数、lambda的情况
            std::cout << "member function or lambda, v = " << v << std::endl;
        }

        promise_type(int v) { // 匹配自由函数
            std::cout << "free function, v = " << v << std::endl;
        }

        promise_type() {
            std::cout << "default ctor" << std::endl;
        }

        auto initial_suspend() { return std::suspend_never{}; }
        auto final_suspend() noexcept { return std::suspend_never{}; }
        auto return_void() { }
        Future get_return_object() { return {}; }
        void unhandled_exception() { std::terminate(); }
    };
};

struct Class {
    Future member_coro(int) {
        co_return;
    }
};

Future free_coro(int) {
    co_return;
}

Future free_coro2(std::string) {
    co_return;
}

int main() {
    Class obj;
    obj.member_coro(0);

    auto lambda_coro = [](int) -> Future {
        co_return;
    };
    lambda_coro(1);

    free_coro(2);
    free_coro2("hello");

    return 0;
}
