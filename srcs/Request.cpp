#include "../inc/Request.hpp"
#include "../inc/Logger.hpp"
#include <cstring>
#include <sys/socket.h>
#include <cstdlib>

const std::string Request::_headersEnd("\r\n\r\n");
const std::string Request::_headerEnd("\r\n");

bool Request::isEqual(const unsigned char a, const unsigned char b)
{
  return a == b;
}

bool Request::storeHeaders()
{
  std::istringstream iss(std::string(_buf.begin(), _buf.end()));
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

int Request::extractHeaders(const int fd)
{
  ssize_t bytesRead = 0;

  bytesRead = recv(fd, _buf.data(), _buf.capacity(), MSG_PEEK);

  if (bytesRead == -1) return 1;

  const std::vector<unsigned char>::const_iterator it =
    std::search(_buf.begin(), _buf.end(),
      _headersEnd.begin(), _headersEnd.end(), isEqual);

  if (it == _buf.end()) return 1;

  _offset = bytesRead - (it - _buf.begin()) + _headersEnd.size();

  bytesRead = recv(fd, _buf.data(), bytesRead - _offset, 0);

  if (bytesRead == -1) return 1;

  return 0;
}

Request::Request(): _offset(0)
{
  _buf.resize(_buffSize);
  LOG_DEBUG << "Request created";
}

Request::Request(const Request& other):
  _offset(other.getOffset()),
  _method(other.getMethod()),
  _path(other.getPath()),
  _version(other.getVersion()),
  _buf(other.getBuf()),
  _headers(other.getHeaders())
{
  LOG_DEBUG << "Request copied";
}

Request& Request::operator=(const Request& other)
{
  if (this == &other)
    return *this;

  _offset = other.getOffset();
  _method = other.getMethod();
  _path = other.getPath();
  _version = other.getVersion();
  _buf = other.getBuf();
  _headers = other.getHeaders();

  LOG_DEBUG << "Request copy assigned";
  return *this;
}

Request::~Request()
{
  LOG_DEBUG << "Request destructed";
}

const std::map<std::string, std::string>&
Request::getHeaders() const { return _headers; }

size_t Request::getOffset() const { return _offset; }

const std::string& Request::getMethod() const { return _method; }

const std::string& Request::getPath() const { return _path; }

const std::string& Request::getVersion() const { return _version; }

const std::vector<unsigned char>& Request::getBuf() const { return _buf; }