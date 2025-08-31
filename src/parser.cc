#include "lusoscript/parser.hh"

Parser::Parser(arena::Arena *allocator, error::ErrorState *error_state,
               std::vector<token::Token> tokens)
    : allocator_(allocator),
      error_state_(error_state),
      tokens_(std::move(tokens)),
      current_(0) {}

std::optional<ast::Expr> Parser::parse() {
  try {
    return expression();
  } catch (error::ParserError) {
    return std::nullopt;
  }
}

ast::Expr Parser::expression() { return comma(); }

ast::Expr Parser::comma() {
  const bool has_open_paren = match({token::TokenType::SC_OPEN_PAREN});

  ast::Expr left_expr;

  // In case the expression starts with the binary comma operator...
  if (match({token::TokenType::SC_COMMA})) {
    error_state_->error(previous(),
                        "Binary operator ',' has no left-hand side.");

    // Parses the right-hand side and discards it by not assigning it.
    ast::Expr right_expr = ternary();

    // Creates a placeholder for the invalid left-hand side expression (the spot
    // before the dangling comma) and passes the right-hand side to it (metadata
    // for later use).
    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));
    left_expr = ast::Expr{ast::Error{std::move(right)}};
  } else {
    // Parse the ternary (descending) as usual.
    left_expr = ternary();
  }

  while (match({token::TokenType::SC_COMMA})) {
    if (!has_open_paren)
      throw error(previous(), "Expected '(' before expression.");

    const token::Token opr = previous();
    ast::Expr right_expr = ternary();

    auto left = allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Binary{std::move(left), opr, std::move(right)}};
  }

  if (has_open_paren) {
    consume(token::TokenType::SC_CLOSE_PAREN, "Expected ')' after expression.");
  }

  return left_expr;
}

ast::Expr Parser::ternary() {
  ast::Expr condition = equality();

  if (match({token::TokenType::MC_QUESTION})) {
    const auto question = previous();
    ast::Expr then_expr = expression();

    const auto colon =
        consume(token::TokenType::SC_COLON, "Expected ':' after expression.");

    ast::Expr else_expr = ternary();

    auto cond_ptr = allocator_->make_unique<ast::Expr>(std::move(condition));
    auto then_ptr = allocator_->make_unique<ast::Expr>(std::move(then_expr));
    auto else_ptr = allocator_->make_unique<ast::Expr>(std::move(else_expr));

    condition = ast::Expr{ast::Ternary{std::move(cond_ptr), question,
                                       std::move(then_ptr), colon,
                                       std::move(else_ptr)}};
  }

  return condition;
}

ast::Expr Parser::equality() {
  const std::vector<token::TokenType> operators = {
      token::TokenType::MC_EXCL_EQUAL, token::TokenType::MC_EQUAL_EQUAL};

  ast::Expr left_expr;

  // In case the expression starts with one of the binary equality operators...
  if (match(operators)) {
    const token::Token prev_token = previous();
    error_state_->error(prev_token, "Binary operator '" +
                                        token::toString(prev_token.type) +
                                        "' has no left-hand side.");

    ast::Expr right_expr = comparison();

    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));
    left_expr = ast::Expr{ast::Error{std::move(right)}};
  } else {
    left_expr = comparison();
  }

  while (match(operators)) {
    const token::Token opr = previous();
    ast::Expr right_expr = comparison();

    auto left = allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Binary{std::move(left), opr, std::move(right)}};
  }

  return left_expr;
}

ast::Expr Parser::comparison() {
  const std::vector<token::TokenType> operators = {
      token::TokenType::MC_GREATER, token::TokenType::MC_GREATER_EQUAL,
      token::TokenType::MC_LESS, token::TokenType::MC_LESS_EQUAL};

  ast::Expr left_expr;

  // In case the expression starts with one of the binary comparison
  // operators...
  if (match(operators)) {
    const token::Token prev_token = previous();
    error_state_->error(prev_token, "Binary operator '" +
                                        token::toString(prev_token.type) +
                                        "' has no left-hand side.");

    ast::Expr right_expr = term();

    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));
    left_expr = ast::Expr{ast::Error{{std::move(right)}}};
  } else {
    left_expr = term();
  }

  while (match(operators)) {
    const token::Token opr = previous();
    ast::Expr right_expr = term();

    auto left = allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Binary{std::move(left), opr, std::move(right)}};
  }

  return left_expr;
}

ast::Expr Parser::term() {
  ast::Expr left_expr;

  // Only checks the binary arithmetic addition operator, because the
  // subtraction operator is parsed as a unary expression at the highest
  // level.
  if (match({token::TokenType::SC_PLUS})) {
    error_state_->error(previous(),
                        "Binary operator '+' has no left-hand side.");

    ast::Expr right_expr = factor();

    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));
    left_expr = ast::Expr{ast::Error{{std::move(right)}}};
  } else {
    left_expr = factor();
  }

  while (match({token::TokenType::SC_MINUS, token::TokenType::SC_PLUS})) {
    const token::Token opr = previous();
    ast::Expr right_expr = factor();

    auto left = allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Binary{std::move(left), opr, std::move(right)}};
  }

  return left_expr;
}

ast::Expr Parser::factor() {
  const std::vector<token::TokenType> operators = {
      token::TokenType::SC_FORWARD_SLASH, token::TokenType::SC_STAR};

  ast::Expr left_expr;

  // In case the expression starts with the binary arithmetic division or
  // multiplication operators...
  if (match(operators)) {
    const token::Token prev_token = previous();
    error_state_->error(prev_token, "Binary operator '" +
                                        token::toString(prev_token.type) +
                                        "' has no left-hand side.");

    ast::Expr right_expr = unary();

    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));
    left_expr = ast::Expr{ast::Error{std::move(right)}};
  } else {
    left_expr = unary();
  }

  while (match(operators)) {
    const token::Token opr = previous();
    ast::Expr right_expr = unary();

    auto left = allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Binary{std::move(left), opr, std::move(right)}};
  }

  return left_expr;
}

ast::Expr Parser::unary() {
  if (match({token::TokenType::MC_EXCL, token::TokenType::SC_MINUS})) {
    const token::Token opr = previous();
    ast::Expr right_operand = unary();

    auto right = allocator_->make_unique<ast::Expr>(std::move(right_operand));

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

    consume(token::TokenType::SC_CLOSE_PAREN, "Expected ')' after expression.");

    auto grouping = allocator_->make_unique<ast::Expr>(std::move(group_expr));

    return ast::Expr{ast::Grouping{std::move(grouping)}};
  }

  throw error(peek(), "Expect expression.");
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

token::Token Parser::consume(token::TokenType type, std::string message) {
  if (check(type)) return advance();
  throw error(peek(), message);
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

error::ParserError Parser::error(token::Token token, std::string message) {
  error_state_->error(token, message);
  return error::ParserError(message);
}

void Parser::synchronize() {
  advance();

  while (!isAtEnd()) {
    if (previous().type == token::TokenType::SC_SEMICOLON) return;

    switch (peek().type) {
      case token::TokenType::KW_CLASSE:
      case token::TokenType::KW_FUNCAO:
      case token::TokenType::KW_VAR:
      case token::TokenType::KW_PARA:
      case token::TokenType::KW_SE:
      case token::TokenType::KW_ENQUANTO:
      case token::TokenType::KW_IMPRIMA:
      case token::TokenType::KW_RETORNE:
        return;
    }

    advance();
  }
}
