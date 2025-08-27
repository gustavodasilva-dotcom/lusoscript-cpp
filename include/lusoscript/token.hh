#ifndef LUSOSCRIPT_TOKEN_H
#define LUSOSCRIPT_TOKEN_H

#include <optional>
#include <string>

namespace token {
	enum class TokenType {
		// Keywords
		KW_E, KW_CLASSE, KW_SENAO, KW_FALSO, KW_FUNCAO, KW_PARA, KW_SE, KW_NULO, KW_OU,
		KW_IMPRIMA, KW_RETORNE, KW_SUPER, KW_ESSE, KW_VERDADEIRO, KW_VAR, KW_ENQUANTO,

		// Single-character tokens
		SC_OPEN_PAREN, SC_CLOSE_PAREN, SC_OPEN_CURLY, SC_CLOSE_CURLY,
		SC_COMMA, SC_DOT, SC_MINUS, SC_PLUS, SC_SEMICOLON, SC_FORWARD_SLASH, SC_STAR,

		// One or two character tokens
		MC_EXCL, MC_EXCL_EQUAL,
		MC_EQUAL, MC_EQUAL_EQUAL,
		MC_GREATER, MC_GREATER_EQUAL,
		MC_LESS, MC_LESS_EQUAL,

		// Literals
		LT_IDENTIFIER, LT_STRING, LT_NUMBER,

		END_OF_FILE,
	};

	class Token {
	public:
		TokenType type;
		std::optional<std::string> lexeme;
		std::optional<std::string> literal;
		int line;
	};
}

#endif
