/*************************************************************************
    > File Name: Views.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-08-16 20:48
************************************************************************/
#pragma once
// g++11 -std=c++2a
#include <variant>
#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;
namespace views = std::views;

///////////////////////////////////////////////////////////////////////////////
// group_by_view
template<ranges::input_range Rng, typename Pred>
requires ranges::view<Rng>
struct group_by_view: ranges::view_interface<group_by_view<Rng, Pred>> {
    group_by_view() = default;
    group_by_view(Rng r, Pred p): r(std::move(r)), p(std::move(p)) {}

private:
    struct group_iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = ranges::subrange<ranges::iterator_t<Rng>>;

        group_iterator& operator++() {
            cur = next_cur;
            if (cur != last) {
                next_cur = ranges::find_if_not(ranges::next(cur), last, [&](auto&& elem)
                                                                        { return p(*cur, elem); });
            }
            return *this;
        }
        group_iterator operator++(int) {
            group_iterator tmp(*this);
            ++*this;
            return tmp;
        }
        value_type operator*() const { return {cur, next_cur}; }

        bool operator==(std::default_sentinel_t) const { return cur == last; }

        // C++20 defaulted comparison operators
        bool operator==(const group_iterator&) const = default;

        Pred p;
        ranges::iterator_t<Rng> cur{};
        ranges::iterator_t<Rng> next_cur{};
        ranges::sentinel_t<Rng> last{};
    };
public:

    group_iterator begin() {
        auto beg = ranges::begin(r);
        auto end = ranges::end(r);
        return {p, beg,
            ranges::find_if_not(ranges::next(beg), end,
                                [&](auto&& elem) { return p(*beg, elem); }),
                end};
    }

    std::default_sentinel_t end() { return {}; }

    Rng r;
    Pred p;
};

struct _Group_by: views::__adaptor::_RangeAdaptor<_Group_by> {
    template<ranges::viewable_range Rng, typename Pred>
    constexpr auto operator()(Rng&& r, Pred&& p) const {
        return group_by_view{std::forward<Rng>(r), std::forward<Pred>(p)};
    }
    static constexpr int _S_arity = 2;
    using views::__adaptor::_RangeAdaptor<_Group_by>::operator();
};
inline constexpr _Group_by group_by;

///////////////////////////////////////////////////////////////////////////////
// concat_view
template<ranges::input_range ... Rngs>
requires (ranges::view<Rngs> && ...) && (sizeof...(Rngs) > 1)
struct concat_view {
    static constexpr size_t nRngs = sizeof...(Rngs);
    using RNGs = std::tuple<Rngs...>;
    concat_view() = default;
    explicit concat_view(Rngs... rngs): rngs{std::move(rngs)...}
    { }

    struct concat_iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = std::common_type_t<ranges::range_value_t<Rngs>...>;

        concat_iterator() = default;
        concat_iterator(RNGs* rngs): rngs(rngs)
        { its.template emplace<0>(ranges::begin(std::get<0>(*rngs))); }

        concat_iterator& operator++() {
            // TODO: check empty range, skip it
            std::visit([&](auto&& iter) {
                constexpr size_t idx = iter.value;
                if ( (iter.iterator = ranges::next(iter.iterator))
                        == ranges::end(std::get<idx>(*rngs)) ) {
                    if constexpr (idx + 1 < nRngs) {
                        its.template emplace<idx + 1>(
                            ranges::begin(std::get<idx + 1>(*rngs))
                        );
                    }
                }
            }, its);
            return *this;
        }
        concat_iterator operator++(int) {
            concat_iterator tmp(*this);
            ++*this;
            return tmp;
        }
        using reference = std::common_reference_t<ranges::range_reference_t<Rngs>...>;
        reference operator*() const {
            return std::visit([](auto&& iter) -> reference {
                return *iter.iterator;
            }, its);
        }

        bool operator==(std::default_sentinel_t) const {
            return its.index() == nRngs - 1 &&
                (std::get<nRngs - 1>(its).iterator ==
                 ranges::end(std::get<nRngs - 1>(*rngs)));
        }

        bool operator==(const concat_iterator&) const = default;

    private:
        template<size_t N, typename Rng>
        struct iterator_with_index: std::integral_constant<size_t, N> {
            iterator_with_index() = default;
            iterator_with_index(ranges::iterator_t<Rng> iterator):
                iterator(std::move(iterator)) {}
            ranges::iterator_t<Rng> iterator;
            bool operator==(const iterator_with_index&) const = default;
        };

        template<size_t ...Is>
        static constexpr auto iteratorVariantGenerator(std::index_sequence<Is...>)
            -> std::variant<iterator_with_index<Is, std::tuple_element_t<Is, RNGs>>...>;

        decltype(iteratorVariantGenerator(std::make_index_sequence<nRngs>{})) its;
        RNGs* rngs {};
    };

    concat_iterator begin() { return {&this->rngs}; }

    std::default_sentinel_t end() { return {}; }

private:
    RNGs rngs;
};

inline constexpr auto concat = [] <ranges::viewable_range... Rngs> (Rngs&&... rngs) {
    return concat_view{std::forward<Rngs>(rngs)...};
};

///////////////////////////////////////////////////////////////////////////////
// repeat_n
template<typename Value>
struct repeat_n_view: ranges::view_interface<repeat_n_view<Value>> {
    repeat_n_view() = default;
    repeat_n_view(size_t n, Value value): n(n), value(std::move(value)) { }

private:
    struct repeat_iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = Value;

