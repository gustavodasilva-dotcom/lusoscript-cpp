#include "lusoscript/parser.hh"

#include <cassert>
#include <stdexcept>

Parser::Parser(arena::Arena *arena_allocator, ErrorState *error_state,
               std::vector<token::Token> tokens)
    : arena_allocator_(arena_allocator),
      error_state_(error_state),
      tokens_(std::move(tokens)),
      current_(0) {}

ast::Expr Parser::expression() { return equality(); }

ast::Expr Parser::equality() {
  ast::Expr left_expr = comparison();

  while (match(
      {token::TokenType::MC_EXCL_EQUAL, token::TokenType::MC_EQUAL_EQUAL})) {
    token::Token opr = previous();
    ast::Expr right_expr = comparison();

    auto left = arena_allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right =
        arena_allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Binary{std::move(left), opr, std::move(right)}};
  }

  return left_expr;
}

ast::Expr Parser::comparison() {
  ast::Expr left_expr = term();

  while (
      match({token::TokenType::MC_GREATER, token::TokenType::MC_GREATER_EQUAL,
             token::TokenType::MC_LESS, token::TokenType::MC_LESS_EQUAL})) {
    token::Token opr = previous();
    ast::Expr right_expr = term();

    auto left = arena_allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right =
        arena_allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Binary{std::move(left), opr, std::move(right)}};
  }

  return left_expr;
}

ast::Expr Parser::term() {
  ast::Expr left_expr = factor();

  while (match({token::TokenType::SC_MINUS, token::TokenType::SC_PLUS})) {
    token::Token opr = previous();
    ast::Expr right_expr = factor();

    auto left = arena_allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right =
        arena_allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Binary{std::move(left), opr, std::move(right)}};
  }

  return left_expr;
}

ast::Expr Parser::factor() {
  ast::Expr left_expr = unary();

  while (
      match({token::TokenType::SC_FORWARD_SLASH, token::TokenType::SC_STAR})) {
    token::Token opr = previous();
    ast::Expr right_expr = unary();

    auto left = arena_allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right =
        arena_allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Binary{std::move(left), opr, std::move(right)}};
  }

  return left_expr;
}

ast::Expr Parser::unary() {
  if (match({token::TokenType::MC_EXCL, token::TokenType::SC_MINUS})) {
    token::Token opr = previous();
    ast::Expr right_operand = unary();

    auto right =
        arena_allocator_->make_unique<ast::Expr>(std::move(right_operand));

    return ast::Expr{ast::Unary{opr, std::move(right)}};
  }

  return primary();
}

ast::Expr Parser::primary() {
  if (match({token::TokenType::KW_FALSO})) {
    return ast::Expr{
        ast::Literal{.type = ast::LiteralType::BOOLEAN, .boolean = false}};
  }

  if (match({token::TokenType::KW_VERDADEIRO})) {
    return ast::Expr{
        ast::Literal{.type = ast::LiteralType::BOOLEAN, .boolean = true}};
  }

  if (match({token::TokenType::KW_NULO})) {
    return ast::Expr{ast::Literal{ast::LiteralType::NULO}};
  }

  if (match({token::TokenType::LT_NUMBER, token::TokenType::LT_STRING})) {
    return ast::Expr{ast::Literal{.type = ast::LiteralType::NUMBER_STRING,
                                  .str_num = previous().literal}};
  }

  if (match({token::TokenType::SC_OPEN_PAREN})) {
    ast::Expr group_expr = expression();

    consume(token::TokenType::SC_CLOSE_PAREN);

    auto grouping =
        arena_allocator_->make_unique<ast::Expr>(std::move(group_expr));

    return ast::Expr{ast::Grouping{std::move(grouping)}};
  }

  // TODO: create a customized error handler for syntax errors.
  throw std::runtime_error("Invalid primary expression.");
}

bool Parser::match(std::vector<token::TokenType> types) {
  for (token::TokenType type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }

  return false;
}

token::Token Parser::consume(token::TokenType type) {
  if (check(type)) return advance();

  // TODO: create a customized error handler for syntax errors.
  throw std::runtime_error("Expected token not found.");
}

bool Parser::check(token::TokenType type) {
  if (isAtEnd()) return false;
  return peek().type == type;
}

token::Token Parser::advance() {
  if (!isAtEnd()) current_++;
  return previous();
}

bool Parser::isAtEnd() { return peek().type == token::TokenType::END_OF_FILE; }

token::Token Parser::peek() { return tokens_.at(current_); }

token::Token Parser::previous() { return tokens_.at(current_ - 1); }
