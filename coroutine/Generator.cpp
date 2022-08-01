/*************************************************************************
    > File Name: Generator.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-12-06 20:48
************************************************************************/

#include <iostream>
#include <coroutine>
#include <tuple>
#include <utility>

struct Generator {
    struct promise_type;
    using handle = std::coroutine_handle<promise_type>;
    struct promise_type {
        Generator get_return_object() { return {handle::from_promise(*this)}; }
        auto initial_suspend() noexcept { return std::suspend_never{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void unhandled_exception() { std::terminate(); }
        void return_void() {}
        auto yield_value(int value) {
            current_value_ = value;
            return std::suspend_always{};
        }

        int current_value_;
    };
    void next() { return coro_handle_.resume(); }
    bool done() { return coro_handle_.done(); }
    int current_value() { return coro_handle_.promise().current_value_; }

    Generator(Generator&& rhs) noexcept:
        coro_handle_(std::exchange(rhs.coro_handle_, {})) {}
    ~Generator() { if(coro_handle_) coro_handle_.destroy(); }
private:
    Generator(handle h): coro_handle_(h) {}
    handle coro_handle_;
};

Generator fibo() {
    int a = 1, b = 1;
    while (a < 1000000) {
        co_yield a;
        std::tie(a, b) = std::make_tuple(b, a + b);
    }
    co_return; // 可省略
}

int main() {
    for (auto f = fibo(); !f.done(); f.next())
        std::cout << f.current_value() << std::endl;
}
