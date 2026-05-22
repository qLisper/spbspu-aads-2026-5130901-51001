#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <sstream>
#include <stdexcept>
#include "stack.hpp"
#include "queue.hpp"

namespace expr {

inline int precedence(const std::string & op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/" || op == "%") return 2;
    return 0;
}

inline bool isOperator(const std::string & token) {
    return token == "+" || token == "-" || token == "*" || token == "/" || token == "%";
}

inline bool isNumber(const std::string & token) {
    if (token.empty()) return false;
    size_t i = 0;
    if (token[0] == '-') i = 1; // допускаем отрицательные числа
    if (i == token.size()) return false;
    for (; i < token.size(); ++i) {
        if (!std::isdigit(token[i])) return false;
    }
    return true;
}

inline Queue< std::string > infixToPostfix(const std::string & expression) {
    std::istringstream iss(expression);
    std::string token;
    Queue< std::string > output;
    Stack< std::string > operators;

    while (iss >> token) {
        if (isNumber(token)) {
            output.push(token);
        } else if (token == "(") {
            operators.push(token);
        } else if (token == ")") {
            while (!operators.empty() && operators.top() != "(") {
                output.push(operators.top());
                operators.pop();
            }
            if (operators.empty()) {
                throw std::runtime_error("Mismatched parentheses");
            }
            operators.pop(); // удалить '('
        } else if (isOperator(token)) {
            while (!operators.empty() && operators.top() != "(" &&
                   precedence(operators.top()) >= precedence(token)) {
                output.push(operators.top());
                operators.pop();
            }
            operators.push(token);
        } else {
            throw std::runtime_error("Invalid token: " + token);
        }
    }

    while (!operators.empty()) {
        if (operators.top() == "(" || operators.top() == ")") {
            throw std::runtime_error("Mismatched parentheses");
        }
        output.push(operators.top());
        operators.pop();
    }

    return output;
}

} // namespace expr

#endif
