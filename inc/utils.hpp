#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>
bool	ipV4ToNl(std::string ipV4dotNotation, unsigned int& dst);
std::string	nlToipv4(unsigned int netLong);
bool&	getSigIntFlag();
#endif