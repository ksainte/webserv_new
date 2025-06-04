#ifndef UTILS_HPP
#define UTILS_HPP
#include <stdio.h>
#include <string>
bool	ipV4ToNl(std::string ipV4dotNotation, unsigned int& dst);
std::string	nlToipv4(unsigned int netLong);
ssize_t bodySize(const std::string& s);
bool&	getSigIntFlag();
#endif