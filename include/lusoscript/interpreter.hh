#ifndef LUSOSCRIPT_INTERPRETER_H
#define LUSOSCRIPT_INTERPRETER_H

#include "ast.hh"
#include "error.hh"

class Interpreter {
 public:
  explicit Interpreter(error::ErrorState *error_state);

  void interpret(const ast::Expr &root);

 private:
  error::ErrorState *error_state_;

  std::any evaluate(const ast::Expr &expr);
  std::string stringify(const std::any &value);
  bool isTruthy(std::any value);
  bool isEqual(std::any a, std::any b);
  void checkNumberOperand(token::Token opr, std::any value);
  void checkNumberOperands(token::Token opr, std::any left, std::any right);
  std::any combineStrict(const token::Token &opr, const std::any &left,
                         const std::any &right);
  std::any combineLoose(const token::Token &opr, const std::any &left,
                        const std::any &right);
};

#endif
