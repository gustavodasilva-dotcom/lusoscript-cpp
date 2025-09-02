#ifndef LUSOSCRIPT_INTERPRETER_H
#define LUSOSCRIPT_INTERPRETER_H

#include <any>
#include <vector>

#include "ast.hh"
#include "error.hh"

class Interpreter {
 public:
  explicit Interpreter(error::ErrorState *error_state);

  void interpret(const std::vector<ast::Stmt> &stmts);

 private:
  error::ErrorState *error_state_;

  void execute(const ast::Stmt &stmt);
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
  std::string castAnyFloatToStringAndFormat(std::any value);
  std::string castAnyBooleanToStringAndFormat(std::any value);
};

#endif
