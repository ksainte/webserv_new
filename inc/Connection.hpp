#ifndef CONNECTION_HPP
#define CONNECTION_HPP
#include "IEventHandler.hpp"
#include "IParser.hpp"
#include "../inc/Event.hpp"
#include "../inc/Request.hpp"
#include "../inc/Searcher.hpp"

class Epoll;
class ConnectionManager;

class Connection : public virtual IEventHandler, public Request
{

  void _defaultErrorPage(int errnum);
  bool _checkDefaultFileAccess(const std::string& prefix);
  void _isPathValid();
  void _isMethodAllowed() const;
  static void _isHttpVersionSupported(const std::string& version) ;

  bool isGetRequestaCGI();
  int send_to_cgi(const char * absPath);
  int readFILE();

  void  _checkBodySize() const;

  // 50MB
  static const int _defaultMaxBodySize = 50 * 1000 * 1000;

  Epoll* _manager;
  Searcher* _searcher;
  int _sockFd;
  Event _event;
  std::string _ErrResponse;
  std::string _listDir;
  std::string absPath;
  std::string cgiPath;
  std::string getContentType();
  int prepareEnvForGetCGI();
  void prepareResponse(const Event* p);
  void preparePostRequest(const Event* p);
  void prepareDeleteRequest(const Event* p);
  bool isNotEmpty(const Event* p);
  const LocationBlock* location;
  void sendToGetCGI(std::vector<char*> env);
  bool _continueReadingFile;
  bool _areHeadersSent;
  void sendResponseHeaders();
  std::vector<char*> createMinEnv();

  //GET
  std::ifstream MyReadFile;
  char _buffer[4096];

  static const std::string& getErrorMessage(int errnum);

  static std::map<int, std::string> create_status_map() {
    std::map<int, std::string> m;
    m[400] = "400 Bad Request";
    m[401] = "401 Unauthorized";
    m[403] = "403 Forbidden";
    m[404] = "404 Not Found";
    m[405] = "405 Method Not Allowed";
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

  void handleError(int errnum);
  int handleEvent(const Event* p, unsigned int flags);

  int getSockFd() const;
  Searcher* getSearcher() const;
  Epoll* getManager() const;
  Event* getEvent();

  void setEvent();
  void setSockFd(int sockFd);
  void setClientFd(int clientFd);

};
#endif
