#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>
// unsigned long ipV4ToNl(std::string ipV4dotNotation);
bool ipV4ToNl(std::string ipV4dotNotation, unsigned int& dst);
bool&	getSigIntFlag();
#endif