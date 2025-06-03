#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
#include <exception>
#include <cstddef>

class Exception: public std::exception
{
  int _errnum;
  const char* _msg;
public:
  enum Error
  {
    BAD_REQUEST = 400
  };
  Exception(const char* msg = NULL, int errnum = -1);
  int errnum() const;
  char* what();
};

#endif
