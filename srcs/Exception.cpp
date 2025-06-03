#include "../inc/Exception.hpp"
#include "../inc/Logger.hpp"

Exception::Exception(const char* msg, int errnum)
  : _errnum(errnum), _msg(msg)
{
  LOG_DEBUG << "RequestException created\n";
}

char* Exception::what()
{
  return const_cast<char *>(_msg);
}

int Exception::errnum() const
{
  return _errnum;
}
