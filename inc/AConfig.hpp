#ifndef ACONFIG_HPP
#define ACONFIG_HPP
#include <cassert>
#include "../inc/ServerBlock.hpp"
#include "../inc/types/ConfigType.hpp"
#include "../inc/types/TokenType.hpp"

template <typename T>
class AConfig : public ConfigType
{
public:
  const ServerBlocks& getServerBlocks() const
  {
    return _serverBlocks;
  };

  std::string toJson(const int indentLevel) const
  {
    std::stringstream ss;

    for (ServerBlocks::const_iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); ++it)
    {
      ss << it->toJson(indentLevel);
      if (static_cast<unsigned int>(it - _serverBlocks.begin()) < _serverBlocks.size() - 1)
        ss << ',';
      ss << '\n';
    }
    return ss.str();
  }

protected:
  ServerBlocks _serverBlocks;

  explicit AConfig(T tokens) : _func(), _target(-1), _it(tokens.begin()), _end(tokens.end())
  {
    _func[0] = &AConfig::_serverBlockHandler;
    _func[1] = &AConfig::_locationBlockHandler;
    _func[2] = &AConfig::_directiveHandler;
    for (;_it != _end; ++_it)
      checkTokenType();

    LOG_DEBUG << "configuration loaded";
  }

  virtual ~AConfig()
  {
    LOG_DEBUG << "AConfig destroyed\n";
  };

  void checkTokenType()
  {
    const Token::TokenType type = (*_it).type;
    _value = (*_it).value;

    if (type == Token::LBRACE) ++_target;

    if (type == Token::RBRACE) --_target;

    if (type == Token::LOCATION) _prefix = _value;

    //This case lacks his own Token type
    if (_value == "listen")
    {
      typename T::const_iterator tmp = _it;
      std::advance(tmp, 1);
      _serverBlocks[_serverBlocks.size() - 1].setIpPort((*tmp).value);
      return ;
    }
    //Call token handler function
    if (type < _size)
      (this->*_func[type])();
  };

private:
  static const int _size = 3;
  typedef void (AConfig::*tokenFunc)();
  tokenFunc _func[_size];
  int _target;
  std::string _prefix;
  std::string _value;
  typename T::const_iterator _it;
  typename T::const_iterator _end;

  void _directiveHandler()
  {
    assert(!_serverBlocks.empty() && "size must not be 0");

    // Add directive directly to the last server block
    if (_target == SERVER && _value != "error_page")
    {
      _serverBlocks[_serverBlocks.size() - 1]
        .addDirective(_it, _end);
      return ;
    }

    if (_target == SERVER && _value == "error_page")
    {
      _serverBlocks[_serverBlocks.size() - 1].addErrorPage(_it, _end);
      return ;
    }

    if (_target == LOCATION && _value == "error_page")
    {
      _serverBlocks[_serverBlocks.size() - 1]
        .search(_prefix)
          ->addErrorPage(_it, _end);
    }

    // Add directive to a location sub-block in the last server block
    if (_value != "cgi_params" && _value != "error_page")
    {
      _serverBlocks[_serverBlocks.size() - 1]
        .search(_prefix)
          ->addDirective(_it, _end);
      return;
    }

    // Quick fix to store cgi_params since this directive isn't
    // unique and cant be store inside the directive map
    if (_value == "cgi_params")
    {
      typename T::const_iterator tmp = _it;
      typename T::const_iterator tmp2 = _it;
      std::advance(tmp, 1);
      std::advance(tmp2, 2);
      _serverBlocks[_serverBlocks.size() - 1]
        .search(_prefix)
        ->addCgiParams((*tmp).value, (*tmp2).value);
    }

  }

  void _serverBlockHandler()
  {
    _serverBlocks.push_back(ServerBlock());
  }

  void _locationBlockHandler()
  {
    _serverBlocks[_serverBlocks.size() - 1]
      .insert(new LocationBlock(_prefix));
  }

  enum BlockType
  {
    SERVER = 0,
    LOCATION = 1
  };
};
#endif
// template <typename IteratorType>
// void checkTokenType(const IteratorType& it, const IteratorType& last)
// {
//   static int blockType;
//   static std::string prefix;
//
//   switch ((*it).type)
//   {
//   case Token::SERVER:
//     serverBlockHandler();
//     break;
//   case Token::DIRECTIVE:
//     directiveHandler(it, last, blockType, prefix);
//
//     // Quick fix to store the ip and port
//     // in decimal in network bytes order
//     if ((*it).value == "listen")
//     {
//       IteratorType tmp = it;
//       std::advance(tmp, 1);
//       _serverBlocks[_serverBlocks.size() - 1].setIpPort((*tmp).value);
//     }
//     break;
//
//   case Token::LOCATION:
//     locationBlockHandler((*it).value);
//     prefix = (*it).value;
//     break;
//   case Token::RBRACE:
//     --blockType;
//     break;
//   case Token::LBRACE:
//     ++blockType;
//     break;
//   default:
//     break;
//   }
// };
