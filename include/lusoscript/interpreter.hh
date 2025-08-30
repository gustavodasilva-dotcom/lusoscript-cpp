#ifndef LUSOSCRIPT_INTERPRETER_H
#define LUSOSCRIPT_INTERPRETER_H

#include <string>

#include "error.hh"

class Interpreter {
 public:
  void process(error::ErrorState *error_state, std::string source);
};

#endif
