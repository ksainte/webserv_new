#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <csignal>
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
  _requestIsACGI(),
  _areHeadersSent(),
  _buffer(),
  _requestStartTime(),
  _requestStarted(false)
{
  // Clear the buffer//
  memset(_buffer, 0, sizeof(_buffer));
  memset(&_requestStartTime, 0, sizeof(_requestStartTime));
  LOG_DEBUG << "Connection created\n";
}

Connection::Connection(Searcher& searcher, Epoll& manager):
  _manager(&manager),
  _searcher(&searcher),
  _sockFd(-1),
  _event(),
  _tmpPathExt(),
  _rootPath(),
  _ErrResponse(),
  _listDir(),
  _previousLoc(),
  absPath(),
  _cgiEnv(),
  cgiPath(),
  envStorage(),
  env(),
  location(),
  _continueReadingFile(),
  _requestIsACGI(),
  _isExtensionSet(),
  _areHeadersSent(),
  file(),
  _buffer(),
  dataName(),
  _tempBuff(),
  str(),
  totalReadBytes(),
  _redirect(),
  _requestStartTime(),
  _requestStarted(false)
{
  memset(_buffer, 0, sizeof(_buffer));
  memset(&_requestStartTime, 0, sizeof(_requestStartTime));
  LOG_DEBUG << "Connection created\n";
}

Connection::Connection(const Connection& other):
  Request(other),
  _manager(other.getManager()),
  _searcher(other.getSearcher()),
  _sockFd(other.getSockFd()),
  location(),
  _continueReadingFile(),
  _requestIsACGI(),
  _areHeadersSent(),
  _buffer(),
  _requestStartTime(),
  _requestStarted(false)
{
  memset(&_requestStartTime, 0, sizeof(_requestStartTime));
  LOG_DEBUG << "Connection copied\n";
}

Connection& Connection::operator=(const Connection& other)
{
  if (this == &other)
    return *this;

  _manager = other.getManager();
  _searcher = other.getSearcher();
  _sockFd = other.getSockFd();
  _clientFd = getClientFd();

  _event = Event();
  _tmpPathExt.clear();
  _rootPath.clear();
  _ErrResponse.clear();
  _listDir.clear();
  _previousLoc.clear();
  absPath.clear();
  _cgiEnv.clear();
  cgiPath.clear();
  envStorage.clear();
  env.clear();
  location = NULL;
  _continueReadingFile = false;
  _requestIsACGI = false;
  _isExtensionSet = false;
  _areHeadersSent = false;
  file.close();
  memset(_buffer, 0, sizeof(_buffer));
  dataName.clear();
  _tempBuff.clear();
  str.clear();
  totalReadBytes = 0;
  _redirect.clear();
  memset(&_requestStartTime, 0, sizeof(_requestStartTime));
  _requestStarted = false;
  
  return *this;
}

Connection::~Connection()
{
  try
  {
    if (file.is_open())
    {
      file.close();
    }
  }
  catch (...)
  {
  }

  LOG_DEBUG << "Connection destroyed\n";
}


int Connection::getClientFd()
{
  return _clientFd;
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

  ssize_t maxBodySize = _defaultMaxBodySize;

  if (val && bodySize((*val)[0]) != -1)
    maxBodySize = bodySize((*val)[0]);

  if (requestBodySize > maxBodySize || requestBodySize > _defaultMaxBodySize)
    throw Exception(ErrorMessages::E_MAX_BODY_SIZE, 413);
}

bool Connection::_isPythonFile(const std::string& path)
{
  if (path.length() < 3)
    return false;

  if (path.length() >= 3 && path.substr(path.length() - 3) == ".py")
    return true;

  return false;
}

void Connection::tryCgi()
{
  // if (!_isPythonFile(_path))
  // {
  //   _requestIsACGI = false;
  //   return;
  // }

  location = _searcher->getLocation(_sockFd, _host, _path);
  if (!location)
    throw Exception(ErrorMessages::E_BAD_ROUTE, 404);
  const std::string prefix(location->getPrefix());//faut gere ca!
  const ConfigType::DirectiveValue* p = _searcher->findLocationDirective(_sockFd, "root", _host, prefix.c_str());
  if (!p || p[0].empty())
    throw Exception(ErrorMessages::E_BAD_ROUTE, 404);
  struct stat stats = {};
  cgiPath = (*p)[0];
  p = _searcher->findLocationDirective(_sockFd, "cgi_pass", _host, prefix.c_str());
  if (!p || p[0].empty())
    return ;
  // cgiPath.append("/");
  cgiPath.append((*p)[0]);
  stat(cgiPath.c_str(), &stats);
  if (!access(cgiPath.c_str(), X_OK))
  {
    if (!S_ISDIR(stats.st_mode))
    {
      _requestIsACGI = true;
      return ;
    }
    throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
  }
  throw Exception(ErrorMessages::E_FORBIDDEN, 403);
}

