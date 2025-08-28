#ifndef LUSOSCRIPT_SCANNER_H
#define LUSOSCRIPT_SCANNER_H

#include <string>
#include <vector>

#include "error.hh"
#include "token.hh"

class Scanner {
public:
	explicit Scanner(std::string source, ErrorState *error_state);

	std::vector<token::Token> scanTokens();

private:
	std::string source_;
	ErrorState *error_state_;
	std::vector<token::Token> tokens_;
	int start_;
	int current_;
	int line_;

	bool isAtEnd();
	void scanToken();
	void scanString();
	void scanNumber();
	void scanIdentifier();
	bool isDigit(char c);
	bool isAlpha(char c);
	bool isAlphaNumeric(char c);
	char advance();
	char peek();
	char peekNext();
	bool match(char expected);
	void addToken(token::TokenType token_type);
	void addToken(token::TokenType token_type, std::string literal);
};

#endif
