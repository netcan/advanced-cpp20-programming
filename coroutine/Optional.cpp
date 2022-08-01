/*************************************************************************
    > File Name: Optional.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-12-20 19:39
************************************************************************/
#include <iostream>
#include <optional>
#include <coroutine>

template<typename T>
struct maybe: public std::optional<T> {
    using Base = std::optional<T>;
    using Base::Base;
    maybe(maybe*& p) { p = this; }

    maybe(maybe&& rhs): Base(std::move(rhs)) {}
    maybe(const maybe& rhs): Base(rhs) {}
};

template<typename T, typename... Args>
struct std::coroutine_traits<maybe<T>, Args...> {
    struct promise_type {
        maybe<T> get_return_object() { return maybe<T>{res_}; }
        auto initial_suspend() noexcept { return std::suspend_never{}; }
        auto final_suspend() noexcept { return std::suspend_never{}; }
        void unhandled_exception() { }
        auto yield_value(maybe<T> opt) {
            struct Awaiter {
                bool await_ready() { return opt_.has_value(); }
                void await_suspend(std::coroutine_handle<> handle) {
                    handle.destroy();
                }
                decltype(auto) await_resume() { return *opt_; }
                maybe<T> opt_;
            };
            return Awaiter { std::move(opt) };
        }
        template<typename U>
        void return_value(U&& value) {
            res_->emplace(std::forward<U>(value));
        }
    private:
        maybe<T>* res_ {};
    };
};

maybe<int> read_int() {
    std::cout << "input a number: ";
    int x;
    std::cin >> x;
    if (std::cin.fail()) return std::nullopt;
    return x;
}

maybe<int> compute() {
    int x = co_yield read_int();
    int y = co_yield read_int();
    co_return x + y;
}

std::optional<int> compute2() {
    int x, y;
    if (auto v = read_int()) {
        x = v.value();
    } else { // 错误处理
        return std::nullopt;
    }

    if (auto v = read_int()) {
        y = v.value();
    } else { // 错误处理
        return std::nullopt;
    }
    return x + y;
}

int main(int argc, char** argv) {
#if 1
    auto res = compute();
#else
    auto res = compute2();
#endif
    if (res) {
        std::cout << "result is " << *res << '\n';
    } else {
        std::cout << "result is none\n";
    }
    return 0;
}
