#include <cstring>
#include <sys/epoll.h>
#include <stdexcept>
#include <cstdlib>
#include <unistd.h>
#include "../inc/Epoll.hpp"
#include "../inc/IEventHandler.hpp"
#include "../inc/ConnectionManager.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/constants/SuccessMessages.hpp"
#include "../inc/Logger.hpp"
#include "../inc/utils.hpp"

int	Epoll::registerEvent(const int eventFlags, Event* ptr) const
{
	 epoll_event event = {};
	event.events = eventFlags;
	event.data.ptr = static_cast<void *>(ptr);

	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, ptr->getFd(), &event) == -1)
	{
		LOG_CRITICAL << ErrorMessages::E_EPOLL_CTL_ADD << ": " << strerror(errno);
		return -1;
	}
	LOG_DEBUG << SuccessMessages::S_EPOLL_CTL_ADD;
	return 0;
}

int	Epoll::modifyEvent(const int eventFlags, Event* ptr) const
{
	epoll_event event = {};
	event.events = eventFlags;
	event.data.ptr = static_cast<void *>(ptr);

	if (epoll_ctl(_epfd, EPOLL_CTL_MOD, ptr->getFd(), &event) == -1)
	{
		LOG_CRITICAL << ErrorMessages::E_EPOLL_CTL_MOD << ": " << strerror(errno);
		return -1;
	}
	LOG_DEBUG << SuccessMessages::S_EPOLL_CTL_MOD;
	return 0;
}

void	Epoll::unregisterEvent(const int fd) const
{
	if (epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
			LOG_WARNING << ErrorMessages::E_EPOLL_CTL_DEL << ": " << strerror(errno);
	LOG_DEBUG << SuccessMessages::S_EPOLL_CTL_DEL;
}

Epoll::Epoll() : _epfd(epoll_create1(0)), _events(), _connectionManager(NULL)
{
	if (_epfd == -1)
	{
		LOG_CRITICAL << ErrorMessages::E_EPOLL_INIT << ": " << strerror(errno);
		throw std::runtime_error(ErrorMessages::E_EPOLL_INIT);
	}
	LOG_DEBUG << "EventManager created\n";
}

void	Epoll::wait()
{
	while (getSigIntFlag() == false)
	{
		// Use 1000ms timeout for epoll_wait to allow timeout checking
		const int numEvent = epoll_wait(_epfd, _events, SIZE, 1000);
	
		if (numEvent == -1)
		{
 			LOG_CRITICAL << ErrorMessages::E_EPOLL_WAIT << ": " << strerror(errno);
			throw std::runtime_error(ErrorMessages::E_EPOLL_WAIT);
		}
	
		for (int i = 0; i < numEvent; ++i)
		{
			const Event* event = static_cast<Event *>(_events[i].data.ptr);
			if (event->getHandler()->handleEvent(event, _events[i].events) == 1)
			{
				unregisterEvent(event->getFd());
				close(event->getFd());
			}
		}
		
		// Check for connection timeouts when there are no events or periodically
		if (_connectionManager && numEvent == 0)
		{
			_connectionManager->checkConnectionTimeouts();
		}
	}
}

Epoll::~Epoll()
{
	close(_epfd);
	LOG_DEBUG << "EventManager destroyed\n";
}

int Epoll::getEpfd() const {return _epfd;}
int Epoll::getSize() {return SIZE;}
const epoll_event*	Epoll::getEvents() const {return _events;}

void Epoll::setConnectionManager(ConnectionManager* connManager)
{
	_connectionManager = connManager;
	LOG_DEBUG << "ConnectionManager set for timeout monitoring\n";
}