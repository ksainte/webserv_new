#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <string>
#include <sys/types.h>
class Request
{
	public:

	Request();
	virtual ~Request();
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
	
	static const int _BUFFSIZE = 1000;
	char 				_buff[_BUFFSIZE];
	off_t 			_offset;
	std::string _rawBytes;
	std::string _method;
	std::string _path;
	std::string _version;
};
#endif