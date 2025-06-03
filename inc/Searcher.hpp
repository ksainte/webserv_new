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

  static bool findHost(const ConfigType::DirectiveMap& directives,
                       const std::string& host);


  std::list<std::pair<int, int> > _addresses;

public:
  explicit Searcher(const Config&);

  const ServerBlock& getDefaultServer(int sockFd, const std::string& host) const;
  const LocationBlock* getLocation(int sockFd, const std::string& host, const std::string& url) const;

  const std::list<std::pair<int, int> >& getAddresses() const;

  const ConfigType::DirectiveValue*
  findServerDirective(
    int sockFd,
    const std::string& key,
    const std::string& host) const;

  const ConfigType::DirectiveValue*
  findLocationDirective(
    int sockFd,
    const std::string& key,
    const std::string& host,
    const std::string& route) const;
};
#endif
