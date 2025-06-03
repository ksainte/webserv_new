#include "../inc/Listener.hpp"
#include "../inc/Logger.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/constants/SuccessMessages.hpp"
#include "../inc/utils.hpp"
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>

std::list<int> Listener::_sockFds;

int Listener::handleEvent(const Event* event, const unsigned int flags)
{
  LOG_INFO << "New connection request\n";

  // shut up the compiler
  (void)flags;

  sockaddr_in addr = {};
  socklen_t client_len = sizeof(addr);
  const int clientFd = ::accept(event->getFd(), reinterpret_cast<struct sockaddr*>(&addr), &client_len);

  if (_connManager.initNewConnection(clientFd, event->getFd()) == -1)
  {
    close(clientFd);
    return -1;
  }

  LOG_INFO << "Connection accepted\n";

  return 0;
}

int Listener::handleError(const int errnum)
{
  (void)errnum;
  return false;
}

Listener::Listener(const std::list<std::pair<int, int> >& addresses, Epoll& eventManager,
                   ConnectionManager& connManager):
  _connManager(connManager),
  _eventManager(eventManager)
{
  if (_iterateThroughIpPortList(addresses) == -1)
    throw std::runtime_error(ErrorMessages::E_SOCK_INIT);

  int i = 0;
  for (std::list<int>::const_iterator it = _sockFds.begin(); it != _sockFds.end(); ++it)
  {
    _events[i] = Event(*it, this);
    _eventManager.registerEvent(EPOLLIN, &_events[i]);
    ++i;
  }
}

Listener::~Listener()
{
  // 1. Iterate through each socket fd
  for (std::list<int>::const_iterator it = _sockFds.begin(); it != _sockFds.end(); ++it)
  {
    // 2. Close it
    if (close(*it) == -1)
      LOG_WARNING << strerror(errno);
  }
}

int Listener::_initSockFd(const std::pair<int, int>& address)
{
  // 1. Create a new TCP socket
  const int sockFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if (sockFd == -1)
  {
    LOG_CRITICAL << ErrorMessages::E_SOCK_INIT << strerror(errno);
    return -1;
  }

  // 2. Initialize struct sockaddr_in
  sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = address.second;
  addr.sin_addr.s_addr = address.first;

  // 3. Configure the socket
  const int opt = 1;
  if (setsockopt(sockFd, SOL_SOCKET,
                 SO_REUSEADDR, &opt, sizeof(opt)) == -1)
  {
    LOG_CRITICAL << ErrorMessages::E_SOCK_INIT << strerror(errno);
    return -1;
  }

  // 4. Assign an address and a port to the socket fd
  if (bind(sockFd, reinterpret_cast<sockaddr*>(&addr),
           static_cast<socklen_t>(sizeof(addr))) == -1)
  {
    LOG_CRITICAL << ErrorMessages::E_SOCK_INIT << strerror(errno);
    return -1;
  }
  // 5. Pass the socket in listen state
  if (listen(sockFd, SOMAXCONN) == -1)
  {
    LOG_CRITICAL << ErrorMessages::E_SOCK_INIT << strerror(errno);
    return -1;
  }

  // 6. Return the socket fd
  LOG_INFO << SuccessMessages::SOCK_INIT;
  return sockFd;
}

int Listener::_iterateThroughIpPortList(const std::list<std::pair<int, int> >& addresses)
{
  // 1. Iterate through ip:port list
  for (std::list<std::pair<int, int> >::const_iterator it = addresses.begin(); it != addresses.end(); ++it)
  {
    // 2. Call initSockFd for each list element
    const int sockFd = _initSockFd(*it);
    if (sockFd == -1)
      return -1;

    // 3. Store the returned fd inside a list
    _sockFds.push_back(sockFd);
  }
  return 0;
}
