#include <iostream>
#include "FSMTHompson.cpp"


int main() {
    uint64_t n, alphabet, terminals;
    int64_t e, a, b, c;
    std::cout << "Print number of vertices, edges, terminal vertices and symbols in alphabet:\n";
    std::cin >> n >> e >> terminals >> alphabet;
    FSM::FiniteStateMachine fsm(alphabet, n);
    std::cout << "For every edge print start finish and symbol on edge (one):\n";
    for (int i = 0; i < e; ++i) {
        std::cin >> a >> b >> c;
        fsm.add_edge(a, b, c);
    }
    std::cout << "Print terminal number's:\n";
    for (int i = 0; i < terminals; ++i) {
        std::cin >> a;
        fsm.add_terminal(a);
    }
    auto new_fsm = FSM::thompson_algorithm(fsm);
    new_fsm.print();
}

/*
Test:
4 10 2
0 0 1
0 0 2
1 1 1
1 1 2
2 2 1
2 2 2
0 1 1
0 2 2
1 3 1
2 3 2

Test:
7 10 1 2
0 1 0
0 3 0
1 2 1
2 1 1
3 4 2
4 3 2
3 5 0
1 5 0
5 6 1
6 5 2
5

*/
