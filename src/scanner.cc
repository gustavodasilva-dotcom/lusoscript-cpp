#include "lusoscript/scanner.hh"

Scanner::Scanner(std::string source, ErrorState *error_state) :
	source_(std::move(source)),
	error_state_(error_state),
	start_(0),
	current_(0),
	line_(1)
{}

std::vector<token::Token> Scanner::scanTokens() {
	while (!isAtEnd()) {
		scanToken();
	}

	tokens_.push_back({
		.type = token::TokenType::END_OF_FILE,
		.lexeme = "",
		.line = line_
	});

	return tokens_;
}

bool Scanner::isAtEnd() {
	return current_ >= source_.length();
}

void Scanner::scanToken() {
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
	case ';':
		addToken(token::TokenType::SC_SEMICOLON);
		break;
	case '/':
		addToken(token::TokenType::SC_FORWARD_SLASH);
		break;
	case '*':
		addToken(token::TokenType::SC_STAR);
		break;
	case '!':
		addToken(match('=') ? token::TokenType::MC_EXCL_EQUAL : token::TokenType::MC_EXCL);
		break;
	case '=':
		addToken(match('=') ? token::TokenType::MC_EQUAL_EQUAL : token::TokenType::MC_EQUAL);
		break;
	case '<':
		addToken(match('=') ? token::TokenType::MC_LESS_EQUAL : token::TokenType::MC_LESS);
		break;
	case '>':
		addToken(match('=') ? token::TokenType::MC_GREATER_EQUAL : token::TokenType::MC_GREATER);
		break;
	default:
		error_state_->error(line_, "Unexpected character.");
		break;
	}
}

char Scanner::advance() {
	return source_.at(current_++);
}

void Scanner::addToken(token::TokenType token_type) {
	tokens_.push_back({.type = token_type});
}

void Scanner::addToken(token::TokenType token_type, std::string literal) {
	std::string lexeme = source_.substr(start_, current_);
	tokens_.push_back({token_type, lexeme, literal, line_});
}

bool Scanner::match(char expected) {
	if (isAtEnd())
		return false;

	if (source_.at(current_) != expected)
		return false;

	current_++;

	return true;
}
