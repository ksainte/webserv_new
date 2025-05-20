#include "../inc/Connection.hpp"
#include "../inc/ConnectionManager.hpp"
#include "../inc/Event.hpp"
#include "../inc/EventManager.hpp"
#include "../inc/Event.hpp"
#include <string>
#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

Connection::Connection(): Request(), Response(), _sockFd(-1), _clientFd(-1), _manager(NULL){}

// Connection::~Connection() {}

int Connection::handleEvent(const Event* p, int flags) 
{
  
	if ((flags & EPOLLIN) && read(p->getFd()) == 0)
  {
		_manager->modifyEvent(uint32_t(EPOLLOUT), const_cast<Event*>(p));
  }
	else if (flags & EPOLLOUT) 
  {
    std::map<std::string, std::string> key_value_headers;//array de pointeur, tu accedes a it qui est un pointeur

      // std::cout << "current buff is: \n"<<  str;
      key_value_headers = set_headers();
      
      // for(std::map<std::string, std::string>::const_iterator it = key_value_headers.begin(); it != key_value_headers.end(); ++it)
      // {
      //   std::cout << it->first << ":" << it->second <<  "\n";
      // }
      int pid = fork();

      if (pid == 0)
      {
        send_response(p->getFd(), getFilename(), getMethod(), _manager->_searcher, key_value_headers);
        exit(0);
      }
			_manager->unregisterEvent(p->getFd());
      close(p->getFd());
	}	
	return 0;
}

void Connection::fillConnection(int clientFd, int sockFd)
{
  _clientFd = clientFd;
  _sockFd = sockFd;
}

void Connection::setManager(ConnectionManager* manager) {
  _manager = manager;
}

