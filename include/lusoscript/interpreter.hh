#ifndef LUSOSCRIPT_INTERPRETER_H
#define LUSOSCRIPT_INTERPRETER_H

#include <any>
#include <vector>

#include "ast.hh"
#include "environment.hh"
#include "error.hh"

class Interpreter {
 public:
  explicit Interpreter(error::ErrorState *error_state);

  void interpret(const std::vector<ast::Stmt> &stmts);

 private:
  error::ErrorState *error_state_;
  env::Environment current_env_;

  void execute(const ast::Stmt &stmt);
  void executeBlock(const ast::Block &block, const env::Environment &env);
  std::any evaluate(const ast::Expr &expr);
  bool isTruthy(std::any value);
  bool isEqual(std::any a, std::any b);
  void checkNumberOperand(token::Token opr, std::any value);
  void checkNumberOperands(token::Token opr, std::any left, std::any right);
  std::any combineStrict(const token::Token &opr, const std::any &left,
                         const std::any &right);
  std::any combineLoose(const token::Token &opr, const std::any &left,
                        const std::any &right);
  std::string stringify(const std::any &value);
};

#endif
