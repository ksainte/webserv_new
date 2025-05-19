#include "../inc/Response.hpp"

#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>



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

int Response::send_response(int sock_file_descriptor, std::string filename, std::string method_type)
{

  // std::string root_directory = "./contents";

  // std::cout << "filename is " << filename << "\n";
  // std::cout << "method is " << method_type << "\n";

  // if ((filename.compare("/index.html") == 0)) 
  // {
  //   root_directory = "./index.html";
  // }
  // else 
  // {
  //   root_directory = "./contents";
  //   root_directory.append(filename);
  // }


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