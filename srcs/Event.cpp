#include "../inc/Event.hpp"
#include "../inc/Logger.hpp"

Event::Event(int fd, IEventHandler* handler) :
_fd(fd),
_handler(handler)
{LOG_DEBUG << "Event created\n";}

Event::Event(): _fd(-1), _handler(NULL)
{LOG_DEBUG << "Event created\n";}

Event::~Event() {LOG_DEBUG << "Event destroyed\n";}

Event::Event(const Event& other)
{
	_fd = other.getFd();
	_handler = other.getHandler();
	LOG_DEBUG << "Event copied\n";
}

Event& Event::operator=(const Event& other)
{
	if (this == &other)
		return *this;

	_fd = other.getFd();
	_handler = other.getHandler();
	LOG_DEBUG << "Event copied\n";
	return *this;
}

int Event::getFd() const {return _fd;}

IEventHandler* Event::getHandler() const {return _handler;}