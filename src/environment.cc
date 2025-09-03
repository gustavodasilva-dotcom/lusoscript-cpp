#include "lusoscript/environment.hh"

#include "lusoscript/error.hh"

std::any Environment::get(const token::Token &token) {
  const auto &identifier = token.lexeme;

  const auto it = values_.find(identifier.value());
  if (it != values_.end()) return it->second;

  throw error::RuntimeError(token,
                            "Undefined variable '" + identifier.value() + "'");
}

void Environment::define(const std::string &name, const std::any &value) {
  values_[name] = value;
}

void Environment::assign(const token::Token &token, const std::any &value) {
  const auto &identifier = token.lexeme;

  const auto it = values_.find(identifier.value());
  if (it != values_.end()) {
    values_[identifier.value()] = value;
    return;
  }

  throw error::RuntimeError(token,
                            "Undefined variable '" + identifier.value() + "'");
}
