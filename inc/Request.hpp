#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <vector>
#include "../inc/Response.hpp"
#define BUFFSIZE 1000


class Request
{
	public:

	Request();
	// ~Request();
	// Request(const Request& other);
	// Request& operator=(const Request& other);

	const char*								getBuff() const;
	std::string								getMethod() const;
 	std::string								getFilename() const;
	// int										getBuffSize() const;
	// const std::vector<char>&				getRawBytes() const;

	int	read(int fd);
	void set_headers(void);

	protected:

	// char*							_buff;
	char *							_method_type;
	char*							_filename;
	char 						_buff[BUFFSIZE];
	off_t 								_offset;
	std::string 							str;
	std::string _method;
	std::string _path;
	std::string _version;
};
#endif