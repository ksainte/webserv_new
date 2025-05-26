#ifndef RANDOMTOKENLIST_HPP
#define RANDOMTOKENLIST_HPP
#include <vector>
#include "types/TokenType.hpp"

class RandomTokenList
{
public:
  RandomTokenList();
  ~RandomTokenList();
  const std::vector<Token>& getTokenList() const;
  void generateDirective();
  void generateServerBlock();
  std::string toJson() const;
  std::string randStr(size_t length) const;
  std::string tokenTypeToString(Token::TokenType type) const;
  std::string escapeJsonString(const std::string& input) const;

private:
  std::vector<Token> _tokenList;
  static const std::string TESTCHAR;
  static const int MAX_VAL = 1000;
  static const int MAX_LEN = 20;
  static const int MAX_STR_SEQ = 5;
  static const int MAX_DIRECTIVES_PER_BLOCK = 10;
  static const int LOCATION_PATH_MAX_LEN = 10;
};
#endif