        repeat_iterator& operator++() { --n; return *this; }
        repeat_iterator operator++(int) {
            repeat_iterator tmp(*this);
            ++*this;
            return tmp;
        }
        decltype(auto) operator*() const { return *value; }

        bool operator==(std::default_sentinel_t) const { return n == 0; }
        bool operator==(const repeat_iterator&) const = default;

        size_t n{};
        Value* value{};
    };

public:
    repeat_iterator begin() { return {n, &value}; }
    std::default_sentinel_t end() { return {}; }

private:
    size_t n;
    Value value;
};

inline constexpr auto repeat_n = []<typename Value>(Value&& value, size_t n) {
    return repeat_n_view{n, std::forward<Value>(value)};
};

///////////////////////////////////////////////////////////////////////////////
// chunk_view
template<ranges::input_range Rng>
requires ranges::view<Rng>
struct chunk_view: ranges::view_interface<chunk_view<Rng>> {
    chunk_view() = default;
    chunk_view(Rng r, size_t n): r(std::move(r)), n(n) {}

private:
    struct chunk_iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = ranges::subrange<ranges::iterator_t<Rng>>;

        chunk_iterator& operator++() {
            cur = next_cur;
            if (cur != last) {
                next_cur = ranges::next(cur, n, last);
            }
            return *this;
        }
        chunk_iterator operator++(int) {
            chunk_iterator tmp(*this);
            ++*this;
            return tmp;
        }
        value_type operator*() const { return {cur, next_cur}; }

        bool operator==(std::default_sentinel_t) const { return cur == last; }
        bool operator==(const chunk_iterator&) const = default;

        size_t n{};
        ranges::iterator_t<Rng> cur{};
        ranges::iterator_t<Rng> next_cur{};
        ranges::sentinel_t<Rng> last{};
    };
public:

    chunk_iterator begin() {
        auto beg = ranges::begin(r);
        auto end = ranges::end(r);
        return {n, beg, ranges::next(beg, n, end), end};
    }

    std::default_sentinel_t end() { return {}; }

    Rng r;
    size_t n;
};

struct _Chunk: views::__adaptor::_RangeAdaptor<_Chunk> {
    template<ranges::viewable_range Rng>
    constexpr auto operator()(Rng&& r, size_t n) const {
        return chunk_view{std::forward<Rng>(r), n};
    }
    static constexpr int _S_arity = 2;
    using views::__adaptor::_RangeAdaptor<_Chunk>::operator();
};
inline constexpr _Chunk chunk;

///////////////////////////////////////////////////////////////////////////////
// stride_view
template<ranges::input_range Rng>
requires ranges::view<Rng>
struct stride_view: ranges::view_interface<stride_view<Rng>> {
    stride_view() = default;
    stride_view(Rng rng, size_t stride): rng(std::move(rng)), stride(stride) { }
private:
    struct stride_iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = ranges::range_value_t<Rng>;

        stride_iterator& operator++() {
            ranges::advance(cur, stride, last);
            return *this;
        }
        stride_iterator operator++(int) {
            stride_iterator tmp(*this);
            ++*this;
            return tmp;
        }
        decltype(auto) operator*() const { return *cur; }
        bool operator==(std::default_sentinel_t) const { return cur == last; }
        bool operator==(const stride_iterator&) const = default;

        ranges::iterator_t<Rng> cur{};
        size_t stride{};
        ranges::sentinel_t<Rng> last{};
    };

public:
    stride_iterator begin() {
        return {ranges::begin(rng), stride, ranges::end(rng)};
    }
    std::default_sentinel_t end() { return {}; }
    Rng rng;
    size_t stride;
};

struct _Stride: views::__adaptor::_RangeAdaptor<_Stride> {
    template<ranges::viewable_range Rng>
    constexpr auto operator()(Rng&& r, size_t n) const {
        return stride_view{std::forward<Rng>(r), n};
    }
    static constexpr int _S_arity = 2;
    using views::__adaptor::_RangeAdaptor<_Stride>::operator();
};
inline constexpr _Stride stride;

///////////////////////////////////////////////////////////////////////////////
// transpose
struct _Transpose: views::__adaptor::_RangeAdaptorClosure {
    template<ranges::viewable_range Rng>
    constexpr auto operator()(Rng&& r) const {
        auto flat = r | views::join;
        auto height = ranges::distance(r);
        auto width = ranges::distance(flat) / height;
        auto inner = [=](auto colIdx) mutable {
            return flat | views::drop(colIdx) | stride(width);
        };
        return views::iota(0, width) | views::transform(inner);
    }
};
inline constexpr _Transpose transpose;

///////////////////////////////////////////////////////////////////////////////
// print_range
void print_range(ranges::viewable_range auto&& range, bool need_delim = false, size_t depth = 0) {
    std::cout << "[";
    bool first_token = false;
    for (auto&& v: range) {
        if (first_token && need_delim) std::cout << ", ";
        if constexpr (requires { print_range(v); }) {
            if (first_token) {
                std::cout << "\n";
                for (auto d = 0; d < depth + 1; ++d) std::cout << " ";
            }
            print_range(v, need_delim, depth + 1);
        } else {
            std::cout << v;
        }
        first_token = true;
    }
    std::cout << "]";
}
