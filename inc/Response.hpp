#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <fstream>
#include <map>
#include <string>

class Searcher;

class Response {

	public:

	Response();
	// Response(const Response& other);
	// Response& operator=(const Response& other);
    // int send_response(int sock_file_descriptor, std::string filename, std::string method_type, Searcher &searcher, std::map<std::string, std::string> &key_value_headers);
	int send_response(int sock_file_descriptor, std::string filename, std::string method_type, Searcher &_searcher, std::map<std::string, std::string> &key_value_headers);
    int send_to_cgi(int sock_file_descriptor, char *path);
	int isDirectoryExists(const char *path);
	// ~Response();

	// const std::ifstream& getifs() const;
	// // int send(int fd) const;

	protected:


};
#endif