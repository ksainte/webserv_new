#include "../inc/Logger.hpp"
#include "../inc/EventManager.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/constants/SuccessMessages.hpp"
#include "../inc/ConnectionManager.hpp"
#include "../inc/Connection.hpp"
#include <cstdlib>

int ConnectionManager::initNewConnection(int clientFd, int sockFd)
{
	connection[_connIndex].setManager(this);
	connection[_connIndex].fillConnection(clientFd, sockFd);
	event[_eventIndex].fillEvent(clientFd, connection + _connIndex);
	uint32_t eventFlags = EPOLLIN;
	if (registerEvent(eventFlags, event + _eventIndex) == -1)
		throw std::exception();
	_eventIndex++;
	_connIndex++;
	return (0);
}

ConnectionManager::ConnectionManager(Searcher &searcher) : _searcher(searcher), EventManager(),_connIndex(0)
{
	LOG_DEBUG << "Connection Manager created\n";
}

ConnectionManager::~ConnectionManager()
{
	LOG_DEBUG << "Connection Manager destroyed\n";
}