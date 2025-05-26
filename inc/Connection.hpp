#ifndef CONNECTION_HPP
#define CONNECTION_HPP
#include <map>
#include "IEventHandler.hpp"
#include "../inc/Event.hpp"
#include "../inc/Request.hpp"
#include "../inc/Searcher.hpp"

class EventManager;
class ConnectionManager;

class Connection : public virtual IEventHandler, public Request
{
  typedef std::map<std::string, std::string> Headers;
  typedef std::map<std::string, std::string>::const_iterator HeaderIt;

  bool sendResponse() const;
  int send_to_cgi(const std::string& absPath) const;
  int isDirectoryExists(const char* path) const;

  EventManager* _manager;
  Searcher* _searcher;
  int _sockFd;
  int _clientFd;
  Headers _headers;
  Event _event;

public:
  Connection();
  Connection(Searcher& searcher, EventManager& manager);
  Connection(const Connection& other);
  Connection& operator=(const Connection& other);
  ~Connection();

  int handleError() { return 1; }
  int handleEvent(const Event* p, int flags);

  int getSockFd() const;
  int getClientFd() const;
  Searcher* getSearcher() const;
  EventManager* getManager() const;
  Event* getEvent();

  void setEvent();
  bool setHeaders();
  void setSockFd(int sockFd);
  void setClientFd(int clientFd);

  const Headers&
  getHeaders() const;
};
#endif
