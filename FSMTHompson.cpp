#include "FSMTHompson.h"

namespace FSM {

FiniteStateMachine::FiniteStateMachine(uint64_t alphabet, uint64_t n):
        alphabet(alphabet) {
    graph.resize(n);
}

void FiniteStateMachine::add_edge(int64_t start, int64_t finish, int64_t symbol) {
    graph[start].push_back({finish, symbol});
}

void FiniteStateMachine::add_terminal(int64_t x) {
    terminals.insert(x);
}

void FiniteStateMachine::print() {
    std::cout << "Number of vertex:\n";
    std::cout << graph.size() << std::endl;
    std::cout << "Edges of new automat:\n";
    for (int i = 0; i < graph.size(); ++i) {
        for (auto edge: graph[i]) {
            char c = 'a' + edge.symbol - 1;
            std::cout << i << " " << edge.final << " " << c << std::endl;
        }
    }
    std::cout << "Terminal vertices are:\n";
    for (auto v: terminals) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

std::vector<std::vector<DirectEdge>>&
FiniteStateMachineAlgorithm::get_graph(FiniteStateMachine& this_fsm) {
    return this_fsm.graph;
}
std::set<int64_t>& FiniteStateMachineAlgorithm::get_terminals(FiniteStateMachine& this_fsm) {
    return this_fsm.terminals;
}
uint64_t FiniteStateMachineAlgorithm::get_alphabet(FiniteStateMachine& this_fsm) {
    return this_fsm.alphabet;
}

FiniteStateMachineAlgorithm::FiniteStateMachineAlgorithm(FiniteStateMachine& this_fsm)
    : alphabet(get_alphabet(this_fsm))
    , fsm(this_fsm)
    , new_fsm(get_graph(fsm).size(), alphabet) {}


FiniteStateMachineAlgorithm::FiniteStateMachineAlgorithm(FiniteStateMachine&& this_fsm)
    : alphabet(get_alphabet(this_fsm))
    , fsm(std::move(this_fsm))
    , new_fsm(get_graph(fsm).size(), alphabet) {}


int64_t ThompsonAlgorithm::vector_to_int(const std::vector<int64_t>& v) {
    int64_t res = 0;
    for (auto number: v) {
        res |= (1 << number);
    }
    return res;
}

std::vector<int64_t> ThompsonAlgorithm::int_to_vector(int64_t x) {
    std::vector<int64_t> res(0);
    int64_t ord = 0;
    while (x > 0) {
        if (x % 2 != 0) {
            res.push_back(ord);
            --x;
        }
        x /= 2;
        ++ord;
    }
    return res;
}

void ThompsonAlgorithm::add_new_terminal(const std::vector<int64_t>& state, int64_t state_code) {
    for (auto v: state) {
        if (get_terminals(fsm).contains(v)) {
            state_terminals.insert(state_code);
        }
    }
}

void ThompsonAlgorithm::determinate() {
    std::deque<int64_t> queue;
    queue.push_back(1);
    new_vertex.insert(1);
    add_new_terminal({0}, 1);
    while(!queue.empty()) {
        auto state = queue.front();
        auto vector_state = int_to_vector(state);
        queue.pop_front();
        std::vector<std::vector<int64_t>> new_vector_state(alphabet + 1);
        for (auto& a: vector_state) {
            for(auto& edge: get_graph(fsm)[a]) {
                new_vector_state[edge.symbol].push_back(edge.final);
            }
        }
        for (char symbol = 1; symbol <= static_cast<char>(alphabet); ++symbol) {
            if (!new_vector_state[symbol].empty()) {
                auto new_state = vector_to_int(new_vector_state[symbol]);
                if (!new_vertex.contains(new_state)) {
                    queue.push_back(new_state);
                    new_vertex.insert(new_state);
                    add_new_terminal(new_vector_state[symbol], new_state);
                }
                new_edges.push_back({state, new_state, symbol});
            }
        }
    }
}

void ThompsonAlgorithm::rename_vertices() {
    int64_t count = 0;
    while(!new_vertex.empty()){
        auto v = new_vertex.lower_bound(0);
        auto value = *v;
        new_vertex.erase(v);
        rename_vertex[value] = count++;
    }
    get_graph(new_fsm).resize(count);
    for (auto edge : new_edges) {
        get_graph(new_fsm)[rename_vertex[edge.start]].push_back({rename_vertex[edge.finish], edge.symbol});
    }
    for (auto v: state_terminals) {
        get_terminals(new_fsm).insert(rename_vertex[v]);
    }
}

void ThompsonAlgorithm::delete_empty_edges() {
    std::deque<Edge> queue;
    std::deque<Edge> edges_to_delete;

    for (int i = 0; i < get_graph(fsm).size(); ++i) {
        for (auto edge: get_graph(fsm)[i]) {
            if (edge.symbol == 0) {
                queue.push_back({i, edge.final, edge.symbol});
            }
        }
    }

    while (!queue.empty()) {
        auto edge = queue.front();
        queue.pop_front();
        if (edge.start != edge.finish) {
            if (get_terminals(fsm).contains(edge.finish)) {
                get_terminals(fsm).insert(edge.start);
            }
            for (auto out_edge : get_graph(fsm)[edge.finish]) {
                get_graph(fsm)[edge.start].push_back(out_edge);
                if (out_edge.symbol == 0) {
                    queue.push_back({edge.start, out_edge.final, out_edge.symbol});
                }
            }
        }
        edges_to_delete.push_back(edge);
    }
    delete_edges(edges_to_delete);
}

void ThompsonAlgorithm::delete_edges(std::deque<Edge>& edges_to_delete) {
    while (!edges_to_delete.empty()) {
        auto edge = edges_to_delete.front();
        edges_to_delete.pop_front();
        auto it = std::find(get_graph(fsm)[edge.start].begin(),
                            get_graph(fsm)[edge.start].end(),
                            DirectEdge(edge.finish, edge.symbol));
        if (it != get_graph(fsm)[edge.start].end()) {
            get_graph(fsm)[edge.start].erase(it);
        }
    }
}

FiniteStateMachine ThompsonAlgorithm::iterate() {
    determinate();
    rename_vertices();
    return new_fsm;
}

ThompsonAlgorithm::ThompsonAlgorithm(FiniteStateMachine&& this_fsm): FiniteStateMachineAlgorithm(this_fsm) {
    delete_empty_edges();
}

ThompsonAlgorithm::ThompsonAlgorithm(FiniteStateMachine& this_fsm): FiniteStateMachineAlgorithm(this_fsm) {
    delete_empty_edges();
}

} // end of namespace FSM
