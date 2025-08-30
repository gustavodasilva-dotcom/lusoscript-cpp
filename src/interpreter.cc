#include "lusoscript/interpreter.hh"

#include <iostream>

#include "lusoscript/arena.hh"
#include "lusoscript/lexer.hh"
#include "lusoscript/parser.hh"

void Interpreter::process(ErrorState *error_state, std::string source) {
  Lexer lexer(std::move(source), error_state);
  std::vector<token::Token> tokens = lexer.scanTokens();

  arena::Arena arena_allocator(1024);

  Parser parser(&arena_allocator, error_state, tokens);
}
