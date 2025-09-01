#ifndef LUSOSCRIPT_INTERPRETER_H
#define LUSOSCRIPT_INTERPRETER_H

#include "ast.hh"

class Interpreter {
 public:
  void interpret(const ast::Expr &root);

 private:
  std::any evaluate(const ast::Expr &expr);
  bool isTruthy(std::any value);
  bool isEqual(std::any a, std::any b);
};

#endif
