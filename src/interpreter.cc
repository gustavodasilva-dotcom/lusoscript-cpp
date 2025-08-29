#include "lusoscript/interpreter.hh"

#include <iostream>

#include "lusoscript/lexer.hh"

void Interpreter::process(ErrorState *error_state, std::string source) {
  Lexer lexer(std::move(source), error_state);
  std::vector<token::Token> tokens = lexer.scanTokens();

  for (token::Token token : tokens) {
    std::cout << token.toString() << std::endl;
  }
}
