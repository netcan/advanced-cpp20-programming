/*************************************************************************
    > File Name: TypeList.hpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-10-13 20:03
************************************************************************/
#pragma once
#include <vector>
#include <cstdint>
#include <iostream>
#include <concepts>
#include <algorithm>
#include <chrono>
#include <iterator>
#include <cassert>
#include <string_view>
#include <type_traits>
#include <variant>
#include <functional>
#include <array>
using namespace std; // TODO: remove it in header

template<typename ...>
struct dump;

template<typename T>
struct Return { using type = T; };

struct None;

template <typename ...Ts>
struct TypeList {
    struct IsTypeList {};
    using type = TypeList;
    constexpr static size_t size = sizeof...(Ts);

    template <typename ...T>
    using append = TypeList<Ts..., T...>;

    template <typename ...T>
    using prepend = TypeList<T..., Ts...>;

    template <template<typename...> typename T>
    using to = T<Ts...>;
};

template<typename TypeList>
concept TL = requires {
  typename TypeList::IsTypeList;
  typename TypeList::type;
};

template<TL In, typename Init, template<typename, typename> class Op>
struct Fold: Return<Init> { }; // 边界情况，返回最终的累计元素
template<typename Acc, template<typename, typename> class Op,
         typename H, typename ...Ts> // 通过偏特化取得当前迭代的元素H
struct Fold<TypeList<H, Ts...>, Acc, Op>: // 更新累计值，递归地对剩余元素进行调用
       Fold<TypeList<Ts...>, typename Op<Acc, H>::type, Op> {};

template<TL In, typename Init, template<typename, typename> class Op>
using Fold_t = typename Fold<In, Init, Op>::type;

template<TL In, template <typename> class F>
struct Map; // 元函数声明，接受一个TypeList和一个单参元函数
template<template <typename> class F, typename ...Ts> // 元函数实现
struct Map<TypeList<Ts...>, F>: TypeList<typename F<Ts>::type...> {};

// template<TL In, template <typename> class F>
// class Map {
    // template<TL ACC, typename E>
    // using OP = typename ACC::template append<typename F<E>::type>;
// public:
    // using type = Fold_t<In, TypeList<>, OP>;
// };

template<TL In, template <typename> class F>
using Map_t = typename Map<In, F>::type;

template<TL In, template <typename> class P, TL Out = TypeList<>>
struct Filter: Out { }; // 边界情况，当列表为空得到空列表

template<template <typename> class P, TL Out, typename H, typename ...Ts>
struct Filter<TypeList<H, Ts...>, P, Out>: // 通过偏特化取的当前迭代的元素H
  conditional_t<P<H>::value, // 对H进行判断，若为真则添加到Out列表
    Filter<TypeList<Ts...>, P, typename Out::template append<H>>,
    Filter<TypeList<Ts...>, P, Out>> { }; // 否则丢弃掉当前元素

// template<TL In, template <typename> class P>
// class Filter {
    // template<TL ACC, typename E>
    // using OP = conditional_t<P<E>::value,
                             // typename ACC::template append<E>,
                             // ACC>;
// public:
    // using type = Fold_t<In, TypeList<>, OP>;
// };

template<TL In, template <typename> class P>
using Filter_t = typename Filter<In, P>::type;

// template<TL In,
    // template<typename> typename P,
    // TL S = TypeList<>,
    // TL R = TypeList<>>
// struct Partition {
    // struct type {
        // using Satisfied = S;
        // using Rest = R;
    // };
// };

template<TL In, template<typename> typename P>
class Partition {
    template<typename Arg>
    using NotP = bool_constant<!P<Arg>::value>;
public:
    struct type {
        using Satisfied = Filter_t<In, P>;
        using Rest = Filter_t<In, NotP>;
    };
};

template<TL In, template<typename> class P>
using Partition_t = typename Partition<In, P>::type;

// template<typename H, typename ...Ts, template<typename> typename P, TL S, TL R>
// struct Partition<TypeList<H, Ts...>, P, S, R>: conditional_t<P<H>::value,
      // Partition<TypeList<Ts...>, P, typename S::template append<H>, R>,
      // Partition<TypeList<Ts...>, P, S, typename R::template append<H>>> {};

template<TL... In>
struct Concat;

template<TL... In>
using Concat_t = typename Concat<In...>::type;

template<> struct Concat<>: TypeList<> { }; // 当没有输入时，返回空列表
template<TL In> struct Concat<In>: In { }; // 若只有一个列表，返回本身

template<TL In, TL In2, TL ...Rest> // 多于两个列表的情况，递归地串联
struct Concat<In, In2, Rest...>: Concat_t<Concat_t<In, In2>, Rest...> { };

template<typename ...Ts, typename ...Ts2>
struct Concat<TypeList<Ts...>, TypeList<Ts2...>>: TypeList<Ts..., Ts2...> { };

template<TL In>
class Flatten {
    struct Impl {
        template<typename Acc, typename E>
        struct Append: Acc::template append<E> {};

        template<typename Acc, typename ...Ts>
        struct Append<Acc, TypeList<Ts...>>:
            Concat_t<Acc, typename Flatten<TypeList<Ts...>>::type> {};
    };

    template<typename Acc, typename E>
    using Append = typename Impl::template Append<Acc, E>;

public:
    using type = Fold_t<In, TypeList<>, Append>;
};

template<TL In>
using Flatten_t = typename Flatten<In>::type;

template<TL In, template<typename, typename> class Cmp>
struct Sort: TypeList<> {};

