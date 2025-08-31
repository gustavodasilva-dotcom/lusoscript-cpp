#include "lusoscript/interpreter.hh"

#include <iostream>

#include "lusoscript/arena.hh"
#include "lusoscript/lexer.hh"
#include "lusoscript/parser.hh"

void Interpreter::process(error::ErrorState *error_state, std::string source) {
  Lexer lexer(std::move(source), error_state);
  std::vector<token::Token> tokens = lexer.scanTokens();

  // Allocates 4 MB of memory for the arena.
  arena::Arena allocator(1024 * 1024 * 4);

  Parser parser(&allocator, error_state, tokens);
  std::optional<ast::Expr> expression = parser.parse();

  // As opposed to simply returning in the event of an error, verify whether the
  // parser result contains an expression. If so, allow the code to continue
  // printing the resultant tree (even if it is invalid).
  if (error_state->getHadError() && !expression.has_value()) return;

  ast::AstPrinter printer;
  std::cout << printer.print(std::move(expression.value())) << std::endl;
}
