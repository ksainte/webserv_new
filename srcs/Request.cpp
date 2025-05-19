#include "../inc/Request.hpp"
#include "../inc/Logger.hpp"
#include <cstring>
#include <sys/socket.h>
#include <cstdlib>
#include <string.h>

std::string Request::getMethod() const
{
	return (_method);
}

std::string Request::getFilename() const
{
	return (_path);
}

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
		str.append(_buff, sizeof(_buff));
		memset(_buff, 0 , BUFFSIZE);
		_offset = 0;
		size_t pos = str.find("\r\n\r\n");
		if (pos != std::string::npos)
		{
			return (0);
		}
		return (1);
    }
//   str.append(_buff, sizeof(_buff));
	str.append(_buff, 0, strlen(_buff));
	return (0);
}

std::map<std::string, std::string> &Request::set_headers(void)
{
  std::string index_filename = "/index.html";


	std::istringstream stream(str);
	std::string line;

	std::cout << "REQUEST set headers\n";

	// Parse request line
	if (std::getline(stream, line)) 
	{
		std::istringstream lineStream(line);//how does it know to omit /?
		lineStream >> _method >> _path >> _version;
		//Add request line parsing logic here
		
		_path = (_path.compare("/") == 0) ? index_filename : _path;
		// std::cout << _method << "\n";
		// std::cout << _path << "\n";
		// std::cout << _version << "\n";
	}
	while (std::getline(stream, line) && line != "\r") 
	{
		// std::cout << "\ncurrent line is "<< line << "\n";
		size_t pos = line.find(":");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);

			// Trim whitespace
			// Shoudl also trim whitespaces before key and after value
			key.erase(key.find_last_not_of(" ") + 1);
			value.erase(0, value.find_first_not_of(" "));
			value.erase(value.find_last_not_of("\r") + 1);

			// Convert key to lowercase for case-insensitive comparison
			for (size_t i = 0; i < key.size(); ++i) 
			{
				key[i] = std::tolower(key[i]);
			}
			key_value_headers.insert(std::pair<std::string, std::string>(key, value));
			// std::cout << key << "\n";
			// std::cout << value << "\n";
			// std::cout << "-----------------------------------------";

		}
	}
	std::cout << "Map size: " << key_value_headers.size() << std::endl;
	return (key_value_headers);
}
// void Request::set_headers(void)
// {
// 	std::istringstream stream(_buff);
// 	std::string line;

// 	std::cout << "REQUEST set headers\n";

// 	// Parse request line
// 	if (std::getline(stream, line)) 
// 	{
// 		std::istringstream lineStream(line);//how does it know to omit /?
// 		lineStream >> method >> path >> version;
// 		//Add request line parsing logic here
		
// 		std::cout << method << "\n";
// 		std::cout << path << "\n";
// 		std::cout << version << "\n";
// 		std::cout << "End of first line\n";
// 	}
// 	// Parse headers
// 	while (std::getline(stream, line) && line != "\r") 
// 	{
// 		std::cout << "current line is "<< line << "\n";
// 		size_t pos = line.find(":");
// 		if (pos != std::string::npos) {
// 			std::string key = line.substr(0, pos);
// 			std::string value = line.substr(pos + 1);

// 			// Trim whitespace
// 			// Shoudl also trim whitespaces before key and after value
// 			key.erase(key.find_last_not_of(" ") + 1);
// 			value.erase(0, value.find_first_not_of(" "));
// 			value.erase(value.find_last_not_of("\r") + 1);

// 			// Convert key to lowercase for case-insensitive comparison
// 			for (size_t i = 0; i < key.size(); ++i) {
// 				key[i] = std::tolower(key[i]);
// 			}

// 			// Store header
// 			headers.insert(key, value);//HashTable headers;
// 		}
// 	}
// 	std::cout << "HEADERS WORK END\n";
// 	state &= ~HEADERS_END;
// 	state |= HEADERS_SET;
// }

Request::Request()
{
  _offset = 0;
	memset(_buff, 0 , BUFFSIZE);
	LOG_DEBUG << "Request created";
}

// Request::Request(const Request& other) {

// 	_buff = new char[_buffSize];
// 	memcpy(_buff, other.getBuff(), other.getBuffSize());
// 	LOG_DEBUG << "Request copied";
// }

// Request& Request::operator=(const Request& other) {

// 	if (this == &other)
// 		return *this;
	
// 	delete[] _buff;
// 	_rawBytes.clear();
	
// 	_rawBytes = other.getRawBytes();
// 	_buff = new char[_buffSize];
// 	memcpy(_buff, other.getBuff(), other.getBuffSize());
	
// 	LOG_DEBUG << "Request copy assigned";
// 	return *this;
// }

// Request::~Request() {
// 	delete[] _buff;
// 	_buff = NULL;
// 	_rawBytes.clear();
// 	LOG_DEBUG << "Request destructed";
// }

// const char* Request::getBuff() const 
// {return _buff;}

// int Request::getBuffSize() const 
// {return _buffSize;}

// const std::vector<char>& Request::getRawBytes() const 
// {return _rawBytes;};