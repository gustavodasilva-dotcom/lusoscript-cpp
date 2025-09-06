#include "lusoscript/repl.hh"

#include <iostream>

#include "lusoscript/driver.hh"
#include "lusoscript/state.hh"

void Repl::run() {
  Driver driver;

  std::string input;

  std::cout << "> ";

  state::AppState app_state{.mode = state::RunningMode::REPL,
                            .error = error::ErrorState{}};

  while (std::getline(std::cin, input)) {
    app_state.source = input;

    driver.process(&app_state);

    app_state.error.resetHadError();
    app_state.error.resetHadRuntimeError();

    std::cout << "> ";
  }
}
