#ifndef UTILS_HPP
#define UTILS_HPP
#include <cstdio>
#include <string>
bool	ipV4ToNl(std::string ipV4dotNotation, unsigned int& dst);
std::string	nlToipv4(unsigned int netLong);
ssize_t bodySize(const std::string& s);
bool&	getSigIntFlag();
bool isDir(const char* path);
std::string listDir(const std::string& name);
#endif