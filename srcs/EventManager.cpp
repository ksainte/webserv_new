#include <cstring>
#include <sys/epoll.h>
#include <stdexcept>
#include <cstdlib>
#include <unistd.h>
#include "../inc/EventManager.hpp"
#include "../inc/IEventHandler.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/constants/SuccessMessages.hpp"
#include "../inc/Logger.hpp"
#include "../inc/utils.hpp"

int	EventManager::registerEvent(int eventFlags, Event* ptr) const
{
	struct epoll_event event;
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

int	EventManager::modifyEvent(int eventFlags, Event* ptr) const
{
	struct epoll_event event;
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

int	EventManager::unregisterEvent(int clientFd) const
{
	if (epoll_ctl(_epfd, EPOLL_CTL_DEL, clientFd, NULL) == -1)
	{
		LOG_WARNING << ErrorMessages::E_EPOLL_CTL_DEL << ": " << strerror(errno);
		return -1;
	}
	LOG_DEBUG << SuccessMessages::S_EPOLL_CTL_DEL;
	return 0;
}

EventManager::EventManager() : _epfd(epoll_create1(0)) 
{
	if (_epfd == -1)
	{
		LOG_CRITICAL << ErrorMessages::E_EPOLL_INIT << ": " << strerror(errno);
		throw std::runtime_error(ErrorMessages::E_EPOLL_INIT);
	}
	LOG_DEBUG << "EventManager created\n";
}

void	EventManager::run()
{
	while (getSigIntFlag() == false)
	{
		int numEvent = epoll_wait(_epfd, _events, _SIZE, 0);
	
		if (numEvent == -1)
		{
			LOG_CRITICAL << ErrorMessages::E_EPOLL_WAIT << ": " << strerror(errno);
			throw std::runtime_error(ErrorMessages::E_EPOLL_WAIT);
		}
	
		for (int i = 0; i < numEvent; ++i)
		{
			Event* event = static_cast<Event *>(_events[i].data.ptr);
			if (event->getHandler()->handleEvent(event, _events[i].events) == -1)
				unregisterEvent(event->getFd());
		}
	}
}

EventManager::~EventManager()
{
	close(_epfd);
	LOG_DEBUG << "EventManager destroyed\n";
}

int EventManager::getEpfd() const {return _epfd;}
int EventManager::getSize() const {return _SIZE;}
const struct epoll_event*	EventManager::getEvents() const {return _events;}