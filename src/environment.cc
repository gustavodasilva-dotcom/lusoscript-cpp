#include "lusoscript/environment.hh"

#include "lusoscript/error.hh"

env::Environment::Environment() : enclosing_(nullptr), values_({}) {}

env::Environment::Environment(env::Environment *enclosing)
    : enclosing_(enclosing), values_({}) {}

std::any env::Environment::get(const token::Token &token) {
  const auto &identifier = token.lexeme.value();

  const auto it = values_.find(identifier);
  if (it != values_.end()) return it->second;

  if (enclosing_ != nullptr) return enclosing_->get(token);

  throw error::RuntimeError(token, "Undefined variable '" + identifier + "'");
}

void env::Environment::define(const std::string &name, const std::any &value) {
  values_[name] = value;
}

void env::Environment::assign(const token::Token &token,
                              const std::any &value) {
  const auto &identifier = token.lexeme.value();

  const auto it = values_.find(identifier);
  if (it != values_.end()) {
    values_[identifier] = value;
    return;
  }

  if (enclosing_ != nullptr) {
    enclosing_->assign(token, value);
    return;
  }

  throw error::RuntimeError(token, "Undefined variable '" + identifier + "'");
}
