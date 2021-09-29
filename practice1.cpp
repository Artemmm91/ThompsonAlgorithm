#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

const int32_t INF = INT32_MAX;

std::set<int> all_sums(std::set<int>& s, int k) { // make set of sums every element in set
    std::set<int> new_s = s, buffer;
    new_s.insert(0);
    for (int i = 0; i <= k; ++i) {
        buffer.clear();
        if (s.contains(i)) {
            for (auto element: new_s) {
                if (element + i <= k) {
                    buffer.insert(element + i);
                }
            }
            for (auto element: buffer) {
                new_s.insert(element);
            }
        }
    }
    return new_s;
}

template<char symbol, int32_t k>
struct Algorithm {
private:
    struct Dynamics {
        std::vector<int32_t> ans; // ans[i] - min length of word with suffix x^i
        std::set<int32_t> X_words; // what length of words of type x^l, 0 <= l <= k
        explicit Dynamics(char c) : ans(k + 1, INF) {
            ans[0] = 1;
            if (c != symbol) {
                return;
            }
            ans[1] = 1;
            X_words.insert(1);
        }

        Dynamics() : ans(k + 1, INF) {}
    };

    Dynamics sum(Dynamics &&first, Dynamics &&second) { // operation '+'
        Dynamics new_word;
        for (int32_t i = 0; i <= k; ++i) {
            new_word.ans[i] = std::min(first.ans[i], second.ans[i]);
        }
        for (auto X: first.X_words) {
            new_word.X_words.insert(X);
        }
        for (auto X: second.X_words) {
            new_word.X_words.insert(X);
        }
        return new_word;
    }

    Dynamics concatenate(Dynamics &&first, Dynamics &&second) { //operation '.'
        Dynamics new_word;
        int32_t min_word_first = first.ans[0];
        for (int32_t i = 0; i <= k; ++i) { // I. get suffix from second, and minimal word from first
            if (min_word_first < INF && second.ans[i] < INF)
                new_word.ans[i] = min_word_first + second.ans[i];
        }
        for (int32_t i = 0; i <= k; ++i) { // II. second word - x^t, first - has suffix x^(k-t), t=x_word
            for (auto x_word: second.X_words) {
                if (x_word <= i && first.ans[i - x_word] < INF) {
                    new_word.ans[i] = std::min(new_word.ans[i], first.ans[i - x_word] + x_word);
                }
            }
        }
        for (auto x_first: first.X_words) { // updating every variant of sum
            for (auto x_second: second.X_words) {
                if (x_first + x_second <= k) {
                    new_word.X_words.insert(x_first + x_second);
                }
            }
        }
        return new_word;
    }

    Dynamics clini(Dynamics &&last) {
        Dynamics new_word;
        new_word.X_words = all_sums(last.X_words, k); // all variants of suffixes
        new_word.ans[0] = 0; // empty word
        for (auto x_word: new_word.X_words) { // suffix x^t contains at word x^x_word, x_word >= t
            for (int i = 0; i <= x_word; ++i) {
                new_word.ans[i] = std::min(new_word.ans[i], x_word);
            }
        }
        for(int i = 0; i <= k; ++i) { // star contains one word
            new_word.ans[i] = std::min(new_word.ans[i], last.ans[i]);
        }
        for (int32_t i = 0; i <= k; ++i) { // updating ans[i]
            for(int32_t j = 0; j <= i; ++j) { // choosing suffix x^j
                if (new_word.X_words.contains(j) && new_word.ans[i - j] < INF) {
                    new_word.ans[i] = std::min(new_word.ans[i], j + new_word.ans[i - j]);
                }
            }
        }
        return new_word;
    }
public:
    int32_t solve(const std::string &s) {
        std::vector<Dynamics> stack;
        for (char c: s) {
            if (c >= 'a' && c <= 'c') {
                stack.emplace_back(c);
                continue;
            }
            if (c == '+' || c == '.') {
                if (stack.size() < 2) return -1;
                Dynamics second = std::move(stack[stack.size() - 1]);
                Dynamics first = std::move(stack[stack.size() - 2]);
                stack.pop_back();
                stack.pop_back();
                if (c == '+')
                    stack.push_back(sum(std::move(first), std::move(second)));
                else
                    stack.push_back(concatenate(std::move(first), std::move(second)));
                continue;
            }
            if (c == '*') {
                if (stack.empty()) return -1;
                Dynamics last = std::move(stack[stack.size() - 1]);
                stack.pop_back();
                stack.push_back(clini(std::move(last)));
                continue;
            }
            return -1;
        }
        if (stack.empty() || stack.size() > 1) return -1;
        Dynamics answer = stack.back();
        if (answer.ans[k] == INF) return INF;
        return answer.ans[k];
    }
};


template<char symbol, int32_t k>
void test(std::string&& problem, int answer) {
    Algorithm<symbol, k> alg;
    assert(answer == alg.solve(problem));
}


int main() {
    test<'c', 2>(std::string("acc.."), 3);
    test<'a', 2>(std::string("acb..bab.c.*.ab.ba.+.+*a."), 4);
    test<'b', 2>(std::string("ab+c.aba.*.bac.+.+*"), INF);
    test<'a', 2>(std::string("...*+"), -1);
    test<'a', 2>(std::string("abc"), -1);
    test<'c', 2>(std::string("acc..ac.+"), 3);
    test<'c', 1>(std::string("acc..ac.+"), 2);
    test<'c', 2>(std::string("ac.ccc..+*"), 3);
    test<'c', 5>(std::string("ac.ccc..+*"), 6);
    test<'c', 10>(std::string("cccc...*"), 12);
    std::cout << "OK. Tests passed";
}
