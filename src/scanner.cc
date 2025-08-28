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
		start_ = current_;
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
	case '/':
		if (match('/')) {
			while (peek() != '\n' && !isAtEnd())
				advance();
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

void Scanner::scanString() {
	while (peek() != '"' && !isAtEnd()) {
		if (peek() == '\n')
			line_++;

		advance();
	}

	if (isAtEnd()) {
		error_state_->error(line_, "Unterminated string.");
		return;
	}

	// Extract the string literal value without the enclosing double quotes.
	std::string value = source_.substr(start_ + 1, current_ - start_ - 1);
	addToken(token::TokenType::LT_STRING, value);

	// Consume the remaining closing double quote ('"').
	advance();
}

void Scanner::scanNumber() {
	while (isDigit(peek()))
		advance();

	// Checking if the current character is a decimal separator.
	if (peek() == '.' && isDigit(peekNext())) {
		// If so, Consume the decimal separator.
		advance();

		// And scan the fractional part.
		while (isDigit(peek()))
			advance();
	}

	std::string value = source_.substr(start_, current_ - start_);
	addToken(token::TokenType::LT_NUMBER, value);
}

void Scanner::scanIdentifier() {
	// Using the "maximal-munch" principle (consuming as many characters as possible).
	while (isAlphaNumeric(peek()))
		advance();

	std::string text = source_.substr(start_, current_ - start_);

	token::TokenType token_type;

	// If the text extracted does not correspond to a keyword, treat it as a user identifier.
	const auto it = token::Keywords.find(text);
	if (it != token::Keywords.end()) {
		token_type = it->second;
	} else {
		token_type = token::TokenType::LT_IDENTIFIER;
	}

	addToken(token_type);
}

bool Scanner::isDigit(char c) {
	return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isAlphaNumeric(char c) {
	return isAlpha(c) || isDigit(c);
}

char Scanner::advance() {
	return source_.at(current_++);
}

char Scanner::peek() {
	if (isAtEnd())
		return '\0';

	return source_.at(current_);
}

char Scanner::peekNext() {
	if (current_ + 1 >= source_.length())
		return '\0';

	return source_.at(current_ + 1);
}

bool Scanner::match(char expected) {
	if (isAtEnd())
		return false;

	if (source_.at(current_) != expected)
		return false;

	current_++;

	return true;
}

void Scanner::addToken(token::TokenType token_type) {
	tokens_.push_back({.type = token_type});
}

void Scanner::addToken(token::TokenType token_type, std::string literal) {
	std::string lexeme = source_.substr(start_, current_ - start_);
	tokens_.push_back({token_type, lexeme, literal, line_});
}
