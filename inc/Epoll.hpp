#ifndef EPOLL_HPP
#define EPOLL_HPP
#include <sys/epoll.h>
#include "Event.hpp"

class ConnectionManager;

class Epoll
{
public:
  Epoll();
  ~Epoll();

  int   registerEvent(int eventFlags, Event* ptr) const;
  int   modifyEvent(int eventFlags, Event* ptr) const;
  void  unregisterEvent(int fd) const;

  void wait();
  void setConnectionManager(ConnectionManager* connManager);

  int getEpfd() const;
  static int getSize();
  const  epoll_event* getEvents() const;

private:
  int _epfd;
  static const int SIZE = 1024;
  epoll_event _events[SIZE];
  ConnectionManager* _connectionManager;
};
#endif
