#ifndef LUSOSCRIPT_DRIVER_H
#define LUSOSCRIPT_DRIVER_H

#include "state.hh"

class Driver {
 public:
  void process(state::AppState *app_state);
};

#endif
