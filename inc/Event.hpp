#ifndef EVENT_HPP
#define EVENT_HPP
#include "IEventHandler.hpp"
class Event
{
	public:

	// Event(int fd, IEventHandler* handler);
	Event();
	void fillEvent(int fd, IEventHandler* handler);
	// ~Event();
	// Event(const Event& other);
	// Event& operator=(const Event& other);

	int getFd() const;
	IEventHandler* getHandler() const;

	protected:

	int _fd;
	int _fd1;
	IEventHandler *_handler;
};
#endif