// if (access(cgiPath.c_str(), X_OK) == -1)
// throw Exception(ErrorMessages::E_FORBIDDEN, 403);
// if (S_ISDIR(stats.st_mode))
// throw Exception(ErrorMessages::E_BAD_REQUEST, 400);

// std::size_t found = (*p)[0].find(".");
// std::string str = (*p)[0].substr(found + 1);
// if (str.compare(_tmpPathExt) == 0)
// _requestIsACGI = true;

std::string Connection::getContentType()
{
  const std::size_t found = absPath.find(".");
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

void Connection::sendToCGI()
{
  int cgi_pid;
  char* arr[2];

  signal(SIGPIPE, SIG_IGN);
  cgi_pid = fork();

  if (cgi_pid < 0)
    throw Exception(ErrorMessages::E_FORK_FAILED, 500);


  arr[0] = const_cast<char*>(cgiPath.c_str());
  arr[1] = NULL;
  
  if (cgi_pid == 0)
  {
    if (_method == "POST")
      dup2(_clientFd, STDIN_FILENO);
    dup2(_clientFd, STDOUT_FILENO);    
    execve(cgiPath.c_str(), arr, env.data());
    perror("execve: ");
    exit(1);
  }

  int status;

  for (int i = 0; i < _defaultCgiTimeout; ++i)
  {
    pid_t result = waitpid(cgi_pid, &status, WNOHANG);
    
    if (result == cgi_pid)
    {
      signal(SIGPIPE, SIG_DFL);
      return ;
    }
      sleep(1);
  }

  // Timeout reached - kill the child process
  kill(cgi_pid, SIGKILL);

  // Wait for child to die
  wait(&status);

  signal(SIGPIPE, SIG_DFL);
  throw Exception(ErrorMessages::E_TIMEOUT, 408);
}

// void Connection::createMinGetEnv()
// {
//   envStorage.clear();
//   env.clear();
//   std::ostringstream s;
//   file.open(absPath.c_str(), std::ios::binary | std::ios::ate);
//   long lenght = file.tellg();
//   file.close();
//   s << lenght;
//   std::string contentLength(s.str());
//   std::string contentType = getContentType();
//   envStorage.push_back(std::string("CONTENT_LENGTH") + "=" + contentLength);
//   envStorage.push_back(std::string("CONTENT_TYPE") + "=" + contentType);
//   envStorage.push_back("QUERY_STRING=" + absPath);
// }

int Connection::prepareEnvForGetCGI()
{
  // createMinGetEnv();
  const ConfigType::CgiParams& p = location->getCgiParams();
  for (ConfigType::CgiParams::const_iterator it = p.begin(); it != p.end(); ++it)
    envStorage.push_back(it->first + "=" + it->second);
  discardDupEnvVar();
  for (size_t i = 0; i < envStorage.size(); ++i)
    env.push_back(const_cast<char*>(envStorage[i].c_str()));
  env.push_back(NULL);
  sendToCGI();
  _manager->unregisterEvent(_clientFd);
  close(_clientFd);
  setClientFd(-1);
  resetTimeout();
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
  // Start the request timeout timer
  _startRequestTimer();
  try
  {
    extractHeaders();
    _checkUriLen();
    storeHeaders();
	_checkInvalidUrlCharacters();
    _isMethodAllowed();
    if (_isRedirect())
    {
      _setRedirect();
      _manager->modifyEvent(EPOLLOUT, const_cast<Event*>(p));
      return;
    }

    if (/*!_isPythonFile(_path) &&*/ _method == "GET")
      _isPathValid();
    _checkBodySize();
    tryCgi();
  } 
  catch (Exception& e)
  {
    LOG_WARNING << e.what();
    handleError(e.errnum());
  }
  _manager->modifyEvent(EPOLLOUT, const_cast<Event*>(p));
}

void Connection::createMinPostEnv()
{
  envStorage.clear();
  env.clear();
  envStorage.push_back("PATH_INFO=" + cgiPath);
  envStorage.push_back("CONTENT_TYPE=" + _headers["content-type"]);
  envStorage.push_back("CONTENT_LENGTH=" + _headers["content-length"]);
}

void Connection::discardDupEnvVar()
{
  std::size_t i;
  std::size_t j;
  i = 0;
  j = 0;
  while (i < envStorage.size())
  {
    std::size_t found = envStorage[i].find("=");
    std::string sub = envStorage[i].substr(0, found);
    j = i;
    while (j + 1 < envStorage.size())
    {
      std::size_t found = envStorage[j + 1].find("=");
      std::string sub_next = envStorage[j + 1].substr(0, found);
      if (sub.compare(sub_next) == 0)
      {
        envStorage.erase(envStorage.begin() + j + 1);
        j--;
      }
      j++;
    }
    i++;
  }
}


void  Connection::transfer_encoding_chunked(FILE *file_ptr, size_t bytesRead)
{
  size_t chunkSize;
  size_t chunkSizeEnd;
  size_t chunkDataStart;
  size_t chunkDataEnd;
  size_t currentRecv;

  chunkSizeEnd = getChunkSizeEnd();
  chunkSize = getChunkSize(chunkSizeEnd);
  chunkDataStart = chunkSizeEnd + _headerEnd.size();
  chunkDataEnd = chunkDataStart + chunkSize + _headerEnd.size();

  totalReadBytes += fwrite(_tempBuff.data() + chunkDataStart , sizeof(char), bytesRead - chunkDataStart, file_ptr);

  while (bytesRead + _tempBuff.size() < chunkDataEnd)
  {
    memset(_tempBuff.data(), 0 , _tempBuff.size());
    currentRecv = recv(_clientFd, _tempBuff.data(), _tempBuff.capacity(), 0);
    bytesRead += currentRecv;
    totalReadBytes += fwrite(_tempBuff.data(), sizeof(char), currentRecv, file_ptr);
  }
  memset(_tempBuff.data(), 0 , _tempBuff.size());
  currentRecv = recv(_clientFd, _tempBuff.data(), chunkDataEnd - bytesRead, 0);
  totalReadBytes += fwrite(_tempBuff.data(), sizeof(char), currentRecv - _headerEnd.size() , file_ptr);
}


size_t Connection::getChunkSizeEnd()
{
  const std::vector<unsigned char>::const_iterator it = std::search(_tempBuff.begin(), _tempBuff.end(), _headerEnd.begin(), _headerEnd.end(), isEqual);

  if (it == _tempBuff.end())
  {
    throw Exception(ErrorMessages::E_HEADERS_END_NOT_FOUND, Exception::BAD_REQUEST);
  }

  size_t chunkSizeEnd = it - _tempBuff.begin();

  return (chunkSizeEnd);
}

size_t Connection::getChunkSize(size_t chunkSizeEnd)
{
  std::string chunk_size_str(_tempBuff.begin(), _tempBuff.begin() + chunkSizeEnd);

  std::stringstream ss(chunk_size_str);

  size_t chunkSize;

  ss >> std::hex >> chunkSize;

  if (ss.fail() || chunk_size_str.empty())
    throw Exception(ErrorMessages::E_HEADERS_END_NOT_FOUND, Exception::BAD_REQUEST);
  
  return (chunkSize);
}

int Connection::simulateStartChunk()
{
  size_t chunkSize;
  size_t chunkSizeEnd;
  size_t chunkDataStart;
  size_t chunkDataEnd;

  chunkSizeEnd = getChunkSizeEnd();
  chunkSize = getChunkSize(chunkSizeEnd);

  if (chunkSize == 0)
    return (0);
  
  chunkDataStart = chunkSizeEnd + _headerEnd.size();
  chunkDataEnd = chunkDataStart + chunkSize + _headerEnd.size();
  return (chunkDataEnd);
}


void Connection::readHoleChunkAtOnce(FILE *file_ptr, size_t bytesRead)
{
  size_t chunkSizeEnd;
  size_t chunkDataStart;

  chunkSizeEnd = getChunkSizeEnd();
  chunkDataStart = chunkSizeEnd + _headerEnd.size(); 
  totalReadBytes += fwrite(_tempBuff.data() + chunkDataStart , sizeof(char), bytesRead - chunkDataStart - _headerEnd.size(), file_ptr);
}


void Connection::getDataName()
{
  std::size_t found;
  HeaderIt it = _headers.find("transfer-encoding");

  dataName = _path;
  found = dataName.find("=");
  if (found == std::string::npos)
  {
    if (it != _headers.end())
      throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
    dataName = "";
    return ;
  }
  dataName = dataName.substr(found + 1);
}

FILE *Connection::prepareFileForWriting()
{
  FILE *file_ptr;

  getDataName();
  std::string uploadDir = "uploads/";
  uploadDir.append(dataName);
  file_ptr = fopen(uploadDir.c_str(), "wb");
  memset(_tempBuff.data(), 0 , _tempBuff.size());
  return (file_ptr);
}

void Connection::sendChunkedResponse()
{
  std::ostringstream oss;
  oss << "HTTP/1.1 201 Created\r\n"
      << "Content-Type: text/plain\r\n"
      << "Location: http://127.0.0.2:8080/uploads/"
      << dataName << "\r\n"
      << "Content-Length: 0\r\n"
      << "\r\n"; 
  send(_clientFd, oss.str().c_str(), oss.str().size(), 0);
}

void Connection::handleChunkedRequest()
{
  FILE *file_ptr;
	ssize_t bytesRead;
  size_t chunkDataEnd;

  _tempBuff.resize(10000);
  file_ptr = prepareFileForWriting();
  while ((bytesRead = recv(_clientFd, _tempBuff.data(), _tempBuff.capacity(), MSG_PEEK)))
  {
    if (bytesRead == static_cast<ssize_t>(-1))
      throw Exception(ErrorMessages::E_RECV, 500);
    chunkDataEnd = simulateStartChunk();
    if (!chunkDataEnd)
      break;
    memset(_tempBuff.data(), 0, _tempBuff.size());
    if (_tempBuff.capacity() < chunkDataEnd)
    {
      bytesRead = recv(_clientFd, _tempBuff.data(), _tempBuff.capacity(), 0);
      transfer_encoding_chunked(file_ptr, bytesRead);
      memset(_tempBuff.data(), 0, _tempBuff.size());
      continue;
    }
    bytesRead = recv(_clientFd, _tempBuff.data(), chunkDataEnd, 0);
    readHoleChunkAtOnce(file_ptr, bytesRead);
    memset(_tempBuff.data(), 0, _tempBuff.size());
  }
  fclose (file_ptr);
  std::clog << "totalReadBytes is " << totalReadBytes << "\n";
  sendChunkedResponse();
}

int Connection::simulateStartBody()
{
  recv(_clientFd, _tempBuff.data(), _tempBuff.capacity(), MSG_PEEK);

  const std::vector<unsigned char>::const_iterator it = std::search(_tempBuff.begin(), _tempBuff.end(), _headersEnd.begin(), _headersEnd.end(), isEqual);

  if (it == _tempBuff.end())
  {
    throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
  }

  size_t MetaDataBytesEnd = it - _tempBuff.begin();
  
  size_t BodyDataStart = MetaDataBytesEnd + _headersEnd.size();

  return BodyDataStart;
}


size_t Connection::searchForBoundary(std::string boundary)
{
  const std::vector<unsigned char>::const_iterator it = std::search(_tempBuff.begin(), _tempBuff.end(), boundary.begin(), boundary.end(), isEqual);

  if (it == _tempBuff.end())
  {
    return (0);
  }
  size_t boundaryPos = it - _tempBuff.begin();

  return boundaryPos;
}

std::string Connection::searchMetaData(size_t BodyDataStart)
{
  std::string filename = "filename=";

  const std::vector<unsigned char>::const_iterator itEnd  =  _tempBuff.begin() + BodyDataStart;
  const std::vector<unsigned char>::const_iterator it = std::search(_tempBuff.begin(), _tempBuff.begin() + BodyDataStart, filename.begin(), filename.end(), isEqual);
  if (it == itEnd)
  {
    throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
  }
  const std::vector<unsigned char>::const_iterator itStartMarks = std::find(it, itEnd, '"');
  const std::vector<unsigned char>::const_iterator itEndMarks = std::find(itStartMarks + 1, itEnd, '"');
  std::string name(itStartMarks + 1, itEndMarks);
  return (name);
}

std::string Connection::findBoundaryInHeaders()
{
  memset(_tempBuff.data(), 0 , _tempBuff.size());
  std::string headersBoundary;
  HeaderIt it = _headers.find("content-type");

  if (it == _headers.end())
    throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
  size_t found = _headers["content-type"].find("=");
  if (found == std::string::npos)
    throw Exception(ErrorMessages::E_UNSUPPORTED_MEDIA, 415);
  headersBoundary = _headers["content-type"].substr(found + 1);
  std::string boundary = "--";
  boundary.append(headersBoundary);
  return (boundary);
}

FILE *Connection::prepareFileAndSkipMetadata()
{
  FILE *file_ptr;
  size_t BodyDataStart;
  std::string uploadDir = "uploads/";

  BodyDataStart = simulateStartBody();
  getDataName();
  if (dataName == "")
    dataName = searchMetaData(BodyDataStart);
  uploadDir.append(dataName);
  file_ptr = fopen(uploadDir.c_str(), "wb");
  memset(_tempBuff.data(), 0 , _tempBuff.size());
  recv(_clientFd, _tempBuff.data(), BodyDataStart, 0);
  memset(_tempBuff.data(), 0 , _tempBuff.size());
  return (file_ptr);
}

std::string ResponseBody(std::string dataName)
{
    std::string body =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "    <title>upload</title>\n"
    "    <style>\n"
    "        body {\n"
    "            font-family: Arial, sans-serif;\n"
    "            margin: 40px;\n"
    "            background-color: #f4f4f4;\n"
    "            color: #333;\n"
    "            text-align: center;\n"
    "        }\n"
    "        h1 {\n"
    "            color: #d63384;\n"
    "        }\n"
    "        p {\n"
    "            line-height: 1.6;\n"
    "        }\n"
    "        .container {\n"
    "            background-color: #ffffff;\n"
    "            padding: 20px;\n"
    "            border-radius: 8px;\n"
    "            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);\n"
    "            display: inline-block;\n"
    "            max-width: 600px;\n"
    "        }\n"
    "        footer {\n"
    "            margin-top: 30px;\n"
    "            font-size: 0.9em;\n"
    "            color: #666;\n"
    "        }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <div class=\"container\">\n"
    "        <h1>Webserv</h1>\n"
    "        <p>File uploaded successfully</p>\n"
    "        <p>Your upload is available at Location: "
    "http://127.0.0.2:8080/uploads/"
    + dataName +
    "</p>\n"
    "    </div>\n"
    "</body>\n"
    "</html>\n";
    return (body);
}

void Connection::sendMultiPartResponse()
{
  std::string body;

  body = ResponseBody(dataName);
  std::ostringstream oss;
  oss << "HTTP/1.1 201 Created\r\n"
      << "Content-Type: text/html\r\n"
      << "Location: http://127.0.0.2:8080/uploads/"
      << dataName << "\r\n"
      << "Content-Length: " << body.size() << "\r\n"
      << "\r\n"
      << body;
  send(_clientFd, oss.str().c_str(), oss.str().size(), 0);
}

void Connection::handleMultiPartRequest()
{
  FILE *file_ptr;
	ssize_t bytesRead;
  size_t boundaryPos;
  std::string boundary;

  _tempBuff.resize(10000);
  boundary = findBoundaryInHeaders();
  file_ptr = prepareFileAndSkipMetadata();
  while ((bytesRead = recv(_clientFd, _tempBuff.data(), _tempBuff.capacity(), MSG_PEEK)))
  {
    if (bytesRead == static_cast<ssize_t>(-1))
      throw Exception(ErrorMessages::E_RECV, 500);
    boundaryPos = searchForBoundary(boundary);
    memset(_tempBuff.data(), 0 , _tempBuff.size());
    if (boundaryPos == 0)
    {
      bytesRead = recv(_clientFd, _tempBuff.data(), _tempBuff.capacity(), 0);
      totalReadBytes += fwrite(_tempBuff.data(), sizeof(char), bytesRead, file_ptr);
      memset(_tempBuff.data(), 0 , _tempBuff.size());
      continue;
    }
    bytesRead = recv(_clientFd, _tempBuff.data(), boundaryPos, 0);
    totalReadBytes += fwrite(_tempBuff.data(), sizeof(char), bytesRead - _headerEnd.size(), file_ptr);
    break;
  }
  if (boundaryPos == 0)
    throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
  fclose (file_ptr);
  std::clog << "totalReadBytes is " << totalReadBytes << "\n";
  sendMultiPartResponse();
}

void Connection::prepareEnvforPostCGI()
{
  std::ostringstream oss;
  createMinPostEnv();
  const ConfigType::CgiParams& params = location->getCgiParams();
  for (ConfigType::CgiParams::const_iterator it = params.begin(); it != params.end(); ++it)
    envStorage.push_back(it->first + "=" + it->second);
  discardDupEnvVar();
  for (size_t i = 0; i < envStorage.size(); ++i)
    env.push_back(const_cast<char*>(envStorage[i].c_str()));
  env.push_back(NULL);
  sendToCGI();
}


void Connection::isFileToDeleteValid(int *result)
{
  const std::string prefix(location->getPrefix());
  const ConfigType::DirectiveValue* p = _searcher->findLocationDirective(_sockFd, "root", _host, prefix.c_str());

  if (!p || p[0].empty())
    throw Exception(ErrorMessages::E_BAD_ROUTE, 404);
  struct stat stats = {};
  _rootPath = (*p)[0];
  _rootPath.append("/");
  // _rootPath.append(_path);
  _rootPath.append(_path.substr(strlen(prefix.c_str())));

  stat(_rootPath.c_str(), &stats);
  if (!access(_rootPath.c_str(), W_OK))
  {
    if (!S_ISDIR(stats.st_mode))
    {
      *result = remove(_rootPath.c_str());
      return ;
    }
    throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
  }
  throw Exception(ErrorMessages::E_FORBIDDEN, 403);
}


void Connection::prepareDeleteRequest()
{
  int result;
  
  isFileToDeleteValid(&result);
  if (result == 0)
  {
    const std::string body =
    "<html>"
    "<body>"
    "<h1>File deleted.</h1>"
    "</body>"
    "</html>";
  std::ostringstream oss;
  oss << "HTTP/1.1 200 OK\r\n"
      << "Content-Type: text/plain\r\n"
      << "Date: Fri, 21 Jun 2024 14:18:33 GMT\r\n"
      << "Content-Length: " << body.size() << "\r\n"
      << "\r\n"
      << body;
  send(_clientFd, oss.str().c_str(), oss.str().size(), 0);
  }
  else
    throw Exception(ErrorMessages::E_DELETE_FAIL, 500);
  
  _manager->unregisterEvent(_clientFd);
  close(_clientFd);
  setClientFd(-1);
  resetTimeout();
}

bool Connection::isNotEmpty(const Event* p)
{
  if (!_redirect.empty())
  {
    send(p->getFd(), _redirect.c_str(), _redirect.size(), 0);
    _manager->unregisterEvent(p->getFd());
    close(p->getFd());
    setClientFd(-1);
    resetTimeout();
    return false;
  }
  if (!_ErrResponse.empty())
  {
    send(p->getFd(), _ErrResponse.c_str(), _ErrResponse.size(), 0);
    _manager->unregisterEvent(p->getFd());
    close(p->getFd());
    setClientFd(-1);
    resetTimeout();
    return false;
  }
  if (!_listDir.empty())
  {
    send(p->getFd(), _listDir.c_str(), _listDir.size(), 0);
    _manager->unregisterEvent(p->getFd());
    close(p->getFd());
    setClientFd(-1);
    resetTimeout();
    return false;
  }
  return true;
}

void Connection::runProperPostFunction()
{
  HeaderIt it = _headers.find("transfer-encoding");

  if (_requestIsACGI)
    prepareEnvforPostCGI();
  else if (it != _headers.end() && it->second == "chunked")
    handleChunkedRequest();
  else
    handleMultiPartRequest();
  
  _manager->unregisterEvent(_clientFd);
  close(_clientFd);
  setClientFd(-1);
  resetTimeout();
}

int Connection::handleEvent(const Event* p, const unsigned int flags)
{
  if (_isRequestTimedOut())
  {
    _handleRequestTimeout();
    return 1; // Signal to epoll to close this connection
  }

  if (flags & EPOLLIN)
  {
    prepareResponse(p);
  }
  else if (flags & EPOLLOUT && isNotEmpty(p))
  {
    try
    {
      if (_method == "POST")
        runProperPostFunction();
      else if (_method == "DELETE")
        prepareDeleteRequest();
      else
      {
        if (_requestIsACGI)
          return (prepareEnvForGetCGI());
        readFILE();
      }
    }
    catch (Exception& e)
    {
      LOG_WARNING << e.what();
      handleError(e.errnum());
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

void Connection::findPathFinalExtension()
{
  std::size_t i;
  const std::string validExtension[] = {"cgi", "py", "php"};
  std::size_t found;
  _tmpPathExt = _path;
  found = _tmpPathExt.find(".");
  if (found == std::string::npos)
    return ;
  while (found !=std::string::npos)
  {
    _tmpPathExt = _tmpPathExt.substr(found + 1);
    found = _tmpPathExt.find(".");
  }
  if (_tmpPathExt.length() == 0)
  {
    return ;
  }
  _isExtensionSet = true;
  found = _path.find(_tmpPathExt);
  i = 0;
  while(i < validExtension->length())
  {
    if (_tmpPathExt == validExtension[i])
      _path.resize(found - 1);
    i++;
  }
}


void Connection::_isPathValid()
{

  // findPathFinalExtension();
  location = _searcher->getLocation(_sockFd, _host, _path);

  if (!location)
    throw Exception(ErrorMessages::E_BAD_ROUTE, 404);

  if (_isPythonFile(_path))
    return ;

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

  absPath.append("/");

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
  file.open(absPath.c_str(), std::ios::binary | std::ios::ate);
  int fileLenght = file.tellg();
  file.seekg(0, file.beg);
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
  // Check for timeout during file operations
  if (_isRequestTimedOut())
  {
    _handleRequestTimeout();
    return 1;
  } // Reset timeout when connection closes

  if (_areHeadersSent == false)
  {
    sendResponseHeaders();
    _areHeadersSent = true;
    return 0;
  }
  file.read(_buffer, sizeof(_buffer));
  if (file.gcount() > 0)
  {
    send(_clientFd, _buffer, file.gcount(), 0);
    memset(_buffer, 0, sizeof(_buffer));
    return 0;
  }
  file.close();
  _manager->unregisterEvent(_clientFd);
  close(_clientFd);
  setClientFd(-1);
  resetTimeout();
  _areHeadersSent = false;
  memset(_buffer, 0, sizeof(_buffer));
  
  return 0;
}

void Connection::_isHttpVersionSupported(const std::string& version)
{
  if (version == "HTTP/1.1" || version == "HTTP/1.0" || version == "HTTP/0.9")
    return;
  throw Exception(ErrorMessages::E_HTTP_VERSION, 400);
}

const std::string& Connection::_getErrorMessage(const long errnum)
{
  static const ConfigType::HttpStatusCode status = create_status_map();
  static const std::string unknown_error_str = "Unknown Error";

  const ConfigType::HttpStatusCodeIt it = status.find(errnum);
  if (it != status.end())
  {
    return it->second;
  }

  return unknown_error_str;
}

void Connection::_defaultErrorPage(const int errnum)
{
  std::string errval = _getErrorMessage(errnum);

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

void Connection::setSockFd(const int sockFd) { _sockFd = sockFd; }
void Connection::setClientFd(const int clientFd) { _clientFd = clientFd; }
Event* Connection::getEvent() { return &_event; }
int Connection::getSockFd() const { return _sockFd; }
Epoll* Connection::getManager() const { return _manager; }
Searcher* Connection::getSearcher() const { return _searcher; }

// Timeout implementation methods
void Connection::_startRequestTimer()
{
  if (!_requestStarted)
  {
    gettimeofday(&_requestStartTime, NULL);
    _requestStarted = true;
    LOG_DEBUG << "Request timer started for fd " << _clientFd << "\n";
  }
}

bool Connection::_isRequestTimedOut() const
{
  if (!_requestStarted)
    return false;

  timeval currentTime = {};
  gettimeofday(&currentTime, NULL);
  
  double elapsed = (currentTime.tv_sec - _requestStartTime.tv_sec) + 
                   (currentTime.tv_usec - _requestStartTime.tv_usec) / 1000000.0;
  
  // Use CGI timeout for CGI requests, regular timeout for others
  const double timeoutLimit = _requestIsACGI ? _defaultCgiTimeout : _defaultRequestTimeout;
  
  return elapsed > timeoutLimit;
}

double Connection::_getElapsedTime() const
{
  if (!_requestStarted)
    return 0.0;

  timeval currentTime = {};
  gettimeofday(&currentTime, NULL);
  
  return (currentTime.tv_sec - _requestStartTime.tv_sec) + 
         (currentTime.tv_usec - _requestStartTime.tv_usec) / 1000000.0;
}

void Connection::_handleRequestTimeout()
{
  LOG_WARNING << "Request timeout after " << _getElapsedTime() << " seconds for fd " << _clientFd << "\n";
  
  // Send 408 Request Timeout response
  handleError(408);
  
  // If there's an error response, send it
  if (!_ErrResponse.empty())
  {
    send(_clientFd, _ErrResponse.c_str(), _ErrResponse.size(), 0);
  }
  
  // Clean up the connection
  _manager->unregisterEvent(_clientFd);
  close(_clientFd);
  setClientFd(-1);
  
  // Reset timeout state
  _requestStarted = false;
  memset(&_requestStartTime, 0, sizeof(_requestStartTime));
}

bool Connection::_isRedirect() const
{
  return _searcher->findLocationDirective(_sockFd, "return", _host, _path) != NULL;
}

const std::string* Connection::_getRedirectMessage(const long code)
{
  static const ConfigType::HttpStatusCode status = create_status_map();

  const ConfigType::HttpStatusCodeIt it = status.find(code);

  if (it != status.end())
  {
    return &it->second;
  }

  return NULL;
}

void Connection::_setRedirect()
{
  const ConfigType::DirectiveValue* redirect =
    _searcher->findLocationDirective(_sockFd, "return", _host, _path);

  const long statusCode = std::strtol((*redirect)[0].c_str(), NULL, 10);

  const std::string* redirectMessage =
    _getRedirectMessage(statusCode);

  std::stringstream ss;
  ss << "HTTP/1.1 ";

  redirectMessage ?
  ss << *redirectMessage : ss << statusCode;

  ss << "\r\n";
  ss << "Location: " << (*redirect)[1] << "\r\n";
  ss << "\r\n";

  _redirect = ss.str();
}

void  Connection::_checkUriLen() const {

  const size_t MAX_URI_LENGTH = 8192;

  if (_path.length() > MAX_URI_LENGTH) {
    throw Exception(ErrorMessages::E_URI_TOO_LONG, 414);
  }
}

void Connection::_checkBodySizeMismatch(size_t size) const
{
  const HeaderIt it = _headers.find("content-length");
  
  if (it != _headers.end())
  {
    ssize_t declaredSize = bodySize(it->second);
    if (declaredSize != -1 && static_cast<size_t>(declaredSize) != size)
    {
      LOG_WARNING << "Content-Length mismatch: declared " << declaredSize 
                  << ", actual " << size << "\n";
      throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
    }
  }
}

  // RFC 3986 defines valid URI characters
  // Reserved characters: :/?#[]@!$&'()*+,;=
  // Unreserved characters: alphanumeric and -._~

void Connection::_checkInvalidUrlCharacters() const
{
  const std::string& path = _path;
  
  for (size_t i = 0; i < path.length(); ++i)
  {
    unsigned char c = static_cast<unsigned char>(path[i]);
    
    if (c < 32 || c == 127)
    {
      LOG_WARNING << "Invalid URL character detected: control character 0x" 
                  << std::hex << static_cast<int>(c) << " at position " << i << "\n";
      throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
    }
    
    if (c == '<' || c == '>' || c == '"' || c == '|' || c == '\\' || c == '^')
    {
      LOG_WARNING << "Potentially problematic URL character detected: 0x" 
                  << std::hex << static_cast<int>(c) << " at position " << i << "\n";
      throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
    }
  }
  
  for (size_t i = 0; path.length() >= 2 && i < path.length() - 2; ++i)
  {
    if (path[i] == '%')
    {
      if (i + 2 >= path.length() || 
          !std::isxdigit(static_cast<unsigned char>(path[i + 1])) ||
          !std::isxdigit(static_cast<unsigned char>(path[i + 2])))
      {
        LOG_WARNING << "Invalid percent encoding detected at position " << i << "\n";
        throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
      }
    }
  }
  
  if (path.find("%%") != std::string::npos)
  {
    LOG_WARNING << "Double percent encoding detected in URL\n";
    throw Exception(ErrorMessages::E_BAD_REQUEST, 400);
  }
}

bool Connection::isTimedOut() const
{
  return _isRequestTimedOut();
}

void Connection::resetTimeout()
{
  _requestStarted = false;
  memset(&_requestStartTime, 0, sizeof(_requestStartTime));
  LOG_DEBUG << "Request timer reset for fd " << _clientFd << "\n";
}

std::string Connection::getCurrentDate()
{
  time_t now = time(0);
  struct tm tm = *gmtime(&now);
  char time_buffer[256];
  strftime(time_buffer, sizeof(time_buffer), "%a, %d %b %Y %H:%M:%S GMT", &tm);
  return std::string(time_buffer);
}