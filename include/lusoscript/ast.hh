#ifndef LUSOSCRIPT_AST_H
#define LUSOSCRIPT_AST_H

#include <memory>
#include <variant>

#include "arena.hh"
#include "token.hh"

namespace ast {
enum class LiteralType { NUMBER_STRING, BOOLEAN, NULO };

struct Expr;
using ExprPtr = std::unique_ptr<Expr, arena::NoopDeleter<Expr>>;

struct Binary {
  ExprPtr left;
  token::Token opr;
  ExprPtr right;
};

struct Grouping {
  ExprPtr expression;
};

struct Literal {
  LiteralType type;
  std::optional<bool> boolean;
  std::optional<std::string> str_num;
};

struct Unary {
  token::Token opr;
  ExprPtr right;
};

struct Expr {
  std::variant<Binary, Grouping, Literal, Unary> var;
};

class AstPrinter {
 public:
  std::string print(const Expr &expression);

 private:
  std::string output_;
};
}  // namespace ast

#endif
