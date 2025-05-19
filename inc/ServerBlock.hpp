#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP
#include "Trie.hpp"
#include "ADirective.hpp"
#include <stdint.h>
/**
 * @brief (C++98) Represents a server configuration block, managing directives and nested blocks.
 * @details Inherits directive handling from ADirective and a searchable structure
 * (for LocationBlocks) from Trie.
 */
class ServerBlock: public Trie, public ADirective {

	public:

	ServerBlock();
	ServerBlock(const ServerBlock& other);
	ServerBlock& operator=(const ServerBlock& other);
	
	~ServerBlock();
	std::string toJson(int indentLevel) const;

	void	setIpPort(const std::string& ipPort);

	uint32_t	getIp() const;
	uint32_t	getPort() const;

	private:

	//Stored in network bytes order
	uint32_t 	_ip;
	uint16_t	_port;

};
#endif