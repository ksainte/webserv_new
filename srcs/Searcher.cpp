#include "../inc/Searcher.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../inc/Logger.hpp"
#include "../inc/ServerBlock.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/constants/SuccessMessages.hpp"

bool Searcher::findHost(const ConfigType::DirectiveMap& directives, const std::string& host)
{
  // Get server name directive
  ConfigType::DirectiveMapIt it = directives.find(std::string("server_name"));

  if (it == directives.end()) return false;

  // Search host
  ConfigType::DirectiveValueIt it2 = std::find(it->second.begin(), it->second.end(), host);

  if (it2 == it->second.end()) return false;

  LOG_INFO << "Hostname \'" << host << "\' "
    << SuccessMessages::HOST_NAME_FOUND;

  // Return true when host found
  return true;
}

const ServerBlock& Searcher::getDefaultServer(const int sockFd, const std::string& host) const
{
  // Get socket address
  sockaddr_in addr = {};
  socklen_t addrLen = sizeof(addr);
  getsockname(sockFd, reinterpret_cast<sockaddr*>(&addr), &addrLen);

  // Access serverBlocks vector
  const ConfigType::ServerBlocks& serverBlocks = _config.getServerBlocks();

  // Point to the first serverBlock who  address matches socket address
  ConfigType::ServerBlockIt defaultServer;
  bool defaultServerSet = false;

  for (ConfigType::ServerBlockIt it = serverBlocks.begin(); it != serverBlocks.end(); ++it)
  {
    if (it->getIp() != addr.sin_addr.s_addr
      || it->getPort() != addr.sin_port)
      continue;

    if (defaultServerSet == false)
    {
      defaultServer = it;
      defaultServerSet = true;
    }

    if (findHost(it->getDirectives(), host))
      return *it;
  }

  LOG_INFO << ErrorMessages::HOST_NAME_NOT_FOUND;
  return *defaultServer;
}

const LocationBlock* Searcher::getLocation(const int sockFd, const std::string& host, const std::string& url) const
{
  const ServerBlock& serverBlock = getDefaultServer(sockFd, host);
  const LocationBlock* locationBlock = serverBlock.search(url);
  return locationBlock;
}

const ConfigType::DirectiveValue* Searcher::findLocationDirective(const int sockFd,
                                                                  const std::string& key, const std::string& host,
                                                                  const std::string& route) const
{
  // Get the default server for a specific host
  // or the first server
  const ServerBlock& serverBlock = getDefaultServer(sockFd, host);

  // Get route configuration
  const LocationBlock* locationBlock = serverBlock.search(route);
  if (!locationBlock)
  {
    LOG_ERROR << "\'" << route << "\' " << ErrorMessages::E_BAD_ROUTE;
    return NULL;
  }

  // Access route configuration to get the directive
  ConfigType::DirectiveMapIt it = locationBlock->getDirectives().find(key);
  if (it == locationBlock->getDirectives().end())
  {
    LOG_INFO << "\'" + key + "\' " << ErrorMessages::KEY_NOT_FOUND;
    return NULL;
  }

  return &(it->second);
}

const ConfigType::DirectiveValue* Searcher::findServerDirective(const int sockFd,
                                                                const std::string& key, const std::string& host) const
{
  // Get the default server for a specific host
  const ServerBlock& serverBlock =
    getDefaultServer(sockFd, host);

  // Get directive
  ConfigType::DirectiveMapIt it = serverBlock.getDirectives().find(key);
  if (it == serverBlock.getDirectives().end())
  {
    LOG_INFO << "\'" + key + "\' " << ErrorMessages::KEY_NOT_FOUND;
    return NULL;
  }

  LOG_INFO << "\'" + key + "\' " << SuccessMessages::KEY_FOUND;

  // Return directive value
  return &(it->second);
}

const std::list<std::pair<int, int> >& Searcher::getAddresses() const
{
  return _addresses;
}

Searcher::Searcher(const Config& config) : _config(config)
{
  for (ConfigType::ServerBlockIt it = config.getServerBlocks().begin(); it != config.getServerBlocks().end(); ++it)
  {
    if (std::find_if(_addresses.begin(), _addresses.end(),
                     FindPairEqual(std::make_pair((*it).getIp(), (*it).getPort()))) == _addresses.end())
    {
      _addresses.push_back(std::make_pair(it->getIp(), it->getPort()));
    }
  }

  LOG_DEBUG << "Searcher created";
}
