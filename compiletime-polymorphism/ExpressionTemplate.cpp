/*************************************************************************
    > File Name: expression-template.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-05-30 16:01
************************************************************************/
#include <iostream>
#include <type_traits>
#include <memory>
#include <array>
#include <vector>
#include <cassert>
using namespace std;

namespace ExpressionTemplate {
template<typename T, typename U, typename OP>
struct BinaryExpression {
    BinaryExpression(const T& lhs, const U& rhs, OP op):
        lhs(lhs), rhs(rhs), op(op) { }

    auto operator()() const { return op(lhs, rhs); }

protected:
    T lhs;
    U rhs;
    OP op;
};

template<typename T, typename U, typename OP>
struct BinaryContainerExpression:
    private BinaryExpression<T, U, OP> {
    using Base = BinaryExpression<T, U, OP>;
    using Base::Base; // 继承基类的构造函数

    auto operator[](size_t index) const { // 延迟计算接口
        assert(index < size());
        return Base::op(Base::lhs[index], Base::rhs[index]);
    }
    size_t size() const {
        assert(Base::lhs.size() == Base::rhs.size());
        return Base::lhs.size();
    }
};

template<typename T, typename U, typename OP>
BinaryContainerExpression(T, U, OP) -> BinaryContainerExpression<T, U, OP>;

template<typename T, typename = void>
constexpr bool is_container_v = false;

template<typename T>
constexpr bool is_container_v<T,
          void_t<typename T::value_type,
                 typename T::iterator>> = true;

template<typename T, typename U, typename OP>
constexpr bool is_container_v<BinaryContainerExpression<T, U, OP>> = true;

template<typename T, typename U,
    typename = enable_if_t<is_container_v<T> && is_container_v<U>>>
auto operator+(const T& lhs, const U& rhs) {
    auto plus = [](auto x, auto y) { return x + y; };
    return BinaryContainerExpression(lhs, rhs, plus);
}

}

template<typename> struct dump;

int main(int argc, char** argv) {
    {
        using ExpressionTemplate::is_container_v;
        static_assert(is_container_v<array<int, 5>>);
        static_assert(is_container_v<vector<int>>);
        static_assert(!is_container_v<int>);
    }

    auto plus = [](auto x, auto y) { return x + y; };
    {
        using ExpressionTemplate::BinaryExpression;
        // Scalar test
        // auto expr = [x,y] { return x + y; };
        BinaryExpression expr(5, 3.5, plus);
        // ...
        double res = expr() * 2.0;
        cout << res << endl;
    }
    {
        using ExpressionTemplate::BinaryContainerExpression;
        // Container test
        vector<int> x{1, 2, 3}, y{1, 1, 1};
        int alpha = 4;
        auto add_scaled = [alpha](auto lhs, auto rhs)
                          { return lhs + alpha * rhs; };
        auto expr = BinaryContainerExpression(x, y, add_scaled);
        for(size_t i = 0; i < expr.size(); ++i)
            cout << expr[i] << " ";
        cout << endl;
    }

    {
        // Container test
        vector<int> x{1, 2, 3}, y{1, 1, 1}, z{5, 6, 7};
        {
            using ExpressionTemplate::BinaryContainerExpression;
            auto expr = BinaryContainerExpression(
                    BinaryContainerExpression(x, y, plus),
                    z, plus);
            for(size_t i = 0; i < expr.size(); ++i)
                cout << expr[i] << " ";
            cout << endl;
        }
        {
            using ExpressionTemplate::operator+;
            using ExpressionTemplate::is_container_v;
            auto expr = x + y + z;
            static_assert(is_container_v<decltype(expr)>);
            for(size_t i = 0; i < expr.size(); ++i)
                cout << expr[i] << " ";
            cout << endl;
        }
    }

    return 0;
}
