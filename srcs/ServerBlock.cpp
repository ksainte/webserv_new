#include "../inc/ServerBlock.hpp"
#include "../inc/Logger.hpp"
#include "../inc/utils.hpp"
#include <netinet/in.h>
#include <sstream>

ServerBlock::ServerBlock(const ServerBlock& other)
: Trie(other), ADirective(other), _ip(other.getIp()), _port(other.getPort()) 
{LOG_DEBUG << "ServerBlock copied\n";}

ServerBlock& ServerBlock::operator=(const ServerBlock& other) {
	
	if (this == &other)
		return *this;

	Trie::operator=(other);
	ADirective::operator=(other);

	this->_ip = getIp();
	this->_port = getPort();
	
	return *this;
}

ServerBlock::ServerBlock(): Trie(), ADirective(), _ip(0), _port(0)
{LOG_DEBUG << "ServerBlock created";}

ServerBlock::~ServerBlock() {
	LOG_DEBUG << "ServerBlock destroyed";
}

bool	ServerBlock::setIpPort(const std::string& ipPort)
{
	
	if (!ipPort.size()) return false;

	size_t pos = ipPort.find(":");

	bool success = ipV4ToNl(ipPort.substr(0, pos), _ip);

	if (success && pos == std::string::npos)
		return true;

	if (pos == std::string::npos)
		pos = 0;
	else
		++pos;

	std::istringstream iss(ipPort.substr(pos));
	if (!iss) return false;
		
	iss >> _port;

	return iss.rdbuf()->in_avail() == 0 && success;
}

// bool	ServerBlock::setIpPort(const std::string& ipPort)
// {
	
// 	if (!ipPort.size()) return false;

// 	std::string ip = ipPort.substr(0,ipPort.find(":"));
	
// 	if (!ipV4ToNl(ip, _ip)
// 		&& ip.size() == ipPort.size())
// 	{
// 		std::istringstream iss(ipPort);
// 		if (!iss) return false;
		
// 		unsigned short port;
// 		iss >> port;
	
// 		if (iss.rdbuf()->in_avail() != 0)
// 			return false;		
// 	}
// 	else if (ip.size() != ipPort.size())
// 	{
// 		std::istringstream iss(ipPort.substr((ipPort.find(":") + 1)));
// 		if (!iss) return false;
		
// 		unsigned short port;
// 		iss >> port;
	
// 		if (iss.rdbuf()->in_avail() != 0)
// 			return false;	
// 		_port = htons(port);
// 	}
// 	return true;
// }

uint32_t	ServerBlock::getIp() const {return _ip;}

uint32_t	ServerBlock::getPort() const {return _port;}

std::string ServerBlock::toJson(int indentLevel) const {
    std::stringstream ss;
    std::string ind = indent(indentLevel);
    std::string ind2 = indent(indentLevel + 1);

    ss << ind << "{\n";

    // Include the JSON from ADirective
    ss << ind2 << "\"directives\": {\n" << ADirective::toJson(indentLevel + 1)
	<< "\n" << ind2 << "},\n";

    // Include the JSON from Trie
    ss << ind2 << "\"locations\": {\n" << Trie::toJson(indentLevel + 1) 
	<< "\n" << ind2 << "}\n";

    ss << ind << "}";
    return ss.str();
}