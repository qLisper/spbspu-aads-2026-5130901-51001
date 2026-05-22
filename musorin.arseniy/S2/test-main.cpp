#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include "expression.hpp"
#include "stack.hpp"
#include "queue.hpp"

namespace
{

void testEvaluate(const std::string& expr, int expected)
{
  auto postfix = expr::infixToPostfix(expr);
  int result = expr::evaluatePostfix(postfix);
  if (result != expected)
  {
    std::cerr << "FAIL: " << expr << " = " << result
              << " (expected " << expected << ")\n";
    std::exit(1);
  }
}

void testError(const std::string& expr)
{
  try
  {
    auto postfix = expr::infixToPostfix(expr);
    expr::evaluatePostfix(postfix);
    std::cerr << "FAIL: expected error for " << expr << "\n";
    std::exit(1);
  }
  catch (const std::exception&)
  {

  }
}

void testStackQueue()
{
  Stack<int> s;
  s.push(10);
  s.push(20);
  assert(s.top() == 20);
  s.pop();
  assert(s.top() == 10);
  s.pop();
  assert(s.empty());

  Queue<int> q;
  q.push(1);
  q.push(2);
  assert(q.front() == 1);
  q.pop();
  assert(q.front() == 2);
  q.pop();
  assert(q.empty());

  std::cout << "Stack and Queue basic tests passed\n";
}

void runAllTests()
{
  testEvaluate("1 + 2", 3);
  testEvaluate("( 1 + 2 ) * ( 3 - 4 )", -3);
  testEvaluate("1 + 3", 4);
  testEvaluate("4 * 7 - 3", 25);
  testEvaluate("10 / 3 % 2", 1);
  testEvaluate("( 10 / ( 2 + 3 ) % 4 )", 2);

  testError("1 +");
  testError("+ 1");
  testError("1 / 0");
  testError("1 % 0");
  testError("( 1 + 2");
  testError("1 + 2 )");
  testError("1 + a");

  testStackQueue();

  std::cout << "All tests passed\n";
}

}

int main()
{
  runAllTests();
  return 0;
}
