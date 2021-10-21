#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <set>
#include <unordered_map>

namespace FSM {

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

class FiniteStateMachine {
private:
    friend class FiniteStateMachineAlgorithm;

    std::vector<std::vector<DirectEdge>> graph;
    std::set<int64_t> terminals;
    const uint64_t alphabet;

public:
    FiniteStateMachine(uint64_t alphabet, uint64_t n);
    void add_edge(int64_t start, int64_t finish, int64_t symbol);
    void add_terminal(int64_t x);
    void print();
};

class FiniteStateMachineAlgorithm {
protected:
    uint64_t alphabet;
    FiniteStateMachine fsm;
    FiniteStateMachine new_fsm;

    static std::vector<std::vector<DirectEdge>>& get_graph(FiniteStateMachine& this_fsm);
    static std::set<int64_t>& get_terminals(FiniteStateMachine& fsm);
    static uint64_t get_alphabet(FiniteStateMachine& fsm);
public:
    explicit FiniteStateMachineAlgorithm(FiniteStateMachine& this_fsm);
    explicit FiniteStateMachineAlgorithm(FiniteStateMachine&& this_fsm);
    virtual FiniteStateMachine iterate() = 0;
};


class ThompsonAlgorithm: FiniteStateMachineAlgorithm {
private:
    std::set<int64_t> new_vertex;
    std::set<int64_t> state_terminals;
    std::vector<Edge> new_edges;
    std::unordered_map<int64_t, int64_t> rename_vertex;

    static int64_t vector_to_int(const std::vector<int64_t>& v);
    static std::vector<int64_t> int_to_vector(int64_t x);

    void determinate();
    void rename_vertices();
    void delete_empty_edges();
    void add_new_terminal(const std::vector<int64_t>& state, int64_t state_code);
    void delete_edges(std::deque<Edge>& edges_to_delete);
public:
    explicit ThompsonAlgorithm(FiniteStateMachine& this_fsm);
    explicit ThompsonAlgorithm(FiniteStateMachine&& this_fsm);
    FiniteStateMachine iterate() override;
};

FiniteStateMachine thompson_algorithm(FiniteStateMachine& fsm) {
    ThompsonAlgorithm TA(fsm);
    return TA.iterate();
}

} // end of namespace FSM

