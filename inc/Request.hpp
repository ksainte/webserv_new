#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <string>
#include <sys/types.h>
# define _BUFFSIZE 3000

class Request
{
	public:

	Request();
	~Request();
	Request(const Request& other);
	Request& operator=(const Request& other);

	const std::string&	getMethod()		const;
	const std::string&	getPath()			const;
	const std::string&	getVersion()	const;	
	const std::string&	getRawBytes()	const;
	off_t 							getOffset() 	const;
	const char*					getBuff() 		const;
	int									getBuffSize() const;

	int		read(int fd);

	protected:
	
	// static const int _BUFFSIZE = 1000;
	char 				_buff[_BUFFSIZE];
	unsigned char 				_buff2[_BUFFSIZE];
	unsigned char 				_buff3[_BUFFSIZE];
	size_t 			_offsetNewLine;
	off_t 		_offset;
	size_t 				_headersLen;
	size_t 		_pos;
	std::string _rawBytes;
	std::string _method;
	std::string _path;
	std::string _version;
};
#endif