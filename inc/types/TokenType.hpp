#ifndef TOKENTYPE_HPP
#define TOKENTYPE_HPP
#include <string>
struct Token {
	enum TokenType {
		SERVER,
		LOCATION,
		DIRECTIVE,
		LBRACE,
		STRING,
		RBRACE,
		SEMICOLON,
		QUOTE,
		ERROR,
		SINGLE_QUOTE,
		NUMBER,
		END,
		INVALID
	};
	TokenType					type;
	std::string					value;
	Token(): type(INVALID) {};
	Token(TokenType type, std::string value): type(type), value(value) {};
};
#endif