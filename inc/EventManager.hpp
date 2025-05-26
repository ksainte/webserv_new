#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP
#include <sys/epoll.h>
#include "Event.hpp"

class EventManager
{
public:
  EventManager();
  ~EventManager();

  int registerEvent(int eventFlags, Event* ptr) const;
  int modifyEvent(int eventFlags, Event* ptr) const;
  int unregisterEvent(int fd) const;

  void run();

  int getEpfd() const;
  int getSize() const;
  const  epoll_event* getEvents() const;

protected:
  int _epfd;
  static const int SIZE = 1024;
  struct epoll_event _events[SIZE];
};
#endif
