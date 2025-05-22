#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "../inc/Connection.hpp"
#include "../inc/Event.hpp"
#include "../inc/EventManager.hpp"
#include "../inc/Logger.hpp"
#include "../inc/Searcher.hpp"

Connection::Connection(): 
_manager(NULL),
_searcher(NULL),
_sockFd(-1),
_clientFd(-1)
{LOG_DEBUG << "Connection created\n";}

Connection::Connection(Searcher& searcher, EventManager& manager): 
Request(),
_manager(&manager),
_searcher(&searcher),
_sockFd(-1),
_clientFd(-1)
{LOG_DEBUG << "Connection created\n";}

Connection::Connection(const Connection& other):
Request(other),
_manager(other.getManager()),
_searcher(other.getSearcher()),
_sockFd(other.getSockFd()),
_clientFd(other.getClientFd()),
_headers(other.getHeaders())
{LOG_DEBUG << "Connection copied\n";}

Connection& Connection::operator=(const Connection& other) {
	if (this == &other)
		return *this;
	_manager = other.getManager();
	_searcher = other.getSearcher();
	_sockFd = other.getSockFd();
	_clientFd = other.getClientFd();
	_headers = other.getHeaders();
	return *this;
}

Connection::~Connection() {LOG_DEBUG << "Connection destroyed\n";}

int Connection::handleEvent(const Event* p, int flags) 
{
	if ((flags & EPOLLIN) && read(p->getFd()) == 0)
  {
		setHeaders();
		_manager->modifyEvent(uint32_t(EPOLLOUT), const_cast<Event*>(p));
		return 0;
	}
	else if (flags & EPOLLOUT)
  {
		_manager->unregisterEvent(p->getFd());
		
		int pid = fork();
		//child
		if (pid == 0) 
			sendResponse();
	
		close(p->getFd());
	}	
	return 0;
}

bool Connection::sendResponse() const
{
  std::string root_directory;
  std::string value;
	const ConfigType::DirectiveValue* p;

	std::string host("");
	HeaderIt it = _headers.find("host");
	if (it != _headers.end())
		host = it->second;

	const char *route = _searcher->getLocationPrefix(_sockFd, host.c_str(), _path.c_str());
	if (!route)
  {
    std::clog << "\nroute is not valid\n";
    return (0);
  }

  p = _searcher->findLocationDirective(_sockFd, "root", value.c_str(), route);
	if (!p || !(*p)[0].size())
	{
		std::clog << "root is not valid\n";
		return 0;
	}

	struct stat stats;
	std::string absPath = (*p)[0];
	stat(absPath.c_str(), &stats);

	if (!access(absPath.c_str(), F_OK))
	{
		if (!S_ISDIR(stats.st_mode))
			send_to_cgi(absPath);
		else
			absPath.append((_path.substr(strlen(route))));

		stat(absPath.c_str(), &stats);
		
		if (!access(absPath.c_str(), F_OK))
			if (!S_ISDIR(stats.st_mode))
				send_to_cgi(absPath.c_str());
		
	}

	if ((access(absPath.c_str(), F_OK) != 0))
	{
		std::string e501 =
		"HTTP/1.0 501 Not Implemented\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 19\r\n"
    "Connection: close\r\n"
    "Last-Modified: Mon, 23 Mar 2020 02:49:28 GMT\r\n"
    "Expires: Sun, 17 Jan 2038 19:14:07 GMT\r\n"
    "Date: Mon, 23 Mar 2020 04:49:28 GMT\n\n" 
    "501 Not Implemented";

    send(_sockFd, e501.c_str(), e501.size(), 0);
    return(0);
	}

  // p = _searcher.findLocationDirective(sock_file_descriptor, "index", value.c_str(), route);
  // if (p)
  // {
  //   std::clog << "\nINDEX_DIRECTORY is:\n";
  //   for (ConfigType::DirectiveValueIt it = (*p).begin(); it != (*p).end(); ++it) 
  //   {
  //       std::cout << *it << "\n";//checker si c est directory ou file
  //       std::string temp;
  //       temp = root_directory;
  //       temp.append("/");
  //       temp.append(*it);
  //       std::cout << "New is :\n" << temp << "\n";

  //       char arr1[temp.length() + 1]; 
  //       memset(arr1,0, temp.length());
  //         for (long unsigned int x = 0; x < sizeof(arr1); x++) { 
  //           arr1[x] = temp[x]; 
  //         }

  //         if ((access(arr1, F_OK) < 0)) 
  //           continue;
  //         else {
  //           if ((method_type.compare("GET") == 0))
  //           {
  //             std::cout << "\nReceived GET method\n";
  //             send_to_cgi(sock_file_descriptor, arr1);
  //           }
  //         }
  //   }
  // }
  // if (!p)
  // {
  //   std::clog << "index directory is not valid\n";
  //   return (0);
  // }
  // std::clog << "\nindex is not EXISTING\n";

  return 0;
}

bool	Connection::setHeaders()
{
	std::istringstream iss(_rawBytes);
	std::string line;

	if (std::getline(iss, line)) 
	{
		std::istringstream lineStream(line);
		lineStream >> _method >> _path >> _version;
	}

	while (std::getline(iss, line) && line != "\r") 
	{
		size_t pos = line.find(":");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);

			// Trim whitespace
			// Shoudl also trim whitespaces before key and after value
			key.erase(key.find_last_not_of(" ") + 1);
			value.erase(0, value.find_first_not_of(" "));
			value.erase(value.find_last_not_of("\r") + 1);

			// Convert key to lowercase for case-insensitive comparison
			for (size_t i = 0; i < key.size(); ++i) 
			{
				key[i] = std::tolower(key[i]);
			}
			_headers[key] = value;
		}
	}
	return true;
}

int Connection::send_to_cgi(const std::string& absPath) const
{
  close(1);
  dup2(_clientFd, 1);

  char *arr[2] = {const_cast<char*>(absPath.c_str()), NULL};

  setenv("QUERY_STRING", absPath.c_str(), 1);
  int result;

  result = execv("./cgi-bin/GET.cgi", arr);

  if (result < 0)
  {
    std::cout << "result false\n";
  }
  return 0;
}

int Connection::isDirectoryExists(const char *path) const
{
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
}

void Connection::setEvent() {
	_event = Event(_clientFd, this);
}

const std::map<std::string, std::string>&
Connection::getHeaders() const {return _headers;};
void	Connection::setSockFd(int sockFd) {_sockFd = sockFd;}
void	Connection::setClientFd(int clientFd) {_clientFd = clientFd;};
Event*	Connection::getEvent() {return &_event;}
int	Connection::getSockFd() const {return _sockFd;}
int Connection::getClientFd() const {return _clientFd;}
EventManager* Connection::getManager () const {return _manager;}
Searcher* Connection::getSearcher() const {return _searcher;}