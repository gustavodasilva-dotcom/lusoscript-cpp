#include "lusoscript/token.hh"

#include "cassert"

std::string toString(token::TokenType token_type) {
  switch (token_type) {
    case token::TokenType::KW_E:
      return token::KW_E;
    case token::TokenType::KW_CLASSE:
      return token::KW_CLASSE;
    case token::TokenType::KW_SENAO:
      return token::KW_SENAO;
    case token::TokenType::KW_FALSO:
      return token::KW_FALSO;
    case token::TokenType::KW_FUNCAO:
      return token::KW_FUNCAO;
    case token::TokenType::KW_PARA:
      return token::KW_PARA;
    case token::TokenType::KW_SE:
      return token::KW_SE;
    case token::TokenType::KW_NULO:
      return token::KW_NULO;
    case token::TokenType::KW_OU:
      return token::KW_OU;
    case token::TokenType::KW_IMPRIMA:
      return token::KW_IMPRIMA;
    case token::TokenType::KW_RETORNE:
      return token::KW_RETORNE;
    case token::TokenType::KW_SUPER:
      return token::KW_SUPER;
    case token::TokenType::KW_ESSE:
      return token::KW_ESSE;
    case token::TokenType::KW_VERDADEIRO:
      return token::KW_VERDADEIRO;
    case token::TokenType::KW_VAR:
      return token::KW_VAR;
    case token::TokenType::KW_ENQUANTO:
      return token::KW_ENQUANTO;
    case token::TokenType::SC_OPEN_PAREN:
      return token::SC_OPEN_PAREN;
    case token::TokenType::SC_CLOSE_PAREN:
      return token::SC_CLOSE_PAREN;
    case token::TokenType::SC_OPEN_CURLY:
      return token::SC_OPEN_CURLY;
    case token::TokenType::SC_CLOSE_CURLY:
      return token::SC_CLOSE_CURLY;
    case token::TokenType::SC_COMMA:
      return token::SC_COMMA;
    case token::TokenType::SC_DOT:
      return token::SC_COMMA;
    case token::TokenType::SC_MINUS:
      return token::SC_MINUS;
    case token::TokenType::SC_PLUS:
      return token::SC_PLUS;
    case token::TokenType::SC_SEMICOLON:
      return token::SC_SEMICOLON;
    case token::TokenType::SC_FORWARD_SLASH:
      return token::SC_FORWARD_SLASH;
    case token::TokenType::SC_STAR:
      return token::SC_STAR;
    case token::TokenType::MC_EXCL:
      return token::MC_EXCL;
    case token::TokenType::MC_EXCL_EQUAL:
      return token::MC_EXCL_EQUAL;
    case token::TokenType::MC_EQUAL:
      return token::MC_EQUAL;
    case token::TokenType::MC_EQUAL_EQUAL:
      return token::MC_EQUAL_EQUAL;
    case token::TokenType::MC_GREATER:
      return token::MC_GREATER;
    case token::TokenType::MC_GREATER_EQUAL:
      return token::MC_GREATER_EQUAL;
    case token::TokenType::MC_LESS:
      return token::MC_LESS;
    case token::TokenType::MC_LESS_EQUAL:
      return token::MC_LESS_EQUAL;
    case token::TokenType::LT_IDENTIFIER:
      return token::LT_IDENTIFIER;
    case token::TokenType::LT_STRING:
      return token::LT_STRING;
    case token::TokenType::LT_NUMBER:
      return token::LT_NUMBER;
    case token::TokenType::END_OF_FILE:
      return token::END_OF_FILE;
    default:
      assert(0 && "Unmapped token type.");
  }
}

std::string token::Token::toString() {
  std::string result = "Token type: " + ::toString(type);

  if (lexeme.has_value()) {
    result += " / lexeme: " + lexeme.value();
  }

  if (literal.has_value()) {
    result += " / literal: " + literal.value();
  }

  result += " / line: " + std::to_string(line);

  return result;
}
