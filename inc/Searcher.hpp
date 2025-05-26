#ifndef SEARCHER_HPP
# define SEARCHER_HPP
#include <functional>
#include <list>
#include "../inc/Config.hpp"
#include "../inc/types/ConfigType.hpp"

struct FindPairEqual :
  std::unary_function<std::pair<int, int>, bool>
{
  std::pair<int, int> target;

  explicit FindPairEqual(const std::pair<int, int>& t): target(t)
  {
  }

  bool operator()(const std::pair<int, int>& p) const
  {
    return p == target;
  }
};

class Searcher
{

  const Config& _config;

  static bool isServerNameSet(const ConfigType::DirectiveMap& directives,
                              const char* hostname);

  void iterateThroughServerBlock(const ConfigType::ServerBlockIt& first,
                                 const ConfigType::ServerBlockIt& last);

  const ServerBlock& _getDefaultServer(int sockFd, const char* hostname) const;

  std::list<std::pair<int, int> > _addresses;

  void _storeAddress(int address, int port);

public:
  explicit Searcher(const Config&);

  const char* getLocationPrefix(int sockFd, const char* host, const char* url) const;

  const std::list<std::pair<int, int> >& getAddresses() const;

  const ConfigType::DirectiveValue* findServerDirective(int sockFd,
                                                        const std::string& key, const char* host) const;

  const ConfigType::DirectiveValue* findLocationDirective(int sockFd,
                                                          const std::string& key, const char* hostname,
                                                          const char* route) const;
};
#endif
