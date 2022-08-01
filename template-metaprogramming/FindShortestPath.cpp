/*************************************************************************
    > File Name: TypeList.hpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-10-13 20:03
************************************************************************/

#include "TypeList.hpp"

template<typename Node>
concept Vertex = requires {
    Node::id; // 通过特征::id来判定结点Node
};

template<Vertex F, Vertex T>
struct Edge {
    using From = F;
    using To = T;
};

template<typename Node = void>
requires (Vertex<Node> || is_void_v<Node>)
struct EdgeTrait {
    template<typename Edge> using IsFrom  = is_same<typename Edge::From, Node>;
    template<typename Edge> using IsTo    = is_same<typename Edge::To, Node>;
    template<typename Edge> using GetFrom = Return<typename Edge::From>;
    template<typename Edge> using GetTo   = Return<typename Edge::To>;
};

template<typename Link, TL Out = TypeList<>>
struct Chain;

template<Vertex F, TL Out>
struct Chain<auto(*)(F) -> void, Out> {
    using From = F;
    using type = Out;
};

template<Vertex F, typename T, TL Out>
class Chain<auto(*)(F) -> T, Out> {
    using To = typename Chain<T, Out>::From;
public:
    using From = F;
    using type = typename Chain<T,
          typename Out::template append<Edge<From, To>>>::type;
};

template<typename Link>
using Chain_t = typename Chain<Link>::type;

#define node(node) auto(*) (node)
#define link(link) link -> void

template<typename... Chains>
class Graph {
    using Edges = Unique_t<Concat_t<Chain_t<Chains>...>>;

///////////////////////////////////////////////////////////////////////////////
    template<Vertex From, Vertex Target, TL Path>
    struct PathFinder;

    // Reach Target!
    template<Vertex Target, TL Path>
    struct PathFinder<Target, Target, Path>:
        Path::template append<Target> { };

    // Skip cycle
    template<Vertex CurrNode, Vertex Target, TL Path>
    requires (Elem_v<Path, CurrNode>)
    struct PathFinder<CurrNode, Target, Path>: TypeList<> {}; // return empty path

    template<Vertex CurrNode, Vertex Target, TL Path = TypeList<>>
    class PathFinder {
        using EdgesFrom = Filter_t<Edges, EdgeTrait<CurrNode>::template IsFrom>;
        using NextNodes = Map_t<EdgesFrom, EdgeTrait<>::GetTo>;

        template<Vertex AdjacentNode>
        using GetPath = PathFinder<AdjacentNode, Target,
                                   typename Path::template append<CurrNode>>;

        using AllPathFromCurNode = Map_t<NextNodes, GetPath>;

        template<TL AccMinPath, TL Path_>
        using GetMinPath = conditional_t<AccMinPath::size == 0 ||
                                         (AccMinPath::size > Path_::size && Path_::size > 0), Path_, AccMinPath>;
    public:
        using type = Fold_t<AllPathFromCurNode, TypeList<>, GetMinPath>;
    };

    template<Vertex From, Vertex Target>
    using PathFinder_t = typename PathFinder<From, Target>::type;

///////////////////////////////////////////////////////////////////////////////

    template<typename NodeType>
    struct PathRef {
        const NodeType* path;
        size_t sz;
    };

    template<Vertex Node, Vertex... Nodes>
    class PathStorage {
        using NodeType = std::decay_t<decltype(Node::id)>;
        constexpr static NodeType
            pathStorage[] { Node::id, Nodes::id... };
    public:
        constexpr static PathRef<NodeType> path {
            .path = pathStorage,
            .sz   = sizeof...(Nodes) + 1,
        };
    };

    using AllNodePairs = CrossProduct_t<
        Unique_t<Map_t<Edges, EdgeTrait<>::GetFrom>>,
        Unique_t<Map_t<Edges, EdgeTrait<>::GetTo>>,
        std::pair>;

    template<typename NodePair>
    using IsNonEmptyPath = bool_constant<(PathFinder_t<typename NodePair::first_type,
                                                       typename NodePair::second_type>::size > 0)>;

    using ReachableNodePairs = Filter_t<AllNodePairs, IsNonEmptyPath>;

    template<typename NodePair>
    using SavePath = Return<std::pair<NodePair,
                                      typename PathFinder_t<typename NodePair::first_type,
                                                            typename NodePair::second_type>::template to<PathStorage>>>;

    using SavedAllPath = Map_t<ReachableNodePairs, SavePath>;

///////////////////////////////////////////////////////////////////////////////

    template<typename NodeType, Vertex From, Vertex Target, typename PathStorage_>
    constexpr static bool matchPath(NodeType from, NodeType to,
            PathRef<NodeType>& result, std::pair<std::pair<From, Target>, PathStorage_>) {
        if (From::id == from && Target::id == to) {
            result = PathStorage_::path;
            return true;
        }
        return false;
    }

    template<typename NodeType, typename ...PathPairs>
    constexpr static void matchPath(NodeType from, NodeType to,
            PathRef<NodeType>& result, TypeList<PathPairs...>) {
        (matchPath(from, to, result, PathPairs{}) || ...);
    }

public:
    // export compile/run-time interface
    template<typename NodeType>
    constexpr static PathRef<NodeType> getPath(NodeType from, NodeType to) {
        PathRef<NodeType> result{};
        matchPath(from, to, result, SavedAllPath{});
        return result;
    }
};

///////////////////////////////////////////////////////////////////////////////
template<char ID>
struct Node { constexpr static char id = ID; };

using A = Node<'A'>;
using B = Node<'B'>;
using C = Node<'C'>;
using D = Node<'D'>;
using E = Node<'E'>;

using g = Graph<
    link(node(A) -> node(B) -> node(C) -> node(D)),
    link(node(A) -> node(C)),   // test shortest path: A -> C -> D
    link(node(B) -> node(A)),   // test cycle
    link(node(A) -> node(E)) >; // test D -> E unreachable

static_assert(g::getPath('A', 'D').sz == 3); // compile-time test
static_assert(g::getPath('A', 'A').sz == 1);
static_assert(g::getPath('B', 'D').sz == 3);
static_assert(g::getPath('B', 'E').sz == 3);
static_assert(g::getPath('D', 'E').sz == 0);

int main(int argc, char** argv) {
    char from = 'A';
    char to = 'D';
    if (argc > 2) {
        from = argv[1][0]; // A
        to = argv[2][0]; // D
    }
    auto path = g::getPath(from, to); // runtime test
    std::cout << "from " << from << " to " << to
        << " path size: " << path.sz << std::endl;
    for (size_t i = 0; i < path.sz; ++i) {
        std::cout << path.path[i];
        if (i != path.sz - 1) {
            std::cout << "->";
        }
    }
    std::cout << std::endl;

    return 0;
}

