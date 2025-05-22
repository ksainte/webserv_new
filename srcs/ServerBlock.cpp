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

ServerBlock::ServerBlock(): Trie(), ADirective()
{LOG_DEBUG << "ServerBlock created";}

ServerBlock::~ServerBlock() {
	LOG_DEBUG << "ServerBlock destroyed";
}

bool	ServerBlock::setIpPort(const std::string& ipPort)
{
	
	if (!ipPort.size()) return false;

	size_t pos = ipPort.find(":");

	// early return because incomplete ip address completion using default ip:port
	// value is not currently implemented
	if (pos == std::string::npos) return false;

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
	_port = htons(_port);

	return iss.rdbuf()->in_avail() == 0 && success;
}

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