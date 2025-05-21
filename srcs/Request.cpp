#include "../inc/Request.hpp"
#include "../inc/Logger.hpp"
#include <cstring>
#include <sys/socket.h>
#include <cstdlib>
#include <string.h>

int	Request::read(int fd) 
{
	size_t max_len = sizeof(_buff) - _offset;
	ssize_t bytesRead = recv(fd, _buff + _offset, max_len, 0);
	_buff[max_len] = '\0';

	if (bytesRead == -1)
		throw std::runtime_error("Error Reading from Client");
	_offset += bytesRead;
    if (_offset == sizeof(_buff))
	{
		_rawBytes.append(_buff, sizeof(_buff));
		memset(_buff, 0 , _BUFFSIZE);
		_offset = 0;
		size_t pos = _rawBytes.find("\r\n\r\n");
		if (pos != std::string::npos)
		{
			return (0);
		}
		return (1);
    }
	_rawBytes.append(_buff, 0, strlen(_buff));
	return (0);
}

Request::Request(): _offset(0) {LOG_DEBUG << "Request created";}

Request::Request(const Request& other): 
_offset(other.getOffset()), 
_rawBytes(other.getRawBytes()),
_method(other.getMethod()),
_path(other.getPath()),
_version(other.getVersion())
{

	memcpy(_buff, other.getBuff(), _BUFFSIZE);
	LOG_DEBUG << "Request copied";
}

Request& Request::operator=(const Request& other) {

	if (this == &other)
		return *this;
	
	_offset = other.getOffset();
	_rawBytes = other.getRawBytes();
	_method = other.getMethod();	
	_path = other.getPath();
	_version = other.getVersion();
	memcpy(_buff, other.getBuff(), _BUFFSIZE);
	
	LOG_DEBUG << "Request copy assigned";
	return *this;
}

Request::~Request() {LOG_DEBUG << "Request destructed";}

off_t Request::getOffset() const {return _offset;}

const std::string& Request::getMethod() const {return _method;}

const std::string& Request::getPath() const {return _path;}

const std::string& Request::getVersion() const {return _version;}

const char*	Request::getBuff() const {return _buff;}

const std::string& Request::getRawBytes() const {return _rawBytes;}

int	Request::getBuffSize() const {return _BUFFSIZE;}
