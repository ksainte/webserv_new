#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP
#include "Event.hpp"
#include <stdint.h>
#include <sys/epoll.h>
class EventManager : public Event
{
	public:

	EventManager();
	// ~EventManager();

	int	registerEvent(int eventFlags, Event* ptr) const;
	int	modifyEvent(int eventFlags, Event* ptr) const;
	int	unregisterEvent(int fd) const;
	void addTcpEvent(int fd, IEventHandler* handler);

	void	run();

	protected:

	int _epfd;
	Event event[5];
	int _eventIndex;
	static const int MAX_EVENTS = 1024;
	struct epoll_event _events[MAX_EVENTS];
};
#endif