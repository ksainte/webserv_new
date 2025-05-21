// #include <string>
// #include <unistd.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <iostream>
// #include <cstring>
// #include <sys/socket.h>
// #include "../inc/Searcher.hpp"
// #include <sys/stat.h>
// #include "../inc/Response.hpp"

// int Response::send_to_cgi(int sock_file_descriptor, char *path)
// {
//   close(1);
//   dup2(sock_file_descriptor, 1);

//   char *arr[] = {path, NULL};

//   setenv("QUERY_STRING", path, 1);
//   int result;

//   result = execv("./cgi-bin/GET.cgi", arr); // pass your script_name

//   if (result < 0) 
//   {
//     std::cout << "result false\n";
//   }
//   return 0;
// }

// int Response::isDirectoryExists(const char *path)
// {
//     struct stat stats;

//     stat(path, &stats);

//     // Check for file existence
//     if (S_ISDIR(stats.st_mode))
//         return 1;

//     return 0;
// }

// int Response::send_response(int sock_file_descriptor, std::string filename, std::string method_type, Searcher &_searcher, std::map<std::string, std::string> &key_value_headers)
// {
//   std::string root_directory;
//   std::string value;
// 	const ConfigType::DirectiveValue* p;


	
// 	for(std::map<std::string, std::string>::const_iterator it = key_value_headers.begin(); it != key_value_headers.end(); ++it)
// 	{
// 		if (it->first == "host")
// 			value = it->second;
// 	}

// std::clog <<"\nValue is "<<  value << "\n\n";
//   const char *route = _searcher.getLocationPrefix(sock_file_descriptor, value.c_str(), filename.c_str());//recup /contents
//   if (!route)
//   {
//     std::clog << "\nroute is not valid\n";
//     return (0);
//   }

//   std::clog << "\nROUTE/LOCATION is "<< route << "\n";
//   p = _searcher.findLocationDirective(sock_file_descriptor, "root", value.c_str(), route);
//   if (p)
//   {
//     std::clog << "\nROOT_DIRECTORY is:\n";
//     for (ConfigType::DirectiveValueIt it = (*p).begin(); it != (*p).end(); ++it) 
//     {
//         std::cout << *it << "\n";//checker si c est directory ou file
//         root_directory = *it;
//         //si directory, append uri, donc / et contact.html
//     }
//   }
//   if (!p)
//   {
//     std::clog << "root is not valid\n";
//     return 0;
//   }

//   //APPEND URI A ROOT SI EXIST!
//   std::string sub = filename.substr(strlen(route), filename.length() - strlen(route));
//   std::clog << "URI is "<< sub << "\n";
//   root_directory.append(sub);
//   std::cout << "ROOT_DIRECTORY AFTER APPEND IS:\n" << root_directory << "\n";


//   char arr[root_directory.length() + 1]; 
//   memset(arr,0, root_directory.length());
//   for (long unsigned int x = 0; x < sizeof(arr); x++) 
//   {
//     arr[x] = root_directory[x]; 
//   }

//   if (isDirectoryExists(arr))
//     std::clog << "\n ROOOOOT is a directory!\n";
//   else if ((access(arr, F_OK) == 0))
//   {
//     std::clog << "\n ROOOOOT is a file! Send directly\n";
//     send_to_cgi(sock_file_descriptor, arr);
//   }
//   else
//   {
//     std:: cout << "\nERROR HERE (file not found)\n";

// 		std::string send_501_error = "HTTP/1.0 501 Not Implemented\r\n"
//                              "Content-Type: text/plain\r\n"
//                              "Content-Length: 19\r\n"
//                              "Connection: close\r\n"
//                              "Last-Modified: Mon, 23 Mar 2020 02:49:28 GMT\r\n"
//                              "Expires: Sun, 17 Jan 2038 19:14:07 GMT\r\n"
//                              "Date: Mon, 23 Mar 2020 04:49:28 GMT\n\n" 
//                              "501 Not Implemented";
//       send(sock_file_descriptor, send_501_error.c_str(), send_501_error.size(), 0);
//       return(0);
//   }

//   p = _searcher.findLocationDirective(sock_file_descriptor, "index", value.c_str(), route);
//   if (p)
//   {
//     std::clog << "\nINDEX_DIRECTORY is:\n";
//     for (ConfigType::DirectiveValueIt it = (*p).begin(); it != (*p).end(); ++it) 
//     {
//         std::cout << *it << "\n";//checker si c est directory ou file
//         std::string temp;
//         temp = root_directory;
//         temp.append("/");
//         temp.append(*it);
//         std::cout << "New is :\n" << temp << "\n";

//         char arr1[temp.length() + 1]; 
//         memset(arr1,0, temp.length());
//           for (long unsigned int x = 0; x < sizeof(arr1); x++) { 
//             arr1[x] = temp[x]; 
//           }

//           if ((access(arr1, F_OK) < 0)) 
//           {
//             std::clog << "\n -----------no access\n";
//             continue;
//             // std:: cout << "\nERROR (file not found)\n";

//             // std::string send_501_error = "HTTP/1.0 501 Not Implemented\r\n"
//             //                         "Content-Type: text/plain\r\n"
//             //                         "Content-Length: 19\r\n"
//             //                         "Connection: close\r\n"
//             //                         "Last-Modified: Mon, 23 Mar 2020 02:49:28 GMT\r\n"
//             //                         "Expires: Sun, 17 Jan 2038 19:14:07 GMT\r\n"
//             //                         "Date: Mon, 23 Mar 2020 04:49:28 GMT\n\n" 
//             //                         "501 Not Implemented";
//             //   send(sock_file_descriptor, send_501_error.c_str(), send_501_error.size(), 0);
//           }
//           else {
//             if ((method_type.compare("GET") == 0))
//             {
//               std::cout << "\nReceived GET method\n";
//               send_to_cgi(sock_file_descriptor, arr1);
//             }
//           }

//         //si directory, append uri, donc / et contact.html
//     }
//   }
//   if (!p)
//   {
//     std::clog << "index directory is not valid\n";
//     return (0);
//   }
//   std::clog << "\nindex is not EXISTING\n";

//   return 0;
// }

// Response::Response() {LOG_DEBUG << "Response created\n";}

// Response::~Response() {LOG_DEBUG << "Response destroyed\n";}