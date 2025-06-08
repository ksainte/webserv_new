#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>
#include "../inc/Connection.hpp"
#include "../inc/Event.hpp"
#include "../inc/Epoll.hpp"
#include "../inc/Exception.hpp"
#include "../inc/Logger.hpp"
#include "../inc/Searcher.hpp"
#include "../inc/utils.hpp"

Connection::Connection():
  _manager(NULL),
  _searcher(NULL),
  _sockFd(-1),
  location(),
  _continueReadingFile(),
  _areHeadersSent(),
  _buffer()
{
  // Clear the buffer//
  memset(_buffer, 0, sizeof(_buffer));
  LOG_DEBUG << "Connection created\n";
}

Connection::Connection(Searcher& searcher, Epoll& manager):
  _manager(&manager),
  _searcher(&searcher),
  _sockFd(-1),
  location(),
  _continueReadingFile(),
  _areHeadersSent(),
  _buffer()
{
  memset(_buffer, 0, sizeof(_buffer));
  LOG_DEBUG << "Connection created\n";
}

Connection::Connection(const Connection& other):
  Request(other),
  _manager(other.getManager()),
  _searcher(other.getSearcher()),
  _sockFd(other.getSockFd()),
  location(),
  _continueReadingFile(),
  _areHeadersSent(),
  _buffer()
{
  LOG_DEBUG << "Connection copied\n";
}

Connection& Connection::operator=(const Connection& other)
{
  if (this == &other)
    return *this;
  _manager = other.getManager();
  _searcher = other.getSearcher();
  _sockFd = other.getSockFd();
  _headers = other.getHeaders();
  return *this;
}

Connection::~Connection()
{
  try
  {
    if (MyReadFile.is_open())
    {
      MyReadFile.close();
    }
  }
  catch (...)
  {
    // Ignore exceptions during destruction
  }

  LOG_DEBUG << "Connection destroyed\n";
}

void Connection::_checkBodySize() const
{
  const HeaderIt it = _headers.find("content-length");

  ssize_t requestBodySize = 0;

  if (it != _headers.end() && bodySize(it->second) == -1)
    requestBodySize = _defaultMaxBodySize;
  else if (it != _headers.end() && bodySize(it->second) != -1)
    requestBodySize = bodySize(it->second);

  const ConfigType::DirectiveValue* val =
    _searcher->findLocationDirective(_sockFd, "client_max_body_size", _host, _path);

  if (val && bodySize((*val)[0]) < requestBodySize)
    throw Exception(ErrorMessages::E_MAX_BODY_SIZE, 400);
}

bool Connection::isRequestaCGI()
{
  // if (_path.find('?') == std::string::npos) return false;
  location = _searcher->getLocation(_sockFd, _host, _path);
  if (!location)
    throw Exception(ErrorMessages::E_BAD_ROUTE, 404);
  const std::string prefix(location->getPrefix());//faut gere ca!
  const ConfigType::DirectiveValue* p = _searcher->findLocationDirective(_sockFd, "root", _host, prefix.c_str());
  if (!p || p[0].empty())
    return false;
  struct stat stats = {};
  cgiPath = (*p)[0];
  p = _searcher->findLocationDirective(_sockFd, "cgi_pass", _host, prefix.c_str());
  if (!p || p[0].empty())
    return false;
  cgiPath.append("/");
  cgiPath.append((*p)[0]);
  stat(cgiPath.c_str(), &stats);
  if (!access(cgiPath.c_str(), X_OK))
  {
    if (!S_ISDIR(stats.st_mode))
    {
      return true;
    }
    return false;
  }
  return false;
}

std::string Connection::getContentType()
{
  std::size_t found = absPath.find(".");
  if (found == std::string::npos)
    return NULL;
  std::string str = absPath.substr(found + 1);
  for (int i = 0; str[i]; i++)
    str[i] = tolower(str[i]);
  if (str.compare("jpeg") == 0 || str.compare("jpg") == 0)
    return "image/jpeg";
  else if (str.compare("gif") == 0)
    return "image/gif";
  else if (str.compare("html") == 0 || str.compare("htm") == 0)
    return "text/html";
  else if (str.compare("mp4") == 0)
    return "video/mp4";
  else if (str.compare("mvk") == 0)
    return "video/mkv";
  else
    return "text/plain";
}

