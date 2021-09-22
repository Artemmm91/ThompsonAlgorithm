#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <set>
#include <unordered_map>
#ifndef THOMPSON_THOMPSON_H
#define THOMPSON_THOMPSON_H


class Thompson {
private:
    struct DirectEdge {
        int64_t final;
        int64_t symbol;

        DirectEdge(int64_t final, int64_t symbol): final(final), symbol(symbol) {}
        bool operator==(const DirectEdge& de) const = default;
    };

    struct Edge{
        int64_t start;
        int64_t finish;
        int64_t symbol;
    };

    uint64_t alphabet;
    std::vector<std::vector<DirectEdge>> graph;
    std::vector<std::vector<DirectEdge>> new_graph;
    std::set<int64_t> new_vertex;
    std::set<int64_t> terminals;
    std::set<int64_t> state_terminals;
    std::set<int64_t> new_terminals;
    std::vector<Edge> new_edges;
    std::unordered_map<int64_t, int64_t> rename_vertex;

    static int64_t vector_to_int(const std::vector<int64_t>& v);
    static std::vector<int64_t> int_to_vector(int64_t x);
public:
    Thompson(uint64_t alphabet, uint64_t n);

    void add_edge(int64_t start, int64_t finish, int64_t symbol);
    void add_new_terminal(const std::vector<int64_t>& state, int64_t state_code);
    void determinate();
    void rename_vertices();
    void print_result();
    void delete_empty_edges();
    void iterate();
    void add_terminal(int64_t x);
};


#endif //THOMPSON_THOMPSON_H
