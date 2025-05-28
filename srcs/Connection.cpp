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
#include <sys/wait.h>


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


// void Connection::setEnv()
// {
// 	std::string line;

// 	line = _headers["content-type"];
// 	std::string key_1 = "content-type:";
// 	key_1.append(line);
// 	std::clog << "\n" << key_1;
// 	line = _headers["content-length"];
// 	std::string key_2 = "content-length:";
// 	key_2.append(line);
// 	std::clog << "\n" << key_2;
// 	// size_t pos = line.find(":");
// 	// if (pos != std::string::npos) {
// 	// 	std::string key = line.substr(0, pos);
// 	// 	std::string value = line.substr(pos + 1);

// }


void Connection::continueReadingBody(int fdWriteSide, int clientSockFd)
{
	memset(_buff2, 0 , _BUFFSIZE);
	_offset = 0;
	size_t max_len = sizeof(_buff2) - _offset;

	ssize_t bytesRead;

	while (bytesRead = recv(clientSockFd, _buff2 + _offset, max_len, 0))
	{

		if (bytesRead == -1)
			throw std::runtime_error("Error Reading from Client");


		_offset += bytesRead;
		if (_offset == sizeof(_buff))
		{
			_headersLen += _offset;
			std::cout << "\nheaders len is "<< _headersLen << "\n";
			if (write(fdWriteSide, &_buff2, sizeof(_buff2)) == -1)
			{
				perror("write: ");
				return ;
			}
			_offset = 0;
			memset(_buff2, 0 , _BUFFSIZE);
			continue;
		}
		_headersLen += _offset;
		std::clog << "\nHeaders len is "<< _headersLen << "\n";
		if (write(fdWriteSide, &_buff2, sizeof(_buff2)) == -1)
		{
			perror("write: ");
			return ;
		}
		std::clog << "\n End of Reading!!!!!!\n";
		return ;
	}
}



int Connection::handleEvent(const Event* p, int flags) 
{
	if ((flags & EPOLLIN) && read(p->getFd()) == 0)
  {
		setHeaders();
		// setEnv();
		_manager->modifyEvent(uint32_t(EPOLLOUT), const_cast<Event*>(p));
		return 0;
	}
	else if (flags & EPOLLOUT)
  {
		// std::clog << "\n\nrequest is :\n"<< _rawBytes << "\n";
		std::clog << "\nCURRENT METHOD IS :" << _method << "\n";
		_manager->unregisterEvent(p->getFd());

		std::string line;

		line = _headers["content-type"];
		std::string key_1 = "content-type=";
		key_1.append(line);
		// std::clog << "\n" << key_1;
		const char *ct = key_1.c_str();
		line = _headers["content-length"];
		size_t content_length = atoi(line.c_str());
		std::string key_2 = "content-length=";
		key_2.append(line);
		// std::clog << "\n" << key_2;
		const char *cl = key_2.c_str();

		int pid;

		if (_method == "POST")
		{
			char *env[] = 
			{
				(char*)ct,
				(char*)cl,
				NULL
			};

			int fd[2];

			if (pipe(fd) == -1)
			{
				perror("pipe: ");
				return 1;
			}

			if (write(fd[1], &_buff3, sizeof(_buff) - _offsetNewLine) == -1)
			{
				perror("write: ");
				return 1;
			}
			if (_headersLen -_pos - 4 < content_length)
				continueReadingBody(fd[1], p->getFd());
			// 	std::cout << "\naaaaaaaaaa\n";


			pid = fork();

			if (pid == 0)
			{
				dup2(fd[0], STDIN_FILENO);
				close(fd[1]);
				dup2(_clientFd, STDOUT_FILENO);
				execve("cgi.py", (char*[]){"cgi.py", NULL}, env);
				perror("execve: ");
				exit(1);
			}
		
			close(fd[0]);
			close(fd[1]);
		}
		else
		{
			pid = fork();
			// wait(NULL);
			if (pid == 0) 
			{

				sendResponse();
				exit(1);
			}
		}
	
		close(p->getFd());
	}	
	return 0;
}

bool Connection::sendResponse() const
{
  std::string root_directory;
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

  p = _searcher->findLocationDirective(_sockFd, "root", host.c_str(), route);
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
		
		p = _searcher->findLocationDirective(_sockFd, "index", host.c_str(), route);
		if (p) {
			for (ConfigType::DirectiveValueIt it = p->begin(); it != p->end(); ++it) {
				
				std::string tmp(absPath);
				tmp.append("/");
				tmp.append(*it);

				stat(tmp.c_str(), &stats);

				if (!access(absPath.c_str(), F_OK)
					&& !S_ISDIR(stats.st_mode))
					send_to_cgi(tmp);
			}
		}
	}

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
  exit(1);
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