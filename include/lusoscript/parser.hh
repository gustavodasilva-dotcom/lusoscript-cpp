#ifndef LUSOSCRIPT_PARSER_H
#define LUSOSCRIPT_PARSER_H

#include <vector>

#include "arena.hh"
#include "ast.hh"
#include "error.hh"
#include "token.hh"

class Parser {
 public:
  explicit Parser(arena::Arena *arena_allocator, ErrorState *error_state,
                  std::vector<token::Token> tokens);

 private:
  ast::Expr expression();
  ast::Expr equality();
  ast::Expr comparison();
  ast::Expr term();
  ast::Expr factor();
  ast::Expr unary();
  ast::Expr primary();
  bool match(std::vector<token::TokenType> types);
  token::Token consume(token::TokenType type);
  bool check(token::TokenType type);
  token::Token advance();
  bool isAtEnd();
  token::Token peek();
  token::Token previous();

  arena::Arena *arena_allocator_;
  ErrorState *error_state_;
  const std::vector<token::Token> tokens_;
  int current_;
};

#endif
