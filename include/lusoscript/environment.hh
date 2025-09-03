#ifndef LUSOSCRIPT_ENVIRONMENT_H
#define LUSOSCRIPT_ENVIRONMENT_H

#include <any>
#include <string>
#include <unordered_map>

#include "lusoscript/token.hh"

namespace env {
struct Uninitialized {};

class Environment {
 public:
  explicit Environment();
  explicit Environment(Environment *enclosing);

  std::any get(const token::Token &token);
  void define(const std::string &name, const std::any &value);
  void assign(const token::Token &token, const std::any &value);

 private:
  Environment *enclosing_;
  std::unordered_map<std::string, std::any> values_;
};
}  // namespace env

#endif
