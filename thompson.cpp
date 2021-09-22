//
// Created by Artem Mustafin on 22.09.2021.
//

#include "thompson.h"

int64_t Thompson::vector_to_int(const std::vector<int64_t>& v) {
    int64_t res = 0;
    for (auto number: v) {
        res |= (1 << number);
    }
    return res;
}

std::vector<int64_t> Thompson::int_to_vector(int64_t x) {
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

Thompson::Thompson(uint64_t alphabet, uint64_t n):
        alphabet(alphabet) {
    graph.resize(n);
}

void Thompson::add_edge(int64_t start, int64_t finish, int64_t symbol) {
    graph[start].push_back({finish, symbol});
}

void Thompson::add_new_terminal(const std::vector<int64_t>& state, int64_t state_code) {
    for (auto v: state) {
        if (terminals.contains(v)) {
            state_terminals.insert(state_code);
        }
    }
}

void Thompson::add_terminal(int64_t x) {
    terminals.insert(x);
}

void Thompson::determinate() {
    std::deque<int64_t> Q;
    Q.push_back(1);
    new_vertex.insert(1);
    add_new_terminal({0}, 1);
    while(!Q.empty()) {
        auto state = Q.front();
        auto vector_state = int_to_vector(state);
        Q.pop_front();
        std::vector<std::vector<int64_t>> new_vector_state(alphabet + 1);
        for (auto& a: vector_state) {
            for(auto& edge: graph[a]) {
                new_vector_state[edge.symbol].push_back(edge.final);
            }
        }
        for (char symbol = 1; symbol <= static_cast<char>(alphabet); ++symbol) {
            if (!new_vector_state[symbol].empty()) {
                auto new_state = vector_to_int(new_vector_state[symbol]);
                if (!new_vertex.contains(new_state)) {
                    Q.push_back(new_state);
                    new_vertex.insert(new_state);
                    add_new_terminal(new_vector_state[symbol], new_state);
                }
                new_edges.push_back({state, new_state, symbol});
            }
        }
    }
}

void Thompson::rename_vertices() {
    int64_t count = 0;
    while(!new_vertex.empty()){
        auto v = new_vertex.lower_bound(0);
        auto value = *v;
        new_vertex.erase(v);
        rename_vertex[value] = count++;
    }
    new_graph.resize(count);
    for (auto edge : new_edges) {
        new_graph[rename_vertex[edge.start]].push_back({rename_vertex[edge.finish], edge.symbol});
    }
    for (auto v: state_terminals) {
        new_terminals.insert(rename_vertex[v]);
    }
}

void Thompson::print_result() {
    std::cout << "Number of vertex:\n";
    std::cout << new_graph.size() << std::endl;
    std::cout << "Edges of new automat:\n";
    for (int i = 0; i < new_graph.size(); ++i) {
        for (auto edge: new_graph[i]) {
            char c = 'a' + edge.symbol - 1;
            std::cout << i << " " << edge.final << " " << c << std::endl;
        }
    }
    std::cout << "Terminal vertices are:\n";
    for (auto v: new_terminals) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

void Thompson::delete_empty_edges() {
    std::deque<Edge> queue;
    for (int i = 0; i < graph.size(); ++i) {
        for (auto edge: graph[i]) {
            if (edge.symbol == 0) {
                queue.push_back({i, edge.final, edge.symbol});
            }
        }
    }

    while (!queue.empty()) {
        auto edge = queue.front();
        queue.pop_front();
        if (edge.start != edge.finish) {
            if (terminals.contains(edge.finish)) {
                terminals.insert(edge.start);
            }
            for (auto out_edge : graph[edge.finish]) {
                graph[edge.start].push_back(out_edge);
                if (out_edge.symbol == 0) {
                    queue.push_back({edge.start, out_edge.final, out_edge.symbol});
                }
            }
        }
        auto it = std::find(graph[edge.start].begin(),
                            graph[edge.start].end(),
                            DirectEdge(edge.finish, edge.symbol));
        graph[edge.start].erase(it);
    }
}

void Thompson::iterate() {
    delete_empty_edges();
    determinate();
    rename_vertices();
}
