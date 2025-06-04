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
  _clientFd(other.getClientFd())
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
  if (flags & EPOLLIN)
  {
    try
    {
      extractHeaders(_clientFd);
      storeHeaders();
    }
    catch (Exception& e)
    {
      LOG_WARNING << e.what();
      handleError(e.errnum());
    }

    const HeaderIt it = _headers.find("host");
    if (it != _headers.end())
      _host = it->second;

    // setEnv();
    _manager->modifyEvent(EPOLLOUT, const_cast<Event*>(p));
    return 0;
  }
  if (flags & EPOLLOUT)
  {
    if (!_ErrResponse.empty())
    {
      send(_clientFd, _ErrResponse.c_str(), _ErrResponse.size(), 0);
      return 1;
    }

    if (_path == "/favicon.ico")
    {
      _manager->unregisterEvent(p->getFd());
      close(p->getFd());
      return 0;
    }

    if (_method == "POST")
    {
      std::string line;

      line = _headers["content-type"];
      std::string key_1 = "content-type=";
      key_1.append(line);
      const char* ct = key_1.c_str();
      line = _headers["content-length"];
      std::string key_2 = "content-length=";
      key_2.append(line);
      const char* cl = key_2.c_str();

      int pid;

      char* env[] =
      {
        const_cast<char*>(ct),
        const_cast<char*>(cl),
        NULL
      };

      pid = fork();

      if (pid == 0)
      {
        dup2(_clientFd, STDIN_FILENO);
        dup2(_clientFd, STDOUT_FILENO);
        execve("cgi.py", (char*[]){"cgi.py", NULL}, env);
        perror("execve: ");
        exit(1);
      }
      wait(NULL);
      _manager->unregisterEvent(p->getFd());
      close(p->getFd());
    }
    if (_method == "DELETE")
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
    else
    {
        sendResponse();
    }
  }
  return 0;
}

bool Connection::sendResponse()
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
      send_to_cgi(absPath.c_str());
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

        if (!access(tmp.c_str(), F_OK)
          && !S_ISDIR(stats.st_mode))
        {
          send_to_cgi(tmp.c_str());
          return true;
        }
      }
    }
  }

  _manager->unregisterEvent(_clientFd);
  const std::string e501 =
    "HTTP/1.0 501 Not Implemented\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 19\r\n"
    "Connection: close\r\n"
    "Last-Modified: Mon, 23 Mar 2020 02:49:28 GMT\r\n"
    "Expires: Sun, 17 Jan 2038 19:14:07 GMT\r\n"
    "Date: Mon, 23 Mar 2020 04:49:28 GMT\n\n"
    "501 Not Implemented";
    

  send(_clientFd, e501.c_str(), e501.size(), 0);
  close(_clientFd);
  return false;
}

char *get_content_type(char *filename)
{
  char *file_path = strtok(filename, ".");
  char *file_extension;
  while (file_path != NULL) { // find the file extension
    file_extension = file_path;
    file_path      = strtok(NULL, " ");
  }

  // sets file extension to lowercase in order to compare strings
  for (int i = 0; file_extension[i]; i++) {
    file_extension[i] = tolower(file_extension[i]);
  }

  // comparing the strings to match with its corresponding type
  if ((strcmp(file_extension, "jpeg") == 0) ||
      (strcmp(file_extension, "jpg") == 0)) {
    return "image/jpeg";
      }
  else if (strcmp(file_extension, "gif") == 0) {
    return "image/gif";
  }
  else if ((strcmp(file_extension, "html") == 0) ||
           (strcmp(file_extension, "htm") == 0)) {
    return "text/html";
           }
  else if (strcmp(file_extension, "mp4") == 0)
  {
    return "video/mp4";
  }
  else if (strcmp(file_extension, "mvk") == 0)
  {
    return "video/mkv";
  }
  else {
    return "text/plain";
  }
}

