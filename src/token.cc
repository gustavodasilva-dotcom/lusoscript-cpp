#include "lusoscript/token.hh"

#include <iostream>

#include "cassert"

namespace token {
std::string toString(TokenType token_type) {
  switch (token_type) {
    case TokenType::KW_E:
      return KW_E;
    case TokenType::KW_CLASSE:
      return KW_CLASSE;
    case TokenType::KW_SENAO:
      return KW_SENAO;
    case TokenType::KW_FALSO:
      return KW_FALSO;
    case TokenType::KW_FUNCAO:
      return KW_FUNCAO;
    case TokenType::KW_PARA:
      return KW_PARA;
    case TokenType::KW_SE:
      return KW_SE;
    case TokenType::KW_NULO:
      return KW_NULO;
    case TokenType::KW_OU:
      return KW_OU;
    case TokenType::KW_IMPRIMA:
      return KW_IMPRIMA;
    case TokenType::KW_RETORNE:
      return KW_RETORNE;
    case TokenType::KW_SUPER:
      return KW_SUPER;
    case TokenType::KW_ESSE:
      return KW_ESSE;
    case TokenType::KW_VERDADEIRO:
      return KW_VERDADEIRO;
    case TokenType::KW_VAR:
      return KW_VAR;
    case TokenType::KW_ENQUANTO:
      return KW_ENQUANTO;
    case TokenType::SC_OPEN_PAREN:
      return SC_OPEN_PAREN;
    case TokenType::SC_CLOSE_PAREN:
      return SC_CLOSE_PAREN;
    case TokenType::SC_OPEN_CURLY:
      return SC_OPEN_CURLY;
    case TokenType::SC_CLOSE_CURLY:
      return SC_CLOSE_CURLY;
    case TokenType::SC_COMMA:
      return SC_COMMA;
    case TokenType::SC_DOT:
      return SC_COMMA;
    case TokenType::SC_MINUS:
      return SC_MINUS;
    case TokenType::SC_PLUS:
      return SC_PLUS;
    case TokenType::SC_COLON:
      return SC_COLON;
    case TokenType::SC_SEMICOLON:
      return SC_SEMICOLON;
    case TokenType::SC_FORWARD_SLASH:
      return SC_FORWARD_SLASH;
    case TokenType::SC_STAR:
      return SC_STAR;
    case TokenType::MC_QUESTION:
      return MC_QUESTION;
    case TokenType::MC_EXCL:
      return MC_EXCL;
    case TokenType::MC_EXCL_EQUAL:
      return MC_EXCL_EQUAL;
    case TokenType::MC_EQUAL:
      return MC_EQUAL;
    case TokenType::MC_EQUAL_EQUAL:
      return MC_EQUAL_EQUAL;
    case TokenType::MC_GREATER:
      return MC_GREATER;
    case TokenType::MC_GREATER_EQUAL:
      return MC_GREATER_EQUAL;
    case TokenType::MC_LESS:
      return MC_LESS;
    case TokenType::MC_LESS_EQUAL:
      return MC_LESS_EQUAL;
    case TokenType::LT_IDENTIFIER:
      return LT_IDENTIFIER;
    case TokenType::LT_STRING:
      return LT_STRING;
    case TokenType::LT_NUMBER:
      return LT_NUMBER;
    case TokenType::END_OF_FILE:
      return END_OF_FILE;
    default:
      assert(0 && "Unmapped token type.");
  }
}

std::string Token::toString() {
  std::string output;

  if (lexeme.has_value() && !lexeme.value().empty()) {
    output.append("[" + token::toString(type) + ":" + lexeme.value() + "]");
  } else {
    output.append("[" + token::toString(type) + "]");
  }

  // The default statement is omitted to indicate that it is not a literal if it
  // does not fall into any of the clauses.
  switch (type) {
    case token::TokenType::LT_NUMBER:
      output.append(
          " (literal:" + std::to_string(std::any_cast<float>(literal)) + ")");
      break;
    case token::TokenType::LT_STRING:
      output.append(" (literal:" + std::any_cast<std::string>(literal) + ")");
      break;
    case token::TokenType::KW_VERDADEIRO:
      output.append(" (literal:" + token::KW_VERDADEIRO + ")");
      break;
    case token::TokenType::KW_FALSO:
      output.append(" (literal:" + token::KW_FALSO + ")");
      break;
  };

  output.append(" (line " + std::to_string(line) + ")");

  return output;
}
};  // namespace token
