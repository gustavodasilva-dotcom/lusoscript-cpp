/*
 * 09/02/2025 - The `comma()` function enforced the binary expression was
 * enclosed in parentheses. Grammar-wise, it was incorrect, but it had the
 * unintended consequence of making grouping expressions unparsed. To fix that,
 * `comma()` only parses the expression, leaving `primary()` to
 * check for parentheses.
 *
 * Nonetheless, there's still an issue. C produces different AST depending on
 * the parentheses. If there's none around a comma expression, the precedence of
 * the assignment operator is over the comma operator.
 *
 * When the time comes to parse statements, this distinction needs to be
 * addressed at parsetime (https://www.geeksforgeeks.org/c/comma-in-c/),
 * specially considering proper error handling.
 */

#include "lusoscript/parser.hh"

Parser::Parser(arena::Arena *allocator, error::ErrorState &error_state,
               std::vector<token::Token> tokens)
    : allocator_(allocator),
      error_state_(error_state),
      tokens_(std::move(tokens)),
      current_(0) {}

std::vector<ast::Stmt> Parser::parse() {
  std::vector<ast::Stmt> statements;

  while (!isAtEnd()) {
    statements.push_back(declaration());
  }

  return statements;
}

ast::Stmt Parser::declaration() {
  try {
    if (match({token::TokenType::KW_VAR})) return varDeclaration();

    return statement();
  } catch (error::ParserError) {
    const token::Token prev_token = previous();

    synchronize();

    return ast::Stmt{ast::ErrorStmt{prev_token}};
  }
}

ast::Stmt Parser::varDeclaration() {
  token::Token name =
      consume(token::TokenType::LT_IDENTIFIER, "Expected variable name.");

  auto var_decl = ast::Var{name};

  if (match({token::TokenType::MC_EQUAL})) {
    ast::Expr initializer = expression();

    auto init_ptr = allocator_->make_unique<ast::Expr>(std::move(initializer));
    var_decl.initializer = std::move(init_ptr);
  }

  consume(token::TokenType::SC_SEMICOLON,
          "Expected ';' after variable declaration.");

  return ast::Stmt{std::move(var_decl)};
}

ast::Stmt Parser::statement() {
  if (match({token::TokenType::KW_SE})) return ifStatement();
  if (match({token::TokenType::KW_IMPRIMA})) return imprimaStatement();
  if (match({token::TokenType::SC_OPEN_CURLY})) {
    std::vector<ast::Stmt> stmts = block();

    std::vector<ast::StmtPtr> stmt_ptrs;
    stmt_ptrs.reserve(stmts.size());

    for (auto &s : stmts) {
      stmt_ptrs.emplace_back(allocator_->make_unique<ast::Stmt>(std::move(s)));
    }

    return ast::Stmt{ast::Block{std::move(stmt_ptrs)}};
  }

  return expressionStatement();
}

ast::Stmt Parser::ifStatement() {
  consume(token::TokenType::SC_OPEN_PAREN, "Expected `(` after `se`.");

  ast::Expr condition = expression();

  consume(token::TokenType::SC_CLOSE_PAREN, "Expected `)` after condition.");

  const bool has_if_curly = match({token::TokenType::SC_OPEN_CURLY});

  ast::Stmt then_branch = statement();

  if (has_if_curly) {
    consume(token::TokenType::SC_CLOSE_CURLY, "Expected `}` after scope.");
  }

  auto cond_ptr = allocator_->make_unique<ast::Expr>(std::move(condition));
  auto then_ptr = allocator_->make_unique<ast::Stmt>(std::move(then_branch));

  auto if_stmt = ast::If{std::move(cond_ptr), std::move(then_ptr)};

  if (match({token::TokenType::KW_SENAO})) {
    const bool has_else_curly = match({token::TokenType::SC_OPEN_CURLY});

    ast::Stmt else_branch = statement();

    if (has_else_curly) {
      consume(token::TokenType::SC_CLOSE_CURLY, "Expected `}` after scope.");
    }

    if_stmt.else_branch =
        allocator_->make_unique<ast::Stmt>(std::move(else_branch));
  }

  return ast::Stmt{std::move(if_stmt)};
}

std::vector<ast::Stmt> Parser::block() {
  std::vector<ast::Stmt> statements;

  while (!check(token::TokenType::SC_CLOSE_CURLY) && !isAtEnd()) {
    statements.push_back(declaration());
  }

  consume(token::TokenType::SC_CLOSE_CURLY, "Expected '}' after block.");

  return statements;
}

ast::Stmt Parser::imprimaStatement() {
  consume(token::TokenType::SC_OPEN_PAREN, "Expected '(' before value.");

  ast::Expr value = expression();

  consume(token::TokenType::SC_CLOSE_PAREN, "Expected ')' after value.");
  consume(token::TokenType::SC_SEMICOLON,
          "Expected ';' after closing the parentheses.");

  auto value_ptr = allocator_->make_unique<ast::Expr>(std::move(value));
  return ast::Stmt{ast::Imprima{std::move(value_ptr)}};
}

