#include "lusoscript/repl.hh"

#include <iostream>

#include "lusoscript/driver.hh"
#include "lusoscript/error.hh"

void Repl::run() {
  Driver driver;

  std::string input;

  std::cout << "> ";

  error::ErrorState error_state;

  while (std::getline(std::cin, input)) {
    driver.process(&error_state, std::move(input));

    error_state.resetHadError();
    error_state.resetHadRuntimeError();

    std::cout << "> ";
  }
}
