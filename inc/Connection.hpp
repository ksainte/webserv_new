#ifndef CONNECTION_HPP
#define CONNECTION_HPP
#include "IEventHandler.hpp"
#include "IParser.hpp"
#include "../inc/Event.hpp"
#include "../inc/Request.hpp"
#include "../inc/Searcher.hpp"

class Epoll;
class ConnectionManager;

class Connection : public virtual IEventHandler, public Request, public IParser
{

  bool sendResponse();

  int send_to_cgi(const char * absPath);
  // int send_to_cgi(const std::string& absPath) const;
    // int send_to_cgi(const std::string& absPath) const;
        // int send_to_cgi(const std::string& absPath) const;


  Epoll* _manager;
  Searcher* _searcher;
  int _sockFd;
  int _clientFd;
  Event _event;
  std::string _ErrResponse;

  //GET
  std::ifstream MyReadFile;
  char _buffer[4096];

  const std::string& getErrorMessage(int errnum);

  static std::map<int, std::string> create_status_map() {
    std::map<int, std::string> m;
    m[400] = "400 Bad Request";
    m[401] = "401 Unauthorized";
    m[403] = "403 Forbidden";
    m[404] = "404 Not Found";
    m[500] = "500 Internal Server Error";
    m[501] = "501 Not Implemented";
    m[502] = "502 Bad Gateway";
    m[503] = "503 Service Unavailable";
    return m;
  }

public:
  Connection();
  Connection(Searcher& searcher, Epoll& manager);
  Connection(const Connection& other);
  Connection& operator=(const Connection& other);
  ~Connection();

  // Helper to parse request first line
  bool validPath(const std::string& path) const;
  bool supportedVersion(const std::string& version) const;
  bool allowedMethod(const std::string& method) const;

  int handleError(int errnum);
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
