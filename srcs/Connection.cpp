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
    std::map<std::string, std::string> mapName;

      std::cout << "current buff is: \n"<<  str;
      mapName = set_headers();
      int pid = fork();

      if (pid == 0) 
      {
        send_response(p->getFd(), getFilename(), getMethod());
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
  // LOG_DEBUG << "Connection Filled\n";
}

void Connection::setManager(ConnectionManager* manager) {
  _manager = manager;
}

//http://127.0.0.2:8080/contact.htm

//        /contact.htm                                              /contents/contact.htm          /contents/contact.htm -> route = /contents/, uri = contact.htm

//call function avec /contact.htm ->     location /contact {

//route /contact ou si pas il return null

//retirer de l url la route -> .htm = uri, la difference entre !


//route c est la location!

//racine c est dir root!

//->pour cette location

//        root /contents; ou NULL si pas setup

// root est fichier ou dossier?

// /contents est un dossier!

// si dossier, append uri et essayer acceder au fichier, /contents + / + .htm = /contents/.htm -> pas access

// si il trouve en root ->/Apps/19/webserv/contents

// /Apps/19/webserv + / + uri = /Apps/19/webserv/contents/contact.htm

// si fichier, tester si permission et existe->return direct le fichier! pas cense en etre un!

///Apps/19/webserv/contents/contact.htm

///Apps/19/webserv/contents/contact.htm si pas accessible!

//si tu tombes sur un dossier!

// si pas fichier-> index existe?

// si index existe, append a la route!

// si existe pas? depend d une autre directive= autoindex

// autoindex = true->lister repertoire