void Connection::sendToGetCGI()
{
  int pid;
  char* arr[2];

  pid = fork();
  arr[0] = const_cast<char*>(cgiPath.c_str());
  arr[1] = NULL;
  std::cout << "cgipath is " << cgiPath << "\n";
  if (pid == 0)
  {
    close(1);
    dup2(_clientFd, 1);
    execve(cgiPath.c_str(), arr, env.data());
    perror("execve: ");
    exit(1); //faut le changer?
  }
  else if (pid < 0)
  {
    close(_clientFd);
    _manager->unregisterEvent(_clientFd);
    throw Exception(ErrorMessages::E_FORK_FAILED, 404);
  }
  wait(NULL);
  _manager->unregisterEvent(_clientFd);
  close(_clientFd);
}

void Connection::createMinGetEnv()
{
  envStorage.clear();
  env.clear();
  std::ostringstream s;
  MyReadFile.open(absPath.c_str(), std::ios::binary | std::ios::ate);
  int fileLenght = MyReadFile.tellg();
  MyReadFile.close();
  s << fileLenght;
  std::string contentLength(s.str());
  std::string contentType = getContentType();
  envStorage.push_back(std::string("CONTENT_LENGTH") + "=" + contentLength);
  envStorage.push_back(std::string("CONTENT_TYPE") + "=" + contentType);
  envStorage.push_back("QUERY_STRING=" + absPath);
}

int Connection::prepareEnvForGetCGI()
{
  createMinGetEnv();
  const ConfigType::CgiParams& p = location->getCgiParams();
  for (ConfigType::CgiParams::const_iterator it = p.begin(); it != p.end(); ++it)
    envStorage.push_back(it->first + "=" + it->second);
  for (size_t i = 0; i < envStorage.size(); ++i)
    env.push_back(const_cast<char*>(envStorage[i].c_str()));
  env.push_back(NULL);
  sendToGetCGI();
  return (0);
}

void Connection::_isMethodAllowed() const
{
  const ConfigType::DirectiveValue* methods =
    _searcher->findLocationDirective(_sockFd, "method", _host, _path);

  // By default, all methods are allowed
  if (!methods) return;

  ConfigType::DirectiveValueIt it = methods->begin();

  for (; it != methods->end() && _method != *it; ++it)
  {
  }

  if (it == methods->end())
    throw Exception(ErrorMessages::E_BAD_METHOD, 405);
}

void Connection::prepareResponse(const Event* p)
{
  try
  {
    extractHeaders();
    storeHeaders();
    _isMethodAllowed();
    if (_method == "GET")
      _isPathValid();
    _checkBodySize();
    if (isRequestaCGI())
      _requestIsACGI = true;
    if (!_requestIsACGI && _method == "POST")
      throw Exception(ErrorMessages::E_BAD_METHOD, 405);
  } 
  catch (Exception& e)
  {
    LOG_WARNING << e.what();
    handleError(e.errnum());
  }
  _manager->modifyEvent(EPOLLOUT, const_cast<Event*>(p));
}

void Connection::sendToPostCGI()
{
  int pid;

  pid = fork();
  char *arr[2];
  arr[0] = const_cast<char*>(cgiPath.c_str());
  arr[1] = NULL;
  if (pid == 0)
  {
    dup2(_clientFd, STDIN_FILENO);
    dup2(_clientFd, STDOUT_FILENO);
    execve(cgiPath.c_str(), arr, env.data());
    perror("execve: ");
    exit(1);
  }
  else if (pid < 0)
  {
    close(_clientFd);
    _manager->unregisterEvent(_clientFd);
    throw Exception(ErrorMessages::E_FORK_FAILED, 404);
  }
  wait(NULL);
  _manager->unregisterEvent(_clientFd);
  close(_clientFd);
}

void Connection::createMinPostEnv()
{
  envStorage.clear();
  env.clear();
  envStorage.push_back("PATH_INFO=" + cgiPath);
  envStorage.push_back("CONTENT_TYPE=" + _headers["content-type"]);
  envStorage.push_back("CONTENT_LENGTH=" + _headers["content-length"]);
}

void Connection::prepareEnvforPostCGI()
{
  createMinPostEnv();
  const ConfigType::CgiParams& params = location->getCgiParams();
  for (ConfigType::CgiParams::const_iterator it = params.begin(); it != params.end(); ++it)
    envStorage.push_back(it->first + "=" + it->second);
  for (size_t i = 0; i < envStorage.size(); ++i)
    env.push_back(const_cast<char*>(envStorage[i].c_str()));
  env.push_back(NULL);
  sendToPostCGI();
}

