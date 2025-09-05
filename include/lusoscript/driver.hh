#ifndef LUSOSCRIPT_DRIVER_H
#define LUSOSCRIPT_DRIVER_H

#include <string>

#include "error.hh"

class Driver {
 public:
  void process(error::ErrorState *error_state, std::string source,
               const bool &is_repl_input);
};

#endif
