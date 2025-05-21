#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP
#include "Event.hpp"
#include <stdint.h>
#include <sys/epoll.h>
class EventManager
{
	public:

	EventManager();
	~EventManager();

	int	registerEvent(int eventFlags, Event* ptr) const;
	int	modifyEvent(int eventFlags, Event* ptr) const;
	int	unregisterEvent(int fd) const;

	void	run();

	int getEpfd() const;
	int getSize() const;
	const struct epoll_event* getEvents() const;

	protected:

	int _epfd;
	static const int _SIZE = 1024;
	struct epoll_event _events[_SIZE];
};
#endif