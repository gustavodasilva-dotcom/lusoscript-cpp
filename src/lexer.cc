#include "lusoscript/lexer.hh"

Lexer::Lexer(std::string source, error::ErrorState *error_state)
    : source_(std::move(source)),
      error_state_(error_state),
      start_(0),
      current_(0),
      line_(1) {}

std::vector<token::Token> Lexer::scanTokens() {
  while (!isAtEnd()) {
    start_ = current_;
    scanToken();
  }

  tokens_.push_back(
      {.type = token::TokenType::END_OF_FILE, .lexeme = "", .line = line_});

  return tokens_;
}

bool Lexer::isAtEnd() { return current_ >= source_.length(); }

void Lexer::scanToken() {
  char c = advance();

  switch (c) {
    case '(':
      addToken(token::TokenType::SC_OPEN_PAREN);
      break;
    case ')':
      addToken(token::TokenType::SC_CLOSE_PAREN);
      break;
    case '{':
      addToken(token::TokenType::SC_OPEN_CURLY);
      break;
    case '}':
      addToken(token::TokenType::SC_CLOSE_CURLY);
      break;
    case ',':
      addToken(token::TokenType::SC_COMMA);
      break;
    case '.':
      addToken(token::TokenType::SC_DOT);
      break;
    case '-':
      addToken(token::TokenType::SC_MINUS);
      break;
    case '+':
      addToken(token::TokenType::SC_PLUS);
      break;
    case ':':
      addToken(token::TokenType::SC_COLON);
      break;
    case ';':
      addToken(token::TokenType::SC_SEMICOLON);
      break;
    case '*':
      addToken(token::TokenType::SC_STAR);
      break;
    case '?':
      addToken(token::TokenType::MC_QUESTION);
      break;
    case '!':
      addToken(match('=') ? token::TokenType::MC_EXCL_EQUAL
                          : token::TokenType::MC_EXCL);
      break;
    case '=':
      addToken(match('=') ? token::TokenType::MC_EQUAL_EQUAL
                          : token::TokenType::MC_EQUAL);
      break;
    case '<':
      addToken(match('=') ? token::TokenType::MC_LESS_EQUAL
                          : token::TokenType::MC_LESS);
      break;
    case '>':
      addToken(match('=') ? token::TokenType::MC_GREATER_EQUAL
                          : token::TokenType::MC_GREATER);
      break;
    case '/':
      if (match('/')) {
        while (peek() != '\n' && !isAtEnd()) advance();
      } else if (match('*')) {
        scanMultilineComment();
      } else {
        addToken(token::TokenType::SC_FORWARD_SLASH);
      }
      break;
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n':
      line_++;
      break;
    case '"':
      scanString();
      break;
    default:
      if (isDigit(c)) {
        scanNumber();
      } else if (isAlpha(c)) {
        scanIdentifier();
      } else {
        error_state_->error(line_, "Unexpected character.");
      }
      break;
  }
}

void Lexer::scanString() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') line_++;

    advance();
  }

  if (isAtEnd()) {
    error_state_->error(line_, "Unterminated string.");
    return;
  }

  // Consume the remaining closing double quote ('"').
  advance();

  // Extract the string literal value without the enclosing double quotes.
  std::string value = source_.substr(start_ + 1, (current_ - 1) - (start_ + 1));
  addToken(token::TokenType::LT_STRING, value);
}

void Lexer::scanMultilineComment() {
  while (peek() != '*' || peekNext() != '/') {
    if (isAtEnd()) {
      error_state_->error(line_, "Unterminated multiline comment.");
      return;
    }

    if (peek() == '\n') line_++;

    char c = advance();

    // Recursion for nested multiline comments.
    if (c == '/' && match('*')) scanMultilineComment();
  }

  // If it isn't an unterminated comment, the remaining multiline characters
  // ('*' and '/') are consumed.
  advance();
  advance();
}

void Lexer::scanNumber() {
  while (isDigit(peek())) advance();

  // Checking if the current character is a decimal separator.
  if (peek() == '.' && isDigit(peekNext())) {
    // If so, Consume the decimal separator.
    advance();

    // And scan the fractional part.
    while (isDigit(peek())) advance();
  }

  std::string value = source_.substr(start_, current_ - start_);
  addToken(token::TokenType::LT_NUMBER, std::stof(value));
}

void Lexer::scanIdentifier() {
  // Using the "maximal-munch" principle (consuming as many characters as
  // possible).
  while (isAlphaNumeric(peek())) advance();

  std::string text = source_.substr(start_, current_ - start_);

  // If the text extracted does not correspond to a keyword, treat it as a user
  // identifier.
  const auto it = token::Keywords.find(text);
  if (it != token::Keywords.end()) {
    addToken(it->second);
  } else {
    addToken(text == token::KW_NULO ? token::TokenType::KW_NULO
                                    : token::TokenType::LT_IDENTIFIER,
             nullptr);
  }
}

bool Lexer::isDigit(char c) { return c >= '0' && c <= '9'; }

bool Lexer::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

char Lexer::advance() { return source_.at(current_++); }

char Lexer::peek() {
  if (isAtEnd()) return '\0';

  return source_.at(current_);
}

char Lexer::peekNext() {
  if (current_ + 1 >= source_.length()) return '\0';

  return source_.at(current_ + 1);
}

bool Lexer::match(char expected) {
  if (isAtEnd()) return false;

  if (source_.at(current_) != expected) return false;

  current_++;

  return true;
}

void Lexer::addToken(token::TokenType token_type) {
  tokens_.push_back({.type = token_type, .line = line_});
}

void Lexer::addToken(token::TokenType token_type, std::any literal) {
  std::string lexeme = source_.substr(start_, current_ - start_);
  tokens_.push_back({token_type, lexeme, literal, line_});
}
