#include "lusoscript/error.hh"

#include <iostream>

error::ErrorState::ErrorState()
    : had_error_(false),
      had_runtime_error_(false),
      error_count_(0),
      warning_count_(0) {}

void error::ErrorState::error(int line, std::string message) {
  report(line, "", message);
  setHadError();
}

void error::ErrorState::error(token::Token token, std::string message) {
  if (token.type == token::TokenType::END_OF_FILE) {
    report(token.line, " at end", message);
  } else {
    std::string where =
        token.lexeme.has_value() ? " at '" + token.lexeme.value() + "'" : "";
    report(token.line, where, message);
  }

  setHadError();
}

void error::ErrorState::runtimeError(const error::RuntimeError &error) {
  std::cerr << "RuntimeError: " << error.what() << "\n\t on line "
            << error.getToken().line << "." << std::endl;
  had_runtime_error_ = true;
}

bool error::ErrorState::getHadError() { return had_error_; }

bool error::ErrorState::getHadRuntimeError() { return had_runtime_error_; };

void error::ErrorState::resetHadError() { had_error_ = false; }

void error::ErrorState::summary() {
  std::cout << std::string(48, '-') << std::endl;
  std::cout << "Errors: " << error_count_ << std::endl;
  std::cout << "Warnings: " << warning_count_ << std::endl;
}

void error::ErrorState::report(int line, std::string where,
                               std::string message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
}

void error::ErrorState::setHadError() {
  error_count_++;
  if (!had_error_) had_error_ = true;
}

error::ParserError::ParserError(const std::string &message)
    : std::runtime_error(message), message_(message) {}

const char *error::ParserError::what() const noexcept {
  return message_.c_str();
}

error::RuntimeError::RuntimeError(const token::Token &token,
                                  const std::string &message)
    : std::runtime_error(message), message_(message), token_(token) {}

const char *error::RuntimeError::what() const noexcept {
  return message_.c_str();
}

token::Token error::RuntimeError::getToken() const { return token_; }
