#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <vector>
#include "../inc/Response.hpp"
#define BUFFSIZE 1000
#include <map>


class Request
{
	public:

	Request();
	// ~Request();
	// Request(const Request& other);
	// Request& operator=(const Request& other);

	std::string								getMethod() const;
 	std::string								getFilename() const;


	int	read(int fd);
	std::map<std::string, std::string>  &set_headers(void);
	std::map<std::string, std::string> key_value_headers;

	protected:


	char 						_buff[BUFFSIZE];
	off_t 								_offset;
	std::string 							str;
	std::string _method;
	std::string _path;
	std::string _version;
};
#endif