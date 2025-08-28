#ifndef LUSOSCRIPT_TOKEN_H
#define LUSOSCRIPT_TOKEN_H

#include <optional>
#include <string>
#include <unordered_map>

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

	const std::string KW_E = "e";
	const std::string KW_CLASSE = "classe";
	const std::string KW_SENAO = "senao";
	const std::string KW_FALSO = "falso";
	const std::string KW_FUNCAO = "funcao";
	const std::string KW_PARA = "para";
	const std::string KW_SE = "nulo";
	const std::string KW_OU = "ou";
	const std::string KW_IMPRIMA = "imprima";
	const std::string KW_RETORNE = "retorne";
	const std::string KW_SUPER = "super";
	const std::string KW_ESSE = "esse";
	const std::string KW_VERDADEIRO = "verdadeiro";
	const std::string KW_VAR = "var";
	const std::string KW_ENQUANTO = "enquanto";

	const std::unordered_map<std::string, TokenType> Keywords = {
		{KW_E, TokenType::KW_E},
		{KW_CLASSE, TokenType::KW_CLASSE},
		{KW_SENAO, TokenType::KW_SENAO},
		{KW_FALSO, TokenType::KW_FALSO},
		{KW_FUNCAO, TokenType::KW_FUNCAO},
		{KW_PARA, TokenType::KW_PARA},
		{KW_SE, TokenType::KW_SE},
		{KW_OU, TokenType::KW_OU},
		{KW_IMPRIMA, TokenType::KW_IMPRIMA},
		{KW_RETORNE, TokenType::KW_RETORNE},
		{KW_SUPER, TokenType::KW_SUPER},
		{KW_ESSE, TokenType::KW_ESSE},
		{KW_VERDADEIRO, TokenType::KW_VERDADEIRO},
		{KW_VAR, TokenType::KW_VAR},
		{KW_ENQUANTO, TokenType::KW_ENQUANTO},
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
