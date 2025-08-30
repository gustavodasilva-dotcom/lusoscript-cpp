#ifndef LUSOSCRIPT_ERROR_H
#define LUSOSCRIPT_ERROR_H

#include <stdexcept>
#include <string>

#include "token.hh"

namespace error {
class ErrorState {
 public:
  explicit ErrorState();

  void error(int line, std::string message);
  void error(token::Token token, std::string message);
  bool getHadError();
  void resetHadError();
  void summary();

 private:
  bool had_error_;
  int error_count_;
  int warning_count_;

  void report(int line, std::string where, std::string message);
  void setHadError();
};

class ParserError : public std::runtime_error {
 public:
  explicit ParserError(const std::string& message);

  const char* what() const noexcept override;

 private:
  std::string message_;
};
};  // namespace error

#endif
