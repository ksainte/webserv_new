#include "../inc/Request.hpp"
#include "../inc/Logger.hpp"
#include <cstring>
#include <sys/socket.h>
#include <cstdlib>
#include "../inc/Exception.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/constants/SuccessMessages.hpp"

const int Request::_buffSize = 10;

void Request::storeHeaders()
{
  std::istringstream iss(std::string(_buf.begin(), _buf.end()));

  if (!iss)
    throw Exception(ErrorMessages::E_ISS_CREATE, Exception::BAD_REQUEST);

  iss.exceptions(std::istringstream::failbit | std::istringstream::badbit);

  std::string line;
  std::getline(iss, line);

  std::istringstream lineStream(line);

  if (!lineStream)
    throw Exception(ErrorMessages::E_ISS_CREATE, Exception::BAD_REQUEST);

  lineStream.exceptions(std::istringstream::failbit | std::istringstream::badbit);
  lineStream >> _method >> _path >> _version;

  while (std::getline(iss, line) && line[0] != '\r')
  {
    size_t pos = line.find(':');
    if (pos == std::string::npos)
      throw Exception(ErrorMessages::E_BAD_HEADER, Exception::BAD_REQUEST);

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
  LOG_DEBUG << SuccessMessages::HEADERS_STORED;
}

void Request::extractHeaders(const int fd)
{
  ssize_t bytesRead = 0;

  bytesRead = recv(fd, _buf.data(), _buf.capacity(), MSG_PEEK);

  if (bytesRead == -1)
    throw Exception(ErrorMessages::E_RECV, Exception::BAD_REQUEST);

  const std::vector<unsigned char>::const_iterator it =
    std::search(_buf.begin(), _buf.end(),
                _headersEnd.begin(), _headersEnd.end(), isEqual);

  if (it == _buf.end())
  {
    if (bytesRead == _buffSize)
      throw Exception(ErrorMessages::E_HEADERS_TOO_LONG, Exception::BAD_REQUEST);
    throw Exception(ErrorMessages::E_HEADERS_END_NOT_FOUND, Exception::BAD_REQUEST);
  }

  _offset = it - _buf.begin() + _headersEnd.size();
  bytesRead = recv(fd, _buf.data(), _offset, 0);

  if (bytesRead == -1)
    throw Exception(ErrorMessages::E_RECV, Exception::BAD_REQUEST);
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
  _host(other.getHost()),
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
  _host = other.getHost();
  _buf = other.getBuf();
  _headers = other.getHeaders();

  LOG_DEBUG << "Request copy assigned";
  return *this;
}

Request::~Request()
{
  LOG_DEBUG << "Request destructed";
}

bool Request::isEqual(const unsigned char a, const unsigned char b)
{
  return a == b;
}

const std::string Request::_headersEnd("\r\n\r\n");

const std::string Request::_headerEnd("\r\n");

const std::map<std::string, std::string>&
Request::getHeaders() const { return _headers; }

size_t Request::getOffset() const { return _offset; }

const std::string& Request::getMethod() const { return _method; }

const std::string& Request::getPath() const { return _path; }

const std::string& Request::getVersion() const { return _version; }

const std::string& Request::getHost() const { return _host; }

const std::vector<unsigned char>& Request::getBuf() const { return _buf; }
