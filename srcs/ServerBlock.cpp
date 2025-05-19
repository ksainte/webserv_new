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

void	ServerBlock::setIpPort(const std::string& ipPort)
{
	std::string ip = ipPort.substr(0,ipPort.find(":"));
	_ip = ipV4ToNl(ip);

	std::stringstream ss(ipPort.substr((ipPort.find(":") + 1)));

	unsigned short port;
	ss >> std::dec >> port;
	_port = htons(port);
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