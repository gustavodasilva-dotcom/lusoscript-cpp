#ifndef LUSOSCRIPT_STATE_H
#define LUSOSCRIPT_STATE_H

#include "error.hh"

namespace state {
enum class RunningMode { REPL, SourceFile };

struct AppState {
  RunningMode mode;
  std::string source;
  error::ErrorState error;
};
};  // namespace state

#endif
