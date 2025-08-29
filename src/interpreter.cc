#include "lusoscript/interpreter.hh"

#include <iostream>

#include "lusoscript/scanner.hh"

void Interpreter::process(ErrorState *error_state, std::string source) {
  Scanner scanner(std::move(source), error_state);
  std::vector<token::Token> tokens = scanner.scanTokens();

  for (token::Token token : tokens) {
    std::cout << token.toString() << std::endl;
  }
}
