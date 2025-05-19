#include "../inc/RandomTokenList.hpp"
#include <sstream>
#include <ctime>
#include <iomanip>
#include <cstdlib>

const std::string RandomTokenList::TESTCHAR =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";

RandomTokenList::RandomTokenList()
{
	srand(static_cast<unsigned int>(time(0)));
}

RandomTokenList::~RandomTokenList() {}

const std::vector<Token>& RandomTokenList::getTokenList() const {
	return _tokenList;
}

void RandomTokenList::generateServerBlock() {
	
	// 1. Add SERVER Token
	Token serverToken;
	serverToken.type = Token::SERVER;
	_tokenList.push_back(serverToken);

	//2. Add LBRACE Token
	Token lbraceToken1;
	lbraceToken1.type = Token::LBRACE;
	lbraceToken1.value = "{";
	_tokenList.push_back(lbraceToken1);

    // 3. Add random number of Directives inside server block (0 to MAX_DIRECTIVES_IN_BLOCK)
	int numDir = rand() % (MAX_DIRECTIVES_PER_BLOCK + 1);
	for (int i = 0; i < numDir; ++i) {
		try {
			generateDirective();
		}
		catch (std::exception& e){
			throw;
		}
	}

	//4. Add LOCATION Token
	Token locationToken;
	locationToken.type = Token::LOCATION;
	locationToken.value = "/" + randStr(LOCATION_PATH_MAX_LEN);
	_tokenList.push_back(locationToken);

	//5. Add LBRACE Token
	Token lbraceToken2;
	lbraceToken2.type = Token::LBRACE;
	lbraceToken2.value = "{";
	_tokenList.push_back(lbraceToken2);
	
    // 6. Add random number of Directives inside locaiton block (0 to MAX_DIRECTIVES_IN_BLOCK)
	numDir = rand() % (MAX_DIRECTIVES_PER_BLOCK + 1);
	for (int i = 0; i < numDir; ++i) {
		try {
			generateDirective();
		}
		catch (std::exception& e) {
			throw;
		}
	}

	//7. Add RBRACE Token
	Token rbraceToken1;
	rbraceToken1.type = Token::RBRACE;
	rbraceToken1.value = "}";
	_tokenList.push_back(rbraceToken1);

	//8. Add RBRACE Token
	Token rbraceToken2;
	rbraceToken2.type = Token::RBRACE;
	rbraceToken2.value = "}";
	_tokenList.push_back(rbraceToken2);
}

void RandomTokenList::generateDirective() {

	static const std::string directives[] = {
		"listen",
		"server_name",
		"index",
		"root",
		"error_page",
		"return",
		"method"
	};

	static const size_t numDirectives = sizeof(directives) / sizeof(directives[0]);

	Token directiveToken;
	directiveToken.type = Token::DIRECTIVE;
	directiveToken.value = directives[rand() % numDirectives];
	_tokenList.push_back(directiveToken);

	int numStrings = (rand() % MAX_STR_SEQ) + 1;

	for (int i = 0; i < numStrings; i++) {
		Token stringToken;
		stringToken.type = Token::STRING;
		stringToken.value = randStr((rand() % MAX_LEN) + 1);
		_tokenList.push_back(stringToken);
	}

	Token semicolonToken;
	semicolonToken.type = Token::SEMICOLON;
	semicolonToken.value = ";";
	_tokenList.push_back(semicolonToken);

}

std::string RandomTokenList::randStr(size_t length) const {
	std::string randStr;
	for (size_t i = 0; i < length; ++i) {
		randStr += TESTCHAR[rand() % TESTCHAR.size()];
	}
	return randStr;
}

/**
 * @brief Converts a Token::TokenType enum to its string representation.
 * @param type The token type enum value.
 * @return A string representing the token type (e.g., "SERVER", "STRING").
 * @note Ensure ALL possible Token::TokenType values have a case here.
 */
 std::string RandomTokenList::tokenTypeToString(Token::TokenType type) const {
    switch (type) {
        case Token::STRING:    return "STRING";
        case Token::DIRECTIVE: return "DIRECTIVE";
        case Token::SEMICOLON: return "SEMICOLON";
        case Token::SERVER:    return "SERVER";
        case Token::LOCATION:  return "LOCATION";
        case Token::LBRACE:    return "LBRACE";
        case Token::RBRACE:    return "RBRACE";
        default:           return "UNKNOWN"; // Fallback for unhandled types
    }
}

/**
 * @brief Escapes characters in a string for JSON compatibility (C++98 compliant).
 * @param input The raw string.
 * @return The string with JSON special characters escaped.
 */
 std::string RandomTokenList::escapeJsonString(const std::string& input) const {
    std::ostringstream oss;
    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        unsigned char c = static_cast<unsigned char>(*it);

        switch (c) {
            case '"':  oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b";  break;
            case '\f': oss << "\\f";  break;
            case '\n': oss << "\\n";  break;
            case '\r': oss << "\\r";  break;
            case '\t': oss << "\\t";  break;
            default:
                // Handle control characters (0x00 to 0x1f)
                if (c <= 0x1f) {
                    // Output as \uXXXX hexadecimal representation
                    oss << "\\u"
                        << std::hex // Switch stream to hexadecimal mode
                        << std::setw(4)
                        << std::setfill('0')
                        << static_cast<int>(c); // Cast unsigned char to int for printing
                    oss << std::dec; // IMPORTANT: Switch stream back to decimal mode
                } else {
                    // Append printable and extended ASCII characters directly
                    oss << c;
                }
        }
    }
    return oss.str();
}

/**
 * @brief Serializes the internal token list (_tokenList) into a JSON string.
 * @return A std::string containing the JSON representation of the token list.
 */
 std::string RandomTokenList::toJson() const {
    std::ostringstream oss;
    oss << "[\n"; // Start of JSON array, add newline

    bool isFirstToken = true;
    // Using C++98 compatible iterator loop
    for (std::vector<Token>::const_iterator it = _tokenList.begin(); it != _tokenList.end(); ++it) {
        if (!isFirstToken) {
            oss << ",\n"; // Add comma and newline before subsequent elements
        } else {
            isFirstToken = false; // No comma needed for the first element
        }

        oss << "\t{"; // Indent (level 1) and start JSON object for the token

        // Add "type" key-value pair with indentation (level 2)
        oss << "\n\t\t\"type\":\"" << tokenTypeToString((*it).type) << "\",";

        // Add "value" key-value pair with indentation (level 2)
        oss << "\n\t\t\"value\":\"" << escapeJsonString((*it).value) << "\"";

        oss << "\n\t}"; // Add newline, indent (level 1), and close JSON object

        // Note: isFirstToken was set to false after the first iteration above
    }

    oss << "\n]"  << std::endl; // Add newline before closing JSON array
    return oss.str();
}