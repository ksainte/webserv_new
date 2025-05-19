#include "../inc/Response.hpp"

#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include "../inc/Searcher.hpp"



// Response::Response(const Response& other)
// : 

Response::Response(){}


// int Response::send_to_cgi(int sock_file_descriptor, char *path)
int Response::send_to_cgi(int sock_file_descriptor, const char *path)
{
  close(1);
  dup2(sock_file_descriptor, 1);

  const char *arr[] = {path, NULL};

  setenv("QUERY_STRING", path, 1);
  int result;

  result = execv("./cgi-bin/GET.cgi", const_cast<char **>(arr)); // pass your script_name

  if (result < 0) 
  {
    std::cout << "result false\n";
  }
  return 0;
}

int Response::send_response(int sock_file_descriptor, std::string filename, std::string method_type, Searcher &_searcher, std::map<std::string, std::string> &key_value_headers)
{
  std::cout << "filename is " << filename << "\n";
  std::cout << "method is " << method_type << "\n";

  std::string root_directory;
  std::string value;
	const ConfigType::DirectiveValue* p;

// filename /contents/contact.html
for(std::map<std::string, std::string>::const_iterator it = key_value_headers.begin(); it != key_value_headers.end(); ++it)
{
  if (it->first == "host")
    value = it->second;
}

std::clog <<"\nValue is "<<  value << "\n\n";
  const char *route = _searcher.getLocationPrefix(sock_file_descriptor, "2", filename.c_str());//recup /contents 
  // if (!test)
  // {
  //   return 0;
  // }
  std::clog << " test is ------------------"<< route << "\n";
  p = _searcher.findLocationDirective(sock_file_descriptor, "root", "2", route);
  if (p)
  {
    std::clog << "\nroot_directory is:\n";
    for (ConfigType::DirectiveValueIt it = (*p).begin(); it != (*p).end(); ++it) 
    {
        std::cout << *it << "\n";//checker si c est directory ou file
        root_directory = *it;
        //si directory, append uri, donc / et contact.html
    }
  }
  if (!p)
    return 0;
  std::string sub = filename.substr(strlen(route), filename.length() - strlen(route));
  // std::cout << strlen(test) << "\n";
  std::clog << sub << "\n";

  //recuperer l uri!
// exit(1);


  if ((filename.compare("/index.html") == 0)) 
  {
    root_directory = "./index.html";
  }
  else 
  {
    root_directory.append(sub);
  }


  // std::cout << "root directory is " << method_type << "\n";

// char arr[root_directory.length() + 1]; 
// memset(arr,0, root_directory.length());
//   for (int x = 0; x < sizeof(arr); x++) { 
//     arr[x] = root_directory[x]; 
  // }

	const std::string filepath = "/home/larfef/Documents/Cursus/C++/exercices/webserv/contents/contact.htm";

  // if ((access(arr, R_OK) < 0)) 
	if ((access(filepath.c_str(), R_OK) < 0)) 
  {
    std:: cout << "\nERROR (file not found)\n";

		std::string send_501_error = "HTTP/1.0 501 Not Implemented\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 19\r\n"
                             "Connection: close\r\n"
                             "Last-Modified: Mon, 23 Mar 2020 02:49:28 GMT\r\n"
                             "Expires: Sun, 17 Jan 2038 19:14:07 GMT\r\n"
                             "Date: Mon, 23 Mar 2020 04:49:28 GMT\n\n" 
                             "501 Not Implemented";
      send(sock_file_descriptor, send_501_error.c_str(), send_501_error.size(), 0);
  }
  else 
	{
    if ((method_type.compare("GET") == 0))
    {
      std::cout << "\nReceived GET method\n";
      // send_to_cgi(sock_file_descriptor, arr);
			send_to_cgi(sock_file_descriptor, filepath.c_str());
    }
  }

  return 0;
}