ast::Stmt Parser::expressionStatement() {
  ast::Expr expr = expression();

  consume(token::TokenType::SC_SEMICOLON, "Expected ';' after expression.");

  auto expr_ptr = allocator_->make_unique<ast::Expr>(std::move(expr));
  return ast::Stmt{ast::Expression{std::move(expr_ptr)}};
}

ast::Expr Parser::expression() { return comma(); }

ast::Expr Parser::comma() {
  ast::Expr left_expr;

  // In case the expression starts with the binary comma operator...
  if (match({token::TokenType::SC_COMMA})) {
    error_state_.error(previous(),
                       "Binary operator ',' has no left-hand side.");

    // Parses the right-hand side and discards it by not assigning it.
    ast::Expr right_expr = assignment();

    // Creates a placeholder for the invalid left-hand side expression (the spot
    // before the dangling comma) and passes the right-hand side to it (metadata
    // for later use).
    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));
    left_expr = ast::Expr{ast::ErrorExpr{std::move(right)}};
  } else {
    // Parse the assignment (descending) as usual.
    left_expr = assignment();
  }

  while (match({token::TokenType::SC_COMMA})) {
    const token::Token opr = previous();
    ast::Expr right_expr = assignment();

    auto left = allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Binary{std::move(left), opr, std::move(right)}};
  }

  return left_expr;
}

ast::Expr Parser::assignment() {
  ast::Expr expr = ternary();

  if (match({token::TokenType::MC_EQUAL})) {
    const token::Token equals = previous();
    ast::Expr value = assignment();

    if (std::holds_alternative<ast::Variable>(expr.var)) {
      const auto &var = std::get<ast::Variable>(expr.var);

      auto value_ptr = allocator_->make_unique<ast::Expr>(std::move(value));
      return ast::Expr{ast::Assign{var.name, std::move(value_ptr)}};
    }

    error(equals, "Invalid assignment target.");
  }

  return expr;
}

ast::Expr Parser::ternary() {
  ast::Expr condition = logicalOr();

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

ast::Expr Parser::logicalOr() {
  ast::Expr left_expr = logicalAnd();

  while (match({token::TokenType::KW_OU})) {
    const token::Token opr = previous();
    ast::Expr right_expr = logicalAnd();

    auto left = allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Logical{std::move(left), opr, std::move(right)}};
  }

  return left_expr;
}

ast::Expr Parser::logicalAnd() {
  ast::Expr left_expr = equality();

  while (match({token::TokenType::KW_E})) {
    const token::Token opr = previous();
    ast::Expr right_expr = equality();

    auto left = allocator_->make_unique<ast::Expr>(std::move(left_expr));
    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));

    left_expr = ast::Expr{ast::Logical{std::move(left), opr, std::move(right)}};
  }

  return left_expr;
}

ast::Expr Parser::equality() {
  const std::vector<token::TokenType> operators = {
      token::TokenType::MC_EXCL_EQUAL, token::TokenType::MC_EQUAL_EQUAL};

  ast::Expr left_expr;

  // In case the expression starts with one of the binary equality operators...
  if (match(operators)) {
    const token::Token prev_token = previous();
    error_state_.error(prev_token, "Binary operator '" +
                                       token::toString(prev_token.type) +
                                       "' has no left-hand side.");

    ast::Expr right_expr = comparison();

    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));
    left_expr = ast::Expr{ast::ErrorExpr{std::move(right)}};
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
    error_state_.error(prev_token, "Binary operator '" +
                                       token::toString(prev_token.type) +
                                       "' has no left-hand side.");

    ast::Expr right_expr = term();

    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));
    left_expr = ast::Expr{ast::ErrorExpr{{std::move(right)}}};
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
    error_state_.error(previous(),
                       "Binary operator '+' has no left-hand side.");

    ast::Expr right_expr = factor();

    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));
    left_expr = ast::Expr{ast::ErrorExpr{{std::move(right)}}};
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
    error_state_.error(prev_token, "Binary operator '" +
                                       token::toString(prev_token.type) +
                                       "' has no left-hand side.");

    ast::Expr right_expr = unary();

    auto right = allocator_->make_unique<ast::Expr>(std::move(right_expr));
    left_expr = ast::Expr{ast::ErrorExpr{std::move(right)}};
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
    return ast::Expr{ast::Literal{previous().type, false}};
  }

  if (match({token::TokenType::KW_VERDADEIRO})) {
    return ast::Expr{ast::Literal{previous().type, true}};
  }

  if (match({token::TokenType::KW_NULO})) {
    return ast::Expr{ast::Literal{previous().type, nullptr}};
  }

  if (match({token::TokenType::LT_NUMBER, token::TokenType::LT_STRING})) {
    return ast::Expr{ast::Literal{previous().type, previous().literal}};
  }

  if (match({token::TokenType::LT_IDENTIFIER})) {
    return ast::Expr{ast::Variable{previous()}};
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
  error_state_.error(token, message);
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
