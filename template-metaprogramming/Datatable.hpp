/*************************************************************************
    > File Name: Datatable.cpp
    > Author: Netcan
    > Descripton: constexpr Datatable
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-14 21:16
************************************************************************/
#pragma once

#include <algorithm>
#include <concepts>
#include <type_traits>
#include <bitset>
#include <cstdio>
#include "TypeList.hpp"

// <key, valuetype>
template<auto Key, typename ValueType, size_t Dim = 1>
struct Entry {
    constexpr static auto key = Key;
    constexpr static size_t dim = Dim;
    constexpr static bool isArray = Dim > 1;
    using type = ValueType;
};

template<auto Key, typename ValueType, size_t Dim>
struct Entry<Key, ValueType[Dim]>: Entry<Key, ValueType, Dim> { };

template<typename E>
concept KVEntry = requires {
    typename E::type;
    requires is_standard_layout_v<typename E::type>;
    requires is_trivial_v<typename E::type>;
    { E::key } -> convertible_to<size_t>;
    { E::dim } -> convertible_to<size_t>;
};

static_assert(KVEntry<Entry<0, char[10]>>);

///////////////////////////////////////////////////////////////////////////////

template<TL Es = TypeList<>, TL Gs = TypeList<>>
struct GroupEntriesTrait: Gs { };

template<TL Es = TypeList<>, TL Gs = TypeList<>>
using GroupEntriesTrait_t = typename GroupEntriesTrait<Es, Gs>::type;

template<KVEntry H, KVEntry ...Ts, TL Gs>
class GroupEntriesTrait<TypeList<H, Ts...>, Gs> {
    template<KVEntry E>
    using GroupPrediction = std::bool_constant<(H::dim == E::dim &&
            sizeof(typename H::type) == sizeof(typename E::type) &&
            alignof(typename H::type) == alignof(typename E::type))>;

    using Group = Partition_t<TypeList<H, Ts...>, GroupPrediction>;
    using Satisfied = typename Group::Satisfied;
    using Rest = typename Group::Rest;
public:
    using type = typename GroupEntriesTrait<Rest, typename Gs::template append<Satisfied>>::type;
};

template<TL Gs>
class GroupIndexTrait {
    template<size_t GroupIdx = 0, size_t InnerIdx = 0, TL Res = TypeList<>>
    struct Index {
        constexpr static size_t GroupIndex = GroupIdx;
        constexpr static size_t InnerIndex = InnerIdx;
        using Result = Res;
    };

    template<typename Acc, TL G>
    class AddGroup {
        constexpr static size_t GroupIndex = Acc::GroupIndex;
        template<typename Acc_, KVEntry E>
        class AddKey {
            constexpr static size_t InnerIndex = Acc_::InnerIndex;
            struct KeyWithIndex {
                constexpr static auto key = E::key;
                constexpr static auto id = (GroupIndex << 16) | InnerIndex;
            };
            using Result = typename Acc_::Result::template append<KeyWithIndex>;
        public:
            using type = Index<GroupIndex + 1, InnerIndex + 1, Result>;
        };
        using Result = typename Acc::Result;
    public:
        using type = Fold_t<G, Index<GroupIndex + 1, 0, Result>, AddKey>;
    };
public:
    using type = typename Fold_t<Gs, Index<>, AddGroup>::Result;
};

template<TL Gs>
using GroupIndexTrait_t = typename GroupIndexTrait<Gs>::type;

template <TL Es>
class Datatable {
    template<KVEntry EH, KVEntry ...ET>
    class GenericRegion {
        constexpr static size_t numberOfEntries = sizeof...(ET) + 1;
        constexpr static size_t maxSize = max(alignof(typename EH::type),
                                              sizeof(typename EH::type)) * EH::dim;
        char data[numberOfEntries][maxSize];
    public:
        bool getData(size_t nthData, void* out, size_t len) {
            if (nthData >= numberOfEntries) [[unlikely]] { return false; }
            copy_n(data[nthData], min(len, maxSize), reinterpret_cast<char*>(out));
            return true;
        }
        bool setData(size_t nthData, const void* value, size_t len) {
            if (nthData >= numberOfEntries) [[unlikely]] { return false; }
            copy_n(reinterpret_cast<const char*>(value), min(len, maxSize), data[nthData]);
            return true;
        }
    };

    template<typename... R>
    class Regions {
        tuple<R...> regions;

        template<size_t I, typename OP>
        bool forData(OP&& op, size_t index) {
            size_t regionIdx = index >> 16;
            size_t nthData = index & 0xFFFF;
            if (I == regionIdx) { return op(std::get<I>(regions), nthData); }
            return false;
        }

        template<typename OP, size_t... Is>
        bool forData(std::index_sequence<Is...>, OP&& op, size_t index) {
            return (forData<Is>(std::forward<OP>(op), index) || ...);
        }

    public:
        bool getData(size_t index, void* out, size_t len) {
            auto op = [&](auto& region, size_t nthData)
                         { return region.getData(nthData, out, len); };
            return forData(std::make_index_sequence<sizeof...(R)>{}, op, index);
        }

        bool setData(size_t index, const void* value, size_t len) {
            auto op = [&](auto& region, size_t nthData)
                         { return region.setData(nthData, value, len); };
            return forData(std::make_index_sequence<sizeof...(R)>{}, op, index);
        }
    };

    template<TL Gs>
    class GenericRegionTrait {
        template<TL G>
        using ToRegion = Return<typename G::template to<GenericRegion>>;
    public:
        using type = Map_t<Gs, ToRegion>;
    };

    template<TL Gs>
    using GenericRegionTrait_t = typename GenericRegionTrait<Gs>::type;

    template<typename ...Indexes>
    struct Indexer {
        size_t keyToId[sizeof...(Indexes)];
        std::bitset<sizeof...(Indexes)> mask;
        constexpr Indexer() {
            constexpr size_t IndexSize = sizeof...(Indexes);
            static_assert(((Indexes::key < IndexSize) && ...), "key is out of size");
            (void(keyToId[Indexes::key] = Indexes::id), ...);
        }
    };

    using Gs = GroupEntriesTrait_t<Es>;
    using RegionsClass = typename GenericRegionTrait_t<Gs>::template to<Regions>;
    using IndexerClass = typename GroupIndexTrait_t<Gs>::template to<Indexer>;

    RegionsClass regions;
    IndexerClass indexer;
public:
    bool getData(size_t key, void* out, size_t len = -1) {
        if (key >= Es::size || ! indexer.mask[key]) { return false; }
        return regions.getData(indexer.keyToId[key], out, len);
    }
    bool setData(size_t key, const void* value, size_t len = -1) {
        if (key >= Es::size) { return false; }
        return indexer.mask[key] =
            regions.setData(indexer.keyToId[key], value, len);
    }
    void dumpGroupInfo() {
        printf("sizeof Datatable = %zu\n", sizeof(Datatable));
        printf("sizeof Region = %zu\n", sizeof(RegionsClass));
        printf("sizeof Indexer = %zu\n", sizeof(IndexerClass));
        for (size_t k = 0; k < Es::size; ++k) {
            printf("key = %d id = 0x%05x group = %d subgroup = %d\n", k,
                    indexer.keyToId[k],
                    indexer.keyToId[k] >> 16,
                    indexer.keyToId[k] & 0xFFFF);
        }
    }
};

