#include "../inc/Logger.hpp"
#include "../inc/EventManager.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/constants/SuccessMessages.hpp"
#include "../inc/ConnectionManager.hpp"
#include "../inc/Connection.hpp"
#include <cstdlib>

// void	ConnectionManager::removeEventMapEntry(int fd)
// {
// 	std::map<int, Event*>::const_iterator it;
// 	it = _eventMap.find(fd); 

// 	if (it == _eventMap.end())
// 		return ;

// 	free(it->second);
// 	_eventMap.erase(fd);

// 	LOG_DEBUG << SuccessMessages::S_MAP_ERASE;
// }

// void	ConnectionManager::removeConnMapEntry(int fd)
// {
// 	std::map<int, Connection*>::const_iterator it;
// 	it = _connMap.find(fd); 

// 	if (it == _connMap.end())
// 		return ;

// 	free(it->second);
// 	_connMap.erase(fd);

// 	LOG_DEBUG << SuccessMessages::S_MAP_ERASE;
// }

// int ConnectionManager::closeConnection(int clientFd)
// {
// 	std::map<int, Connection *>::const_iterator it = _connMap.find(clientFd);
// 	if (it == _connMap.end())
// 		return -1;

// 	free(it->second);
// 	_connMap.erase(clientFd);

// 	return 0;
// }


int ConnectionManager::initNewConnection(int clientFd, int sockFd)
{
	// if (_connIndex == 5 || )
	// 	return -1;

	connection[_connIndex].setManager(this);
	connection[_connIndex].fillConnection(clientFd, sockFd);
	event[_eventIndex].fillEvent(clientFd, connection + _connIndex);
	uint32_t eventFlags = EPOLLIN;
	if (registerEvent(eventFlags, event + _eventIndex) == -1)
		throw std::exception();
	_eventIndex++;
	_connIndex++;
	return 0;
}

//char test[5]

//test + 0
//test + 1

ConnectionManager::ConnectionManager() : EventManager(),_connIndex(0)
{
	LOG_DEBUG << "Connection Manager created\n";
}

// ConnectionManager::~ConnectionManager()
// {
// 	for (std::map<int, Connection *>::const_iterator it = _connMap.begin(); it != _connMap.end(); ++it)
// 	{
// 		delete it->second;
// 	}
// 	_connMap.clear();
// 	for (std::map<int, Event*>::const_iterator it = _eventMap.begin(); it != _eventMap.end(); ++it)
// 	{
// 		delete it->second;
// 	}
// 	_eventMap.clear();
// 	LOG_DEBUG << "Connection Manager destroyed\n";
// }