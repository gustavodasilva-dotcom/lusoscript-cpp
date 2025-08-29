#include "lusoscript/repl.hh"

#include <iostream>

#include "lusoscript/error.hh"
#include "lusoscript/interpreter.hh"

void Repl::run() {
  Interpreter interpreter;

  std::string input;

  std::cout << "> ";

  ErrorState error_state;

  while (std::getline(std::cin, input)) {
    interpreter.process(&error_state, std::move(input));

    error_state.resetHadError();

    std::cout << "> ";
  }
}
