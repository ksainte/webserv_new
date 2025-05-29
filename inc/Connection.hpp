#ifndef CONNECTION_HPP
#define CONNECTION_HPP
#include "IEventHandler.hpp"
#include "../inc/Event.hpp"
#include "../inc/Request.hpp"
#include "../inc/Searcher.hpp"

class Epoll;
class ConnectionManager;

class Connection : public virtual IEventHandler, public Request
{
  bool sendResponse() const;
  int send_to_cgi(const std::string& absPath) const;

  Epoll* _manager;
  Searcher* _searcher;
  int _sockFd;
  int _clientFd;
  Event _event;

public:
  Connection();
  Connection(Searcher& searcher, Epoll& manager);
  Connection(const Connection& other);
  Connection& operator=(const Connection& other);
  ~Connection();

  int handleError() { return 1; }
  int handleEvent(const Event* p, unsigned int flags);

  int getSockFd() const;
  int getClientFd() const;
  Searcher* getSearcher() const;
  Epoll* getManager() const;
  Event* getEvent();

  void setEvent();
  void setSockFd(int sockFd);
  void setClientFd(int clientFd);

};
#endif
