#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP
#include "Connection.hpp"
#include "EventManager.hpp"

class Searcher;

class ConnectionManager:public EventManager
{
	public:

	ConnectionManager(const Searcher &searcher);
	~ConnectionManager();

	int	initNewConnection(int clientFd, int sockFd);
	// int handleEvent(const Event* p, int flags);
	// int	closeConnection(int fd);
	
	// const std::map<int, Event *> &getData() const;
	
	public:
	
	const Searcher &_searcher;
	int _connIndex;
	Connection connection[5];
	static const int MAX_CONN = 1000;

};
#endif

