#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "expression.hpp"

int main(int argc, char* argv[])
{
    std::istream* in = &std::cin;
    std::ifstream file;
    if (argc == 2) {
        file.open(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Error: cannot open file " << argv[1] << '\n';
            return 1;
        }
        in = &file;
    } else if (argc > 2) {
        std::cerr << "Usage: ./lab [filename]\n";
        return 1;
    }

    Stack<int> results;
    std::string line;
    while (std::getline(*in, line)) {
        if (line.empty()) continue;
        try {
            auto postfix = expr::infixToPostfix(line);
            int result = expr::evaluatePostfix(postfix);
            results.push(result);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return 1;
        }
    }

    // Вывод результатов в обратном порядке (стек уже содержит в обратном порядке)
    bool first = true;
    while (!results.empty()) {
        if (!first) std::cout << ' ';
        std::cout << results.top();
        results.pop();
        first = false;
    }
    std::cout << '\n';
    return 0;
}
