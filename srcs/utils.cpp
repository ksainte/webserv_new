#include "../inc/utils.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <sstream>

/**
 * @brief Convert a ipV4 address in dot notation into binary in network-bytes order
 * 
 * @param ipV4dotNotation ipV4 in string dot notation e.g. "127.0.0.1"
 * @return unsigned long 
 */

bool ipV4ToNl(std::string ipV4dotNotation, unsigned int& dst)
{
	unsigned long byte1, byte2, byte3, byte4;

	std::replace(ipV4dotNotation.begin(),
	 ipV4dotNotation.end(), '.', ' ');
	std::stringstream ss(ipV4dotNotation);

	ss >> byte1 >> byte2 >> byte3 >> byte4;

	char remaining;
	if (ss.fail()
		|| byte1 > 255 
		|| byte2 > 255
		|| byte3 > 255 
		|| byte4 > 255
		|| ss >> remaining)
		return false;
		// throw std::invalid_argument(ErrorMessages::E_BAD_ARG);

	// unsigned long host_bytes = 0;
	dst = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
	dst = htonl(dst);
	// host_bytes = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;


	// return htonl(host_bytes);
	return true;
}

bool&	getSigIntFlag() {
	static bool sigIntFlag = false;
	return sigIntFlag;
}
