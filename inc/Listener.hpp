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

class Searcher;

class Listener: public virtual IEventHandler
{

	public:

	Listener(const std::list<IpPort>& ipPortList, EventManager& eventManager, ConnectionManager& connManager);
	~Listener();

	private:

	static const int SOCK_MAX = 10;
	Event _events[SOCK_MAX];
	ConnectionManager& _connManager;
	EventManager& _eventManager;
	std::list<int> sockfds;

	int	initSockFd(const IpPort& ipPort) const;
	int	iterateThroughIpPortList(const std::list<IpPort>& ipPortList);
	int	handleEvent(const Event* event, int flags);
	int	handleError();

};
#endif