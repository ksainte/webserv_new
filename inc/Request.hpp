#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <string>
#include <vector>
#include <map>

class Request
{
protected:

  typedef std::map<std::string, std::string> Headers;
  typedef std::map<std::string, std::string>::const_iterator HeaderIt;

  static bool  isEqual(unsigned char a, unsigned char b);

  size_t _offset;
  std::string _method;
  std::string _path;
  std::string _version;

  std::vector<unsigned char> _buf;
  Headers _headers;
  static const std::string _headersEnd;
  static const std::string _headerEnd;
  static const int _buffSize = 1024;
  static const int _maxHeadersLen = 1024;

public:
  Request();
  virtual ~Request();
  Request(const Request& other);
  Request& operator=(const Request& other);

  const std::string& getMethod() const;
  const std::string& getPath() const;
  const std::string& getVersion() const;
  const std::vector<unsigned char>& getBuf() const;
  size_t getOffset() const;


  int extractHeaders(int fd);
  bool storeHeaders();

  const Headers&
  getHeaders() const;

};
#endif