template<template<typename, typename> class Cmp, typename Pivot, typename ...Ts>
class Sort<TypeList<Pivot, Ts...>, Cmp> {
    template<typename E> using LT = Cmp<E, Pivot>;
    using P = Partition_t<TypeList<Ts...>, LT>;
    using SmallerSorted = typename Sort<typename P::Satisfied, Cmp>::type;
    using BiggerSorted =  typename Sort<typename P::Rest,      Cmp>::type;
public:
    using type = Concat_t<typename SmallerSorted::template append<Pivot>, BiggerSorted>;
};

template<TL In, template<typename, typename> class Cmp>
using Sort_t = typename Sort<In, Cmp>::type;

template<TL In, typename E>
struct Elem: false_type { };

template<typename E, typename ...Ts>
struct Elem<TypeList<Ts...>, E>:
    bool_constant<(false || ... || is_same_v<E, Ts>)> { };

// template<TL In, typename E>
// class Elem {
    // template<typename Acc, typename T>
    // using FindE = conditional_t<Acc::value, Acc, is_same<T, E>>;

    // using Found = Fold_t<In, false_type, FindE>;
// public:
    // constexpr static bool value = Found::value;
// };

template<TL In, typename E>
constexpr bool Elem_v = Elem<In, E>::value;

template<TL In>
class Unique {
    template<typename Acc, typename E>
    struct Append: conditional_t<Elem_v<Acc, E>,
        Acc, typename Acc::template append<E>> {};

public:
    using type = Fold_t<In, TypeList<>, Append>;
};

template<TL In>
using Unique_t = typename Unique<In>::type;

template<TL In, template <typename> class F>
struct FindBy: Return<None> {};

template<TL In, template <typename> class F>
using FindBy_t = typename FindBy<In, F>::type;

template<typename H, typename ...Ts, template <typename> class F>
struct FindBy<TypeList<H, Ts...>, F>: conditional_t<F<H>::value,
    Return<H>, FindBy<TypeList<Ts...>, F>> {};

template<TL A, TL B, template<typename, typename> class Pair>
struct CrossProduct {
    template<TL ResultOuter, typename ElemA>
    struct OuterAppend {
        template<TL ResultInner, typename ElemB>
        using InnerAppend = typename ResultInner
                              ::template append<Pair<ElemA, ElemB>>;
        using type = Fold_t<B, ResultOuter, InnerAppend>;
    };

public:
    using type = Fold_t<A, TypeList<>, OuterAppend>;
};

template<typename A, typename B, template<typename, typename> class Pair>
using CrossProduct_t = typename CrossProduct<A, B, Pair>::type;


static void BasicCheck() {
    using AList = TypeList<int, char>;
    static_assert(TL<AList>);
    static_assert(AList::size == 2);
    static_assert(is_same_v<AList::prepend<double>, TypeList<double, int, char>>);
    static_assert(is_same_v<AList::append<double>, TypeList<int, char, double>>);
    static_assert(is_same_v<AList::to<tuple>, tuple<int, char>>);
    static_assert(is_same_v<AList::to<variant>, variant<int, char>>);
}

static_assert(is_same_v<TypeList<int, char>,
        TypeList<int>::append<char>>);

static_assert(is_same_v<Concat_t<TypeList<int, double>, TypeList<char, float>>,
        TypeList<int, double, char, float>>);

static_assert(is_same_v<Concat_t<TypeList<int>, TypeList<char, float>, TypeList<double>>,
        TypeList<int, char, float, double>>);

using LongList = TypeList<char, float, double, int, char>;
static_assert(is_same_v<LongList::prepend<int>, TypeList<int, char, float, double, int, char>>);
using EmptyList = TypeList<>;

template<typename L, typename R>
using SizeCmp = bool_constant<(sizeof(L) < sizeof(R))>;

template<typename T>
using SizeLess4 = bool_constant<(sizeof(T) < 4)>;

template<typename Acc, typename E>
using TypeSizeAcc = integral_constant<size_t, Acc::value + sizeof(E)>;

static_assert(is_same_v<FindBy_t<TypeList<long, double, char>, SizeLess4>, char>);
static_assert(is_same_v<FindBy_t<TypeList<long, double>, SizeLess4>, None>);

using SplitBySize4 = Partition_t<LongList, SizeLess4>;
static_assert(is_same_v<SplitBySize4::Satisfied, TypeList<char, char>>);
static_assert(is_same_v<SplitBySize4::Rest, TypeList<float, double, int>>);

static_assert(is_same_v<Sort_t<LongList, SizeCmp>, TypeList<char, char, float, int, double>>);
static_assert(Fold_t<LongList, integral_constant<int, 0>, TypeSizeAcc>::value == 18);
static_assert(is_same_v<Unique_t<LongList>, TypeList<char, float, double, int>>);
static_assert(is_same_v<Unique_t<EmptyList>, EmptyList>);
static_assert(is_same_v<Filter_t<LongList, SizeLess4>, TypeList<char, char>>);
static_assert(Elem_v<LongList, char>);
static_assert(!Elem_v<LongList, long long>);
static_assert(!Elem_v<EmptyList, char>);

static_assert(is_same_v<Map_t<LongList, add_pointer>,
        TypeList<char*, float*, double*, int*, char*>>);
static_assert(is_same_v<Map_t<EmptyList, add_pointer>,
        EmptyList>);

static_assert(is_same_v<Flatten_t<LongList>, LongList>);

static_assert(is_same_v<
        Flatten_t<TypeList<int, TypeList<char, TypeList<double>>>>
        , TypeList<int, char, double>>);

static_assert(CrossProduct_t<LongList, LongList, pair>::size ==
        LongList::size * LongList::size);

