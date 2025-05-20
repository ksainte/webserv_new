#include "../inc/Listener.hpp"
#include "../inc/Logger.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/constants/SuccessMessages.hpp"
#include "../inc/utils.hpp"
#include <cstring>
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

	if (initNewConnection(clientfd, event->getFd()) == -1)
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


void	Listener::run()
{
	while (getSigIntFlag() == false)
	{
		int numEvent = epoll_wait(_epfd, _events, MAX_EVENTS, 0);
	
		if (numEvent == -1)
		{
			LOG_CRITICAL << ErrorMessages::E_EPOLL_WAIT << ": " << strerror(errno);
			throw std::runtime_error(ErrorMessages::E_EPOLL_WAIT);
		}
	
		for (int i = 0; i < numEvent; ++i)
		{
			Event* event = static_cast<Event *>(_events[i].data.ptr);
			event->getHandler()->handleEvent(event, _events[i].events);
		}
	}
}


Listener::Listener(const std::list<std::pair<int, int> >& ipPortList, const Searcher& searcher): ConnectionManager(searcher)
{

	if (iterateThroughIpPortList(ipPortList) == -1)
		throw std::runtime_error(ErrorMessages::E_SOCK_INIT);

	for (std::list<int>::const_iterator it = sockfds.begin(); it != sockfds.end(); ++it)
	{

		addTcpEvent(*it, static_cast<IEventHandler *>(this));

	}
}

int Listener::initSockFd(const std::pair<int, int>& address) const
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
	addr.sin_port = htons(address.second);
	addr.sin_addr.s_addr = address.first;

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
	LOG_INFO << SuccessMessages::SOCK_INIT << " " << nlToipv4(address.first) 
	<< ":" << address.second;
	return sockfd;
}

int Listener::iterateThroughIpPortList(const std::list<std::pair<int, int> >& addresses) 
{
	
	// 1. Iterate through ip:port list
	for (std::list<std::pair<int, int> >::const_iterator it = addresses.begin(); it != addresses.end(); ++it)
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

// Listener::~Listener() 
// {
// 	// 1. Iterate through each socket fd 
// 	for (std::list<int>::const_iterator it = sockfds.begin(); it != sockfds.end(); ++it)
// 	{
// 		// 2. Close it
// 		if (close(*it) == -1)
// 			LOG_WARNING << strerror(errno);
// 	}

// 	for (std::map<int, Event *>::const_iterator it = _eventMap.begin(); it != _eventMap.end(); ++it)
// 		delete it->second;

// 	_eventMap.clear();
// }