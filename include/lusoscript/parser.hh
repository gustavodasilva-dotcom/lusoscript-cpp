#ifndef LUSOSCRIPT_PARSER_H
#define LUSOSCRIPT_PARSER_H

#include "arena.hh"
#include "ast.hh"
#include "error.hh"
#include "token.hh"

class Parser {
 public:
  explicit Parser(arena::Arena *allocator, error::ErrorState &error_state,
                  std::vector<token::Token> tokens);

  std::vector<ast::Stmt> parse();

 private:
  ast::Stmt declaration();
  ast::Stmt varDeclaration();
  ast::Stmt statement();
  ast::Stmt ifStatement();
  std::vector<ast::Stmt> block();
  ast::Stmt imprimaStatement();
  ast::Stmt whileStatement();
  ast::Stmt expressionStatement();
  ast::Expr expression();
  ast::Expr comma();
  ast::Expr assignment();
  ast::Expr ternary();
  ast::Expr logicalOr();
  ast::Expr logicalAnd();
  ast::Expr equality();
  ast::Expr comparison();
  ast::Expr term();
  ast::Expr factor();
  ast::Expr unary();
  ast::Expr primary();
  bool match(std::vector<token::TokenType> types);
  token::Token consume(token::TokenType type, std::string message);
  bool check(token::TokenType type);
  token::Token advance();
  bool isAtEnd();
  token::Token peek();
  token::Token previous();
  error::ParserError error(token::Token token, std::string message);
  void synchronize();

  arena::Arena *allocator_;
  error::ErrorState &error_state_;
  const std::vector<token::Token> tokens_;
  int current_;
};

#endif
