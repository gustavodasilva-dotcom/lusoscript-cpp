#ifndef LUSOSCRIPT_ENVIRONMENT_H
#define LUSOSCRIPT_ENVIRONMENT_H

#include <any>
#include <string>
#include <unordered_map>

#include "lusoscript/token.hh"

class Environment {
 public:
  std::any get(const token::Token &token);
  void define(std::string name, std::any value);

 private:
  std::unordered_map<std::string, std::any> values_;
};

#endif
