#ifndef EVENT_HPP
#define EVENT_HPP
#include "IEventHandler.hpp"
class Event
{
	public:

	Event();
	Event(int fd, IEventHandler* handler);
	Event(const Event& other);
	Event& operator=(const Event& other);
	~Event();

	int getFd() const;
	IEventHandler* getHandler() const;

	protected:

	int _fd;
	IEventHandler *_handler;
};
#endif