void Connection::prepareDeleteRequest(const Event* p)
{
  std::string str = "/home/ks19/Apps/19/webserv_Kev_branch_working_21_May";
  str.append(_path);
  std::clog << _path;
  int result = remove(str.c_str());
  if (result == 0)
  {
    const std::string e501 =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html; charset=UTF-8\r\n"
      "Date: Fri, 21 Jun 2024 14:18:33 GMT\r\n"
      "Content-Length: 1234\r\n\r\n"
      "<html>"
      "<body>"
      "<h1>File file.html deleted.</h1>"
      "</body>"
      "</html>";
    send(_clientFd, e501.c_str(), e501.size(), 0);
  }
  std::clog << result;
  _manager->unregisterEvent(p->getFd());
  close(p->getFd());
}

bool Connection::isNotEmpty(const Event* p)
{
  if (!_ErrResponse.empty())
  {
    send(p->getFd(), _ErrResponse.c_str(), _ErrResponse.size(), 0);
    _manager->unregisterEvent(p->getFd());
    close(p->getFd());
    return false;
  }
  if (!_listDir.empty())
  {
    send(p->getFd(), _listDir.c_str(), _listDir.size(), 0);
    _manager->unregisterEvent(p->getFd());
    close(p->getFd());
    return false;
  }
  return true;
}

int Connection::handleEvent(const Event* p, const unsigned int flags)
{
  if (flags & EPOLLIN)
  {
    prepareResponse(p);
  }
  else if (flags & EPOLLOUT && isNotEmpty(p))
  {
    if (_method == "POST")
      prepareEnvforPostCGI();
    else if (_method == "DELETE")
      prepareDeleteRequest(p);
    else
    {
      if (_requestIsACGI)
        return (prepareEnvForGetCGI());
      readFILE();
    }
  }
  return 0;
}

bool Connection::_checkDefaultFileAccess(const std::string& prefix)
{
  const ConfigType::DirectiveValue* index =
    _searcher->findLocationDirective(_sockFd, "index", _host, prefix.c_str());

  if (!index) return false;

  for (ConfigType::DirectiveValueIt it = index->begin(); it != index->end(); ++it)
  {
    std::string tmp(absPath);
    tmp.append("/");
    tmp.append(*it);

    if (!access(tmp.c_str(), R_OK)
      && !isDir(tmp.c_str()))
    {
      absPath = tmp;
      return true;
    }
  }

  return false;
}

void Connection::_isPathValid()
{
  location = _searcher->getLocation(_sockFd, _host, _path);

  if (!location)
    throw Exception(ErrorMessages::E_BAD_ROUTE, 404);

  const std::string prefix(location->getPrefix());

  const ConfigType::DirectiveValue* root =
    _searcher->findLocationDirective(_sockFd, "root", _host, prefix);

  if (!root || root->empty())
    throw Exception(ErrorMessages::E_BAD_PATH, 404);

  // absPath is the vector first item
  absPath = (*root)[0];

  // if absPath is not already not valid
  // we send a 404 not found error
  if (access(absPath.c_str(), R_OK))
    throw Exception(ErrorMessages::E_BAD_PATH, 404);

  // If absPath is a valid path in the filesystem
  // we send the file
  if (!isDir(absPath.c_str())) return;

  // If not we append the uri to absPath
  absPath.append(_path.substr(strlen(prefix.c_str())));

  // If absPath + uri is a valid file
  // we send it
  if (prefix != _path
    && !access(absPath.c_str(), F_OK)
    && !isDir(absPath.c_str()))
    return;

  const ConfigType::DirectiveValue* autoindex =
    _searcher->findLocationDirective(_sockFd, "autoindex", _host, _path);

  if (isDir(absPath.c_str()) && autoindex && (*autoindex)[0] == "true")
  {
    _listDir = listDir(absPath);
    return;
  }

  // We check default file access
  // until one is valid, or we reach vector end
  if (_checkDefaultFileAccess(prefix)) return;

  throw Exception(ErrorMessages::E_BAD_PATH, 404);
}

void Connection::sendResponseHeaders()
{
  MyReadFile.open(absPath.c_str(), std::ios::binary | std::ios::ate);
  int fileLenght = MyReadFile.tellg();
  MyReadFile.seekg(0, MyReadFile.beg);
  std::string contentType = getContentType();
  std::ostringstream headers;
  headers << "HTTP/1.1 200 OK\r\n"
    << "Content-Length: " << fileLenght << "\r\n"
    << "Content-Type: " << contentType << "\r\n"
    << "Connection: close\r\n"
    << "Last-Modified: Mon, 23 Mar 2020 02:49:28 GMT\r\n"
    << "Expires: Sun, 17 Jan 2038 19:14:07 GMT\r\n\r\n";
  std::string headers_buff = headers.str();
  send(_clientFd, headers_buff.data(), headers_buff.size(), 0);
}

