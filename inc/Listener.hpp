#ifndef LISTENER_HPP
#define LISTENER_HPP
#include "ConnectionManager.hpp"
#include "EventManager.hpp"
#include "IEventHandler.hpp"
#include <list>
#include <map>

typedef struct IpPort 
{
	const char 			*ip;
	unsigned short	port;
	IpPort(const char *ip, const unsigned short port) 
		: ip(ip), port(port) {};
} IpPort;

class Listener: public virtual IEventHandler, public ConnectionManager
{

	public:

	Listener(const std::list<std::pair<int, int> >& addresses);
	void run();
	// ~Listener();

	private:


	std::list<int> sockfds;
	// std::map<int, Event *> _eventMap;

	int	initSockFd(const std::pair<int, int>& address) const;
	int	iterateThroughIpPortList(const std::list<std::pair<int, int> >& addresses);

	int	handleEvent(const Event* event, int flags);
	int	handleError();
};
#endif