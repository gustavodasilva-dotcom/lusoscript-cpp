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
	char advance();
	void addToken(token::TokenType token_type);
	void addToken(token::TokenType token_type, std::string literal);
	bool match(char expected);
};

#endif
