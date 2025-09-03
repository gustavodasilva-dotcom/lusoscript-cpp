#include "lusoscript/environment.hh"

#include "lusoscript/error.hh"

std::any Environment::get(const token::Token &token) {
  const auto &identifier = token.lexeme;

  const auto it = values_.find(identifier.value());
  if (it != values_.end()) return it->second;

  throw error::RuntimeError(token,
                            "Undefined variable '" + identifier.value() + "'");
}

void Environment::define(std::string name, std::any value) {
  values_[name] = value;
}
