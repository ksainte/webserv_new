#include "../inc/Listener.hpp"
#include "../inc/Logger.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/constants/SuccessMessages.hpp"
#include "../inc/utils.hpp"
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>

int	Listener::handleEvent(const Event* event, int flags) 
{
	LOG_INFO << "New connection request\n";

	(void)flags;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int clientfd = ::accept(event->getFd(), (struct sockaddr *)&client_addr, &client_len);

	if (_connManager.initNewConnection(clientfd, event->getFd()) == -1)
	{
		close(clientfd);
		return -1;
	}

	LOG_INFO << "Connection accepted\n";

	return 0;
}

int Listener::handleError() 
{
	return false;
}

Listener::Listener(const std::list<IpPort>& ipPortList, EventManager& eventManager, ConnectionManager& connManager):
_connManager(connManager),
_eventManager(eventManager)
{
	if (iterateThroughIpPortList(ipPortList) == -1)
		throw std::runtime_error(ErrorMessages::E_SOCK_INIT);

	int i = 0;
	for (std::list<int>::const_iterator it = sockfds.begin(); it != sockfds.end(); ++it)
	{
		_events[i] = Event(*it, static_cast<IEventHandler *>(this));
		_eventManager.registerEvent(uint32_t(EPOLLIN), &_events[i]);
		++i;
	}
}

Listener::~Listener() 
{
	// 1. Iterate through each socket fd 
	for (std::list<int>::const_iterator it = sockfds.begin(); it != sockfds.end(); ++it)
	{
		// 2. Close it
		if (close(*it) == -1)
			LOG_WARNING << strerror(errno);
	}
}

int Listener::initSockFd(const IpPort& ipPort) const
{

	// 1. Create a new TCP socket
	int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if (sockfd == -1)
	{
		LOG_CRITICAL << ErrorMessages::E_SOCK_INIT << strerror(errno);
		return -1;
	}

	// 2. Initialize struct sockaddr_in
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(ipPort.port);

	//Use our own converter
	addr.sin_addr.s_addr = ipV4ToNl(ipPort.ip);

	// 3. Configure the socket
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET,
		SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		LOG_CRITICAL << ErrorMessages::E_SOCK_INIT << strerror(errno);
		return -1;
	}

	// 4. Assign an address and a port to the socket fd
	if (bind(sockfd, (struct sockaddr *)&addr, 
	static_cast<socklen_t>(sizeof(addr))) == -1)
	{
		LOG_CRITICAL << ErrorMessages::E_SOCK_INIT << strerror(errno);
		return -1;
	}
	// 5. Pass the socket in listen state
	if (listen(sockfd, SOMAXCONN) == -1)
	{
		LOG_CRITICAL << ErrorMessages::E_SOCK_INIT << strerror(errno);
		return -1;
	}

	// 6. Return the socket fd
	LOG_INFO << SuccessMessages::SOCK_INIT;
	return sockfd;
}

int Listener::iterateThroughIpPortList(const std::list<IpPort>& ipPortList) 
{
	
	// 1. Iterate through ip:port list
	for (std::list<IpPort>::const_iterator it = ipPortList.begin(); it != ipPortList.end(); ++it)
	{

		// 2. Call initSockFd for each list element
		int sockfd = initSockFd(*it);
		if (sockfd == -1)
			return -1;

		// 3. Store the returned fd inside a list
		sockfds.push_back(sockfd);
	}
	return 0;
}