#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <fstream>

class Response {

	public:

	Response();
	// Response(const Response& other);
	// Response& operator=(const Response& other);
    int send_response(int sock_file_descriptor, std::string filename, std::string method_type);
    int send_to_cgi(int sock_file_descriptor, char *path);
	// ~Response();

	// const std::ifstream& getifs() const;
	// // int send(int fd) const;

	protected:

	// int _bytesSent;
	// int _buffSize;
	// int	_openFile();
	// int _readFile();
	// const char* _filename;
	// std::ifstream _ifs;
	// char* _buff;
};
#endif