int Connection::send_to_cgi(const char * absPath)
{
  static int flag;

  if (MyReadFile.gcount() == 0 && flag == 0)
  {
    MyReadFile.open( absPath, std::ios::binary | std::ios::ate);
    std::clog << MyReadFile.tellg() << "\n";
    int fileLenght = MyReadFile.tellg();
    MyReadFile.seekg(0, MyReadFile.beg);
    char *contentType = get_content_type((char*)absPath);

    std::ostringstream headers;
    headers << "HTTP/1.1 200 OK\r\n"
        << "Content-Length: " << fileLenght << "\r\n"
        << "Content-Type: " << contentType << "\r\n"
        << "Connection: keep-alive\r\n"
        << "Last-Modified: Mon, 23 Mar 2020 02:49:28 GMT\r\n"
        << "Expires: Sun, 17 Jan 2038 19:14:07 GMT\r\n\r\n";
    std::string headers_buff = headers.str();

    send(_clientFd, headers_buff.data(), headers_buff.size(), 0);
    flag = 1;
    return (0);
  }

  MyReadFile.read (_buffer, sizeof(_buffer));

  if (MyReadFile.gcount() > 0)
  {
    // std::clog << "\nSent!\n";
    send(_clientFd, _buffer, MyReadFile.gcount(), 0);
    memset(_buffer, 0, sizeof(_buffer));
    // delete[] buffer;
    return 0;
  }
  else
  {
    std::clog << "\nEnd of file!!\n";
    MyReadFile.close();
    _manager->unregisterEvent(_clientFd);
    close(_clientFd);
    flag = 0;
    memset(_buffer, 0, sizeof(_buffer));
    return 0;
  }


}


// int Connection::send_to_cgi(const std::string& absPath) const
// {
//   char* arr[2] = {const_cast<char*>(absPath.c_str()), NULL};
//
//   setenv("QUERY_STRING", absPath.c_str(), 1);
//
//   const int pid = fork();
//
//   if (pid != 0)
//   {
//     close(_clientFd);
//     return 0;
//   }
//
//   close(1);
//   dup2(_clientFd, 1);
//
//   const int result = execv("./cgi-bin/GET.cgi", arr);
//
//   if (result < 0)
//   {
//     std::cout << "result false\n";
//   }
//   exit(1);
// }

bool Connection::supportedVersion(const std::string& version) const
{
  return version == "HTTP/1.1" || version == "HTTP/1.0" || version == "HTTP/0.9";
}

bool Connection::validPath(const std::string& path) const
{
  const LocationBlock* loc = _searcher->getLocation(_clientFd, _host, path);
  return loc != NULL;
}

bool Connection::allowedMethod(const std::string& method) const
{
  const ConfigType::DirectiveValue* allowedMethods =
    _searcher->findLocationDirective(_clientFd, "method", _host, _path);

  // Means all methods are allowed by default
  if (!allowedMethods) return true;

  const ConfigType::DirectiveValueIt it =
    std::find(allowedMethods->begin(), allowedMethods->end(), method);

  return it != allowedMethods->end();
}

const std::string& Connection::getErrorMessage(const int errnum) {

  static const std::map<int, std::string> http_status_codes = create_status_map();
  static const std::string unknown_error_str = "Unknown Error";

  std::map<int, std::string>::const_iterator it = http_status_codes.find(errnum);
  if (it != http_status_codes.end()) {
    return it->second;
  }

  return unknown_error_str;
}

int Connection::handleError(const int errnum) {

  const ConfigType::DirectiveValue* root =
    _searcher->findServerDirective(_sockFd, "root", _host);

  if (root)
  {
    const ConfigType::ErrorPage& errorPages =
      _searcher->getDefaultServer(_sockFd, _host).getErrorPages();

    const ConfigType::ErrorPageIt it = errorPages.find(errnum);
    if (it != errorPages.end())
    {
      std::string absPath = (*root)[0];
      absPath += "/";
      absPath += it->second;
      std::fstream fs(absPath.c_str());
      if (fs.good())
      {
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
      return 1;
    }
  }

  std::string errval = getErrorMessage(errnum);

  _ErrResponse = "<!DOCTYPE html>\n"
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
              "<p><em>Faithfully yours, nginx.</em></p>\n"
              "<hr>\n"
              "<center><h1>" + errval + "</h1></center>\n"
              "</body>\n"
              "</html>";
  LOG_DEBUG << ErrorMessages::E_CONN_CLOSED;
  return true;
}


void Connection::setEvent()
{
  _event = Event(_clientFd, this);
}

void Connection::setSockFd(int sockFd) { _sockFd = sockFd; }
void Connection::setClientFd(int clientFd) { _clientFd = clientFd; }
Event* Connection::getEvent() { return &_event; }
int Connection::getSockFd() const { return _sockFd; }
int Connection::getClientFd() const { return _clientFd; }
Epoll* Connection::getManager() const { return _manager; }
Searcher* Connection::getSearcher() const { return _searcher; }
