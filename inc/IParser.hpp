#ifndef IPARSER_HPP
#define IPARSER_HPP
#include <string>

class IParser
{
protected:
  virtual bool allowedMethod(const std::string& method) const = 0;
  virtual bool validPath(const std::string& path) const = 0;
  virtual bool supportedVersion(const std::string& version) const = 0;
};

#endif
