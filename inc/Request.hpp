#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <string>
#include <vector>
#include <map>

class Request
{
protected:

  enum Error
  {
    E_RECV = 1,
    E_HEADERS_END_NOT_FOUND = 2,
    E_ISS = 3,
  };

  typedef std::map<std::string, std::string> Headers;
  typedef std::map<std::string, std::string>::const_iterator HeaderIt;

  static bool isEqual(unsigned char a, unsigned char b);

  size_t _offset;
  std::string _method;
  std::string _path;
  std::string _version;
  std::string _host;

  std::vector<unsigned char> _buf;
  Headers _headers;
  static const std::string _headersEnd;
  static const std::string _headerEnd;
  static const int _buffSize;
  static const int _maxHeadersLen = 1024;

public:
  Request();
  virtual ~Request();
  Request(const Request& other);
  Request& operator=(const Request& other);

  const std::string& getMethod() const;
  const std::string& getPath() const;
  const std::string& getVersion() const;
  const std::string& getHost() const;
  const std::vector<unsigned char>& getBuf() const;
  size_t getOffset() const;

  void extractHeaders(int fd);
  void storeHeaders();

  const Headers&
  getHeaders() const;
};
#endif
