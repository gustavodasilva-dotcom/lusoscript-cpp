#include "lusoscript/driver.hh"

#include <iostream>

#include "lusoscript/arena.hh"
#include "lusoscript/interpreter.hh"
#include "lusoscript/lexer.hh"
#include "lusoscript/parser.hh"

void Driver::process(state::AppState *app_state) {
  Lexer lexer(app_state->source, app_state->error);
  std::vector<token::Token> tokens = lexer.scanTokens();

  // Allocates 4 MB of memory for the arena.
  arena::Arena allocator(1024 * 1024 * 4);

  Parser parser(&allocator, app_state->error, tokens);
  const auto statements = parser.parse();

  if (!app_state->error.getHadError()) {
    Interpreter interpreter{app_state->error, app_state->mode};
    interpreter.interpret(statements);
  }
}
