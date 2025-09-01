#include "lusoscript/driver.hh"

#include <iostream>

#include "lusoscript/arena.hh"
#include "lusoscript/interpreter.hh"
#include "lusoscript/lexer.hh"
#include "lusoscript/parser.hh"

void Driver::process(error::ErrorState *error_state, std::string source) {
  Lexer lexer(std::move(source), error_state);
  std::vector<token::Token> tokens = lexer.scanTokens();

  // Allocates 4 MB of memory for the arena.
  arena::Arena allocator(1024 * 1024 * 4);

  Parser parser(&allocator, error_state, tokens);
  const auto expression = parser.parse();

  if (!error_state->getHadError()) {
    Interpreter interpreter{error_state};
    interpreter.interpret(std::move(expression));
  }
}
