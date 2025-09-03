#ifndef LUSOSCRIPT_AST_H
#define LUSOSCRIPT_AST_H

#include <memory>
#include <variant>
#include <vector>

#include "arena.hh"
#include "token.hh"

namespace ast {
struct Expr;
struct Stmt;

using ExprPtr = std::unique_ptr<Expr, arena::NoopDeleter<Expr>>;
using StmtPtr = std::unique_ptr<Stmt, arena::NoopDeleter<Stmt>>;

struct Assign {
  token::Token name;
  ExprPtr value;
};

struct Ternary {
  ExprPtr condition;
  token::Token then_opr;
  ExprPtr then_expr;
  token::Token else_opr;
  ExprPtr else_expr;
};

struct Binary {
  ExprPtr left;
  token::Token opr;
  ExprPtr right;
};

struct Grouping {
  ExprPtr expression;
};

struct Literal {
  token::TokenType token_type;
  std::any value;
};

struct Unary {
  token::Token opr;
  ExprPtr right;
};

struct Variable {
  token::Token name;
};

struct ErrorExpr {
  ExprPtr expr;
};

struct Expr {
  std::variant<Assign, Ternary, Binary, Grouping, Literal, Unary, Variable,
               ErrorExpr>
      var;
};

struct Block {
  std::vector<StmtPtr> stmts;
};

struct Expression {
  ExprPtr expression;
};

struct Imprima {
  ExprPtr expression;
};

struct Var {
  token::Token name;
  std::optional<ExprPtr> initializer;
};

struct ErrorStmt {
  token::Token token;
};

struct Stmt {
  std::variant<Block, Expression, Imprima, Var, ErrorStmt> var;
};

class AstPrinter {
 public:
  std::string print(const Expr &expression);

 private:
  std::string output_;
};
}  // namespace ast

#endif
