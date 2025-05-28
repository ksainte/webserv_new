#include <sys/socket.h>
#include <sys/stat.h>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include "../inc/Connection.hpp"
#include "../inc/Event.hpp"
#include "../inc/Epoll.hpp"
#include "../inc/Logger.hpp"
#include "../inc/Searcher.hpp"

Connection::Connection():
  _manager(NULL),
  _searcher(NULL),
  _sockFd(-1),
  _clientFd(-1)
{
  LOG_DEBUG << "Connection created\n";
}

Connection::Connection(Searcher& searcher, Epoll& manager):
  _manager(&manager),
  _searcher(&searcher),
  _sockFd(-1),
  _clientFd(-1)
{
  LOG_DEBUG << "Connection created\n";
}

Connection::Connection(const Connection& other):
  Request(other),
  _manager(other.getManager()),
  _searcher(other.getSearcher()),
  _sockFd(other.getSockFd()),
  _clientFd(other.getClientFd()),
  _headers(other.getHeaders())
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
  _clientFd = other.getClientFd();
  _headers = other.getHeaders();
  return *this;
}

Connection::~Connection()
{
  LOG_DEBUG << "Connection destroyed\n";
}

int Connection::handleEvent(const Event* p, const unsigned int flags)
{
  if (flags & EPOLLIN && read(p->getFd()) == 0)
  {
    setHeaders();
    _manager->modifyEvent(EPOLLOUT, const_cast<Event*>(p));
    return 0;
  }
  if (flags & EPOLLOUT)
  {
    _manager->unregisterEvent(p->getFd());
    sendResponse();
  }
  return 0;
}

bool Connection::sendResponse() const
{
  std::string root_directory;

  std::string host;
  const HeaderIt it = _headers.find("host");
  if (it != _headers.end())
    host = it->second;

  const LocationBlock* location = _searcher->getLocation(_sockFd, host, _path.c_str());
  if (!location)
  {
    std::clog << "\nroute is not valid\n";
    return false;
  }
  const std::string prefix(location->getPrefix());

  const ConfigType::DirectiveValue* p = _searcher->findLocationDirective(_sockFd, "root", host, prefix.c_str());
  if (!p || p[0].empty())
  {
    std::clog << "root is not valid\n";
    return false;
  }

  struct stat stats = {};
  std::string absPath = (*p)[0];
  stat(absPath.c_str(), &stats);

  if (!access(absPath.c_str(), F_OK))
  {
    if (!S_ISDIR(stats.st_mode))
    {
      send_to_cgi(absPath);
      return true;
    }
    else
      absPath.append(_path.substr(strlen(prefix.c_str())));

    stat(absPath.c_str(), &stats);

    if (!access(absPath.c_str(), F_OK))
    {
      if (!S_ISDIR(stats.st_mode))
      {
        send_to_cgi(absPath.c_str());
        return true;
      }
    }

    p = _searcher->findLocationDirective(_sockFd, "index", host, prefix.c_str());
    if (p)
    {
      for (ConfigType::DirectiveValueIt it = p->begin(); it != p->end(); ++it)
      {
        std::string tmp(absPath);
        tmp.append("/");
        tmp.append(*it);

        stat(tmp.c_str(), &stats);

        if (!access(absPath.c_str(), F_OK)
          && !S_ISDIR(stats.st_mode))
        {
          send_to_cgi(tmp);
          return true;
        }
      }
    }
  }

  const std::string e501 =
    "HTTP/1.0 501 Not Implemented\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 19\r\n"
    "Connection: close\r\n"
    "Last-Modified: Mon, 23 Mar 2020 02:49:28 GMT\r\n"
    "Expires: Sun, 17 Jan 2038 19:14:07 GMT\r\n"
    "Date: Mon, 23 Mar 2020 04:49:28 GMT\n\n"
    "501 Not Implemented";

  send(_sockFd, e501.c_str(), e501.size(), 0);
  return false;
}

bool Connection::setHeaders()
{
  std::istringstream iss(_rawBytes);
  std::string line;

  if (std::getline(iss, line))
  {
    std::istringstream lineStream(line);
    lineStream >> _method >> _path >> _version;
  }

  while (std::getline(iss, line) && line[0] != '\r')
  {
    size_t pos = line.find(':');
    if (pos != std::string::npos)
    {
      std::string key = line.substr(0, pos);
      std::string value = line.substr(pos + 1);

      // Trim whitespace
      // Shoudl also trim whitespaces before key and after value
      key.erase(key.find_last_not_of(' ') + 1);
      value.erase(0, value.find_first_not_of(' '));
      value.erase(value.find_last_not_of('\r') + 1);

      // Convert key to lowercase for case-insensitive comparison
      for (size_t i = 0; i < key.size(); ++i)
      {
        key[i] = static_cast<char>(std::tolower(key[i]));
      }
      _headers[key] = value;
    }
  }
  return true;
}

int Connection::send_to_cgi(const std::string& absPath) const
{
  char* arr[2] = {const_cast<char*>(absPath.c_str()), NULL};

  setenv("QUERY_STRING", absPath.c_str(), 1);

  const int pid = fork();

  if (pid != 0)
  {
    close(_clientFd);
    return 0;
  }

  close(1);
  dup2(_clientFd, 1);

  const int result = execv("./cgi-bin/GET.cgi", arr);

  if (result < 0)
  {
    std::cout << "result false\n";
  }
  exit(1);
}

void Connection::setEvent()
{
  _event = Event(_clientFd, this);
}

const std::map<std::string, std::string>&
Connection::getHeaders() const { return _headers; };
void Connection::setSockFd(int sockFd) { _sockFd = sockFd; }
void Connection::setClientFd(int clientFd) { _clientFd = clientFd; };
Event* Connection::getEvent() { return &_event; }
int Connection::getSockFd() const { return _sockFd; }
int Connection::getClientFd() const { return _clientFd; }
Epoll* Connection::getManager() const { return _manager; }
Searcher* Connection::getSearcher() const { return _searcher; }