int Connection::readFILE()
{
  if (_areHeadersSent == false)
  {
    sendResponseHeaders();
    _areHeadersSent = true;
    return 0;
  }
  MyReadFile.read(_buffer, sizeof(_buffer));
  if (MyReadFile.gcount() > 0)
  {
    send(_clientFd, _buffer, MyReadFile.gcount(), 0);
    memset(_buffer, 0, sizeof(_buffer));
    return 0;
  }
  MyReadFile.close();
  _manager->unregisterEvent(_clientFd);
  close(_clientFd);
  _areHeadersSent = false;
  memset(_buffer, 0, sizeof(_buffer));
  std::clog << "\nEnd of file!!\n";
  return 0;
}

void Connection::_isHttpVersionSupported(const std::string& version)
{
  if (version == "HTTP/1.1" || version == "HTTP/1.0" || version == "HTTP/0.9")
    return;
  throw Exception(ErrorMessages::E_HTTP_VERSION, 400);
}

const std::string& Connection::getErrorMessage(const int errnum)
{
  static const std::map<int, std::string> http_status_codes = create_status_map();
  static const std::string unknown_error_str = "Unknown Error";

  std::map<int, std::string>::const_iterator it = http_status_codes.find(errnum);
  if (it != http_status_codes.end())
  {
    return it->second;
  }

  return unknown_error_str;
}

void Connection::_defaultErrorPage(const int errnum)
{
  std::string errval = getErrorMessage(errnum);

  const std::string body =
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "<title>Error</title>\n"
    "<style>\n"
    "    body {\n"
    "        width: 35em;\n"
    "        margin: 0 auto;\n"
    "        font-family: Tahoma, Verdana, Arial, sans-serif;\n"
    "    }\n"
    "</style>\n"
    "</head>\n"
    "<body>\n"
    "<h1>An error occurred.</h1>\n"
    "<p>Sorry, the page you are looking for is currently unavailable.<br/>\n"
    "Please try again later.</p>\n"
    "<p><em>Faithfully yours, webserv.</em></p>\n"
    "<hr>\n"
    "<center><h1>" + errval + "</h1></center>\n"
    "</body>\n"
    "</html>";

  std::ostringstream res;
  res << "HTTP/1.1 " << errval << "\r\n"
    << "Content-Length: " << body.size() << "\r\n"
    << "Content-Type: text/html\r\n"
    << "Connection: close\r\n\r\n"
    << body;

  _ErrResponse = res.str();
}

void Connection::handleError(const int errnum)
{
  const ConfigType::ErrorPage* errorPages;

  if (location && !location->getErrorPages()->empty())
    errorPages = location->getErrorPages();
  else
    errorPages = _searcher->getDefaultServer(_sockFd, _host).getErrorPages();

  const ConfigType::DirectiveValue* root =
    _searcher->findLocationDirective(_sockFd, "root", _host, _path);

  if (!root || errorPages->empty())
  {
    _defaultErrorPage(errnum);
    return;
  }

  const ConfigType::ErrorPageIt it = errorPages->find(errnum);

  if (it == errorPages->end())
  {
    _defaultErrorPage(errnum);
    return;
  }

  std::string absPath = (*root)[0];
  absPath += "/";
  absPath += it->second;

  if (isDir(absPath.c_str()) || access(absPath.c_str(), R_OK))
  {
    _defaultErrorPage(errnum);
    return;
  }

  std::fstream fs(absPath.c_str());

  if (fs.fail())
  {
    _defaultErrorPage(errnum);
    return;
  }

  std::stringstream ss;
  ss << fs.rdbuf();
  std::string body = ss.str();
  size_t contentLength = body.length();

  std::stringstream contentLengthStream;
  contentLengthStream << contentLength;
  std::string contentLengthStr = contentLengthStream.str();

  _ErrResponse =
    "HTTP/1.1 400 Bad Request\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: " + contentLengthStr + "\r\n"
    "\r\n"
    + body;
}

void Connection::setEvent()
{
  _event = Event(_clientFd, this);
}

void Connection::setSockFd(int sockFd) { _sockFd = sockFd; }
void Connection::setClientFd(int clientFd) { _clientFd = clientFd; }
Event* Connection::getEvent() { return &_event; }
int Connection::getSockFd() const { return _sockFd; }
Epoll* Connection::getManager() const { return _manager; }
Searcher* Connection::getSearcher() const { return _searcher; }
