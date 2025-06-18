#ifndef CONNECTION_HPP
#define CONNECTION_HPP
#include "IEventHandler.hpp"
#include "IParser.hpp"
#include "../inc/Event.hpp"
#include "../inc/Request.hpp"
#include "../inc/Searcher.hpp"
#include <sys/time.h>

class Epoll;
class ConnectionManager;

class Connection : public virtual IEventHandler, public Request
{

  void _defaultErrorPage(int errnum);
  bool _checkDefaultFileAccess(const std::string& prefix);
  void _isPathValid();
  void _isMethodAllowed() const;
  static void _isHttpVersionSupported(const std::string& version) ;

  void isRequestaCGI();
  int send_to_cgi(const char * absPath);
  int readFILE();

  void  _checkBodySize() const;

  // 50MB
  static const int _defaultMaxBodySize = 500 * 1000 * 1000;
  
  // Timeout configuration (in seconds)
  static const int _defaultRequestTimeout = 30;  // 30 seconds for regular requests
  static const int _defaultCgiTimeout = 30;       // 5 seconds for CGI requests

  Epoll* _manager;
  Searcher* _searcher;
  int _sockFd;
  Event _event;
  std::string _tmpPathExt;
  std::string _rootPath;
  std::string _ErrResponse;
  std::string _listDir;
  std::string _previousLoc;
  std::string absPath;
  std::string cgiPath;
  std::string getContentType();
  std::vector<std::string> envStorage;
  std::vector<char*> env;
  void discardDupEnvVar();
  void handleChunkedRequest();
  int prepareEnvForGetCGI();
  void prepareResponse(const Event* p);
  void prepareEnvforPostCGI();
  void prepareDeleteRequest();
  bool isNotEmpty(const Event* p);
  const LocationBlock* location;
  void sendToCGI();
  bool _continueReadingFile;
  bool _requestIsACGI;
  bool _isExtensionSet;
  bool _areHeadersSent;
  void sendResponseHeaders();
  void createMinGetEnv();
  void createMinPostEnv();
  void isFileToDeleteValid(int *result);
  void findPathFinalExtension();
  //GET
  std::ifstream MyReadFile;
  char _buffer[4096];
  // std::string 				_tempBuff;
  std::vector<unsigned char> _tempBuff;
  std::string str;
  void  transfer_encoding_chunked(FILE *file_ptr, size_t bytesRead);
  void handleMultiPartRequest();
  int simulateStartBody();
  size_t searchForBoundary(std::string boundary);
  int simulateStartChunk();
  void  readHoleChunkAtOnce(FILE *file_ptr, size_t bytesRead);
  const std::string getDataName();
  size_t totalReadBytes;
  // Redirection response
  std::string _redirect;

  // Redirection methods
  bool  _isRedirect() const;
  void  _setRedirect();

  // Timeout tracking
  timeval _requestStartTime;
  bool _requestStarted;
  
  // Timeout helper methods
  void _startRequestTimer();
  bool _isRequestTimedOut() const;
  double _getElapsedTime() const;
  void _handleRequestTimeout();

  // Getter for status code + message
  static const std::string& _getErrorMessage(long errnum);
  static const std::string* _getRedirectMessage(long statusCode);

  static std::map<long, std::string> create_status_map() {
    std::map<long, std::string> m;
    m[301] = "301 Moved Permanently";
    m[302] = "302 Found";
    m[303] = "303 See Other";
    m[304] = "304 Not Modified";
    m[307] = "307 Temporary Redirect";
    m[308] = "308 Permanent Redirect";
    m[400] = "400 Bad Request";
    m[401] = "401 Unauthorized";
    m[403] = "403 Forbidden";
    m[404] = "404 Not Found";
    m[405] = "405 Method Not Allowed";
    m[408] = "408 Request Timeout";
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
  
  // Public timeout methods
  bool isTimedOut() const;
  void resetTimeout();

};
#endif
