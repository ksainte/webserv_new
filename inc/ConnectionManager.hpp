#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP
#include "Connection.hpp"
class Searcher;
class EventManager;
class ConnectionManager
{
	public:

	ConnectionManager(Searcher &searcher, EventManager& eventManager);
	~ConnectionManager();

	int	initNewConnection(int clientFd, int sockFd);
	
	private:

	const Searcher&			_searcher;
	const EventManager&	_eventManager;
	static const int MAX_CONN = 5;
	static int _numCon;
	Connection connection[MAX_CONN];
};
#endif

