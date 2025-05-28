#include "../inc/Request.hpp"
#include "../inc/Logger.hpp"
#include <cstring>
#include <sys/socket.h>
#include <cstdlib>
#include <string.h>



int	Request::read(int fd)
{
	size_t max_len = sizeof(_buff2) - _offset;
	ssize_t bytesRead = recv(fd, _buff2 + _offset, max_len, 0);

	memcpy(_buff, _buff2, bytesRead);

	if (bytesRead == -1)
		throw std::runtime_error("Error Reading from Client");


	//le seul probleme ici c est que si le buffer est tres petit
	//dans le _rawbyte total il peut y avoir le rnrn, mais dans le current
	//buff il peut y'avoir que < 4 char come nrn, le strstre trouve pas  et
	//ca segfault, une solution c est jsp trop!
	_offset += bytesRead;
    if (_offset == sizeof(_buff))
	{
		_headersLen += _offset;
		std::cout << "\nheaders len is "<< _headersLen << "\n";
		_rawBytes.append(_buff, sizeof(_buff));
		_offset = 0;
		_pos = _rawBytes.find("\r\n\r\n");//le total
		if (_pos != std::string::npos)
		{
			std::cout << "\npos is " << _pos << "\n";// le current
			char* needle = strstr(_buff, "\r\n\r\n");//tu vas chercher le premier newline
			if (!needle)
				std::clog << "needle not found\n";
			_offsetNewLine = needle - _buff + 4;  // on est sur le newline
			// std::clog << "\nclog is "<< _offsetNewLine;
			// std::clog << "\n body is:\n"<<  _buff + _offsetNewLine;
			// std::clog << "\nbodyfinish--------------------\n";
			memcpy(_buff3, _buff2 + _offsetNewLine, sizeof(_buff) - _offsetNewLine);
			// std::clog << "\n buff3 is:\n"<<  _buff3;
			return (0);//arrete de lire!
		}
		memset(_buff2, 0 , _BUFFSIZE);
		memset(_buff, 0 , _BUFFSIZE);
		return (1);
    }
	_headersLen += _offset;
	std::cout << "\nHeaders len is "<< _headersLen << "\n";
	_rawBytes.append(_buff, 0, strlen(_buff));
	_pos = _rawBytes.find("\r\n\r\n");
	std::cout << "\npos is " << _pos << "\n";
	char* needle = strstr(_buff, "\r\n\r\n");//tu vas chercher le premier newline
	_offsetNewLine = needle - _buff + 4;  // on est sur le newline
	// std::clog << "\nclog is "<< _offsetNewLine;
	// std::clog << "\n body :\n"<<  _buff + _offsetNewLine;
	// std::cout << "\nbodyfinish--------------------\n";
	memcpy(_buff3, _buff2 + _offsetNewLine, sizeof(_buff) - _offsetNewLine);
	// std::clog << "\n buff3 is:\n"<<  _buff3;

	return (0);//fin des headers
}

Request::Request(): _offsetNewLine(0), _offset(0), _headersLen(0), _pos(0)
{
	std::memset(_buff, 0, _BUFFSIZE);
	std::memset(_buff2, 0, _BUFFSIZE);

	LOG_DEBUG << "Request created";
}

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


// long lSize;
// char * buffer;
// size_t result;

// fseek (pFile , 0 , SEEK_END);
// lSize = ftell (pFile);
// rewind (pFile);
// printf("\nlSize is : \n%ld", lSize);
// buffer = (char*)malloc(lSize + 1);
// result = fread (buffer, 1, lSize, pFile);
// buffer[lSize + 1] = '\0';
// printf("\nresult is : \n%ld", result);
// int i = 0;
// while(buffer[i])
// {
// 	printf("%02x", buffer[i]);
// 	i++;
// }
// printf("\ni is : \n%d", i);