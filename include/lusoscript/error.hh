#ifndef LUSOSCRIPT_ERROR_H
#define LUSOSCRIPT_ERROR_H

#include <stdexcept>
#include <string>

#include "token.hh"

namespace error {
class RuntimeError;

class ErrorState {
 public:
  explicit ErrorState();

  void error(int line, std::string message);
  void error(token::Token token, std::string message);
  void runtimeError(const RuntimeError &error);
  [[nodiscard]] bool getHadError();
  [[nodiscard]] bool getHadRuntimeError();
  void resetHadError();
  void resetHadRuntimeError();
  void summary();

 private:
  bool had_error_;
  bool had_runtime_error_;
  int error_count_;
  int warning_count_;

  void report(int line, std::string where, std::string message);
  void setHadError();
};

class ParserError : public std::runtime_error {
 public:
  explicit ParserError(const std::string &message);

  const char *what() const noexcept override;

 private:
  std::string message_;
};

class RuntimeError : public std::runtime_error {
 public:
  explicit RuntimeError(const token::Token &token, const std::string &message);

  const char *what() const noexcept override;
  [[nodiscard]] token::Token getToken() const;

 private:
  std::string message_;
  const token::Token &token_;
};
};  // namespace error

#endif
