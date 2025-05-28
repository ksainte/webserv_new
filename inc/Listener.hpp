#ifndef LISTENER_HPP
#define LISTENER_HPP
#include <list>
#include "ConnectionManager.hpp"
#include "Epoll.hpp"
#include "IEventHandler.hpp"

class Searcher;

class Listener : public virtual IEventHandler
{
public:
  Listener(const std::list<std::pair<int, int> >& addresses, Epoll& eventManager, ConnectionManager& connManager);
  ~Listener();
  int handleEvent(const Event* event, unsigned int flags);
  int handleError();

private:
  static const int SOCK_MAX = 10;
  static std::list<int> _sockFds;
  Event _events[SOCK_MAX];
  ConnectionManager& _connManager;
  Epoll& _eventManager;

  static int _initSockFd(const std::pair<int, int>& address);
  static int _iterateThroughIpPortList(const std::list<std::pair<int, int> >& addresses);
};
#endif
