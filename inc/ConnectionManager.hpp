#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP
#include "Connection.hpp"
class Searcher;
class Epoll;

class ConnectionManager
{
public:
  ConnectionManager(Searcher& searcher, Epoll& eventManager);
  ~ConnectionManager();

  int initNewConnection(int clientFd, int sockFd);
  void checkConnectionTimeouts();

private:
  Epoll& _eventManager;
  Searcher& _searcher;
  static const int MAX_CONN = 200;
  static int _numCon;
  Connection connection[MAX_CONN];
};
#endif
