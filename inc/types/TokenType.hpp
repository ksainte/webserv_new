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
		END
	};
	TokenType					type;
	std::string					value;
};
#endif