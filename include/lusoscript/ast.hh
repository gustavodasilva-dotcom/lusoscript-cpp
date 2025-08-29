#ifndef LUSOSCRIPT_AST_H
#define LUSOSCRIPT_AST_H

#include <memory>
#include <variant>

#include "token.hh"

namespace ast {
struct Expr;
using ExprPtr = std::unique_ptr<Expr>;

struct Binary {
  ExprPtr left;
  token::Token opr;
  ExprPtr right;
};

struct Grouping {
  ExprPtr expression;
};

struct Literal {
  std::optional<std::string> value;
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
