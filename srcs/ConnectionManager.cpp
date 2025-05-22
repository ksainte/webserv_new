#include "../inc/Logger.hpp"
#include "../inc/EventManager.hpp"
#include "../inc/ConnectionManager.hpp"
#include "../inc/Connection.hpp"

int ConnectionManager::_numCon = 0;

int ConnectionManager::initNewConnection(int clientFd, int sockFd)
{
	if (_numCon == MAX_CONN) return -1;

	Connection& newCon = connection[_numCon];
	newCon.setSockFd(sockFd);
	newCon.setClientFd(clientFd);
	newCon.setEvent();
	
	uint32_t eventFlags = EPOLLIN;
	if (_eventManager.registerEvent(eventFlags, newCon.getEvent()) == -1)
		return -1;
	++_numCon;

	return (0);
}

ConnectionManager::ConnectionManager(Searcher &searcher, EventManager& eventManager): 
_eventManager(eventManager), 
_searcher(searcher)
{
	for (int i = 0; i < MAX_CONN; ++i) {
		connection[i] = Connection(searcher, eventManager);
	}
	LOG_DEBUG << "Connection Manager created\n";
}

ConnectionManager::~ConnectionManager()
{
	LOG_DEBUG << "Connection Manager destroyed\n";
}