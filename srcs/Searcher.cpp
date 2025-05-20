#include "../inc/Searcher.hpp"
#include "../inc/Logger.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/constants/SuccessMessages.hpp"
#include "../inc/ServerBlock.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <sys/socket.h>

bool Searcher::isServerNameSet(const ConfigType::DirectiveMap& directives, const char* hostname)
{
		// Get server name directive
		ConfigType::DirectiveMapIt it;
		it = directives.find(std::string("server_name"));

		if (it == directives.end())
			return false;

		// Search for hostname 
		ConfigType::DirectiveValueIt it2 = std::find(it->second.begin(), it->second.end(), 
				std::string(hostname));
		if (it2 == it->second.end())
		{
			LOG_INFO << ErrorMessages::HOST_NAME_NOT_FOUND;
			return false;
		}

		LOG_INFO << "Hostname \'" << hostname <<  "\' "
		<< SuccessMessages::HOST_NAME_FOUND;

		// Return true if hostname is found
		return true;
}

const ServerBlock& Searcher::_getDefaultServer(int sockFd, const char* hostname) const
{
	// Store socket ip:port
	struct sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	getsockname(sockFd, (struct sockaddr*)&addr, &addrLen);

	// Access serverBlocks vector
	const ConfigType::ServerBlocks& serverBlocks = _config.getServerBlocks();

	// Point to the first serverBlock who ip:port matches socket ip:port 
	ConfigType::ServerBlockIt defaultServer;
	bool	defaultServerSet = false;
	
	for (ConfigType::ServerBlockIt it = serverBlocks.begin(); it != serverBlocks.end(); ++it)
	{
		if ((*it).getIp() != addr.sin_addr.s_addr
			|| (*it).getPort() != addr.sin_port)
			continue;
		
		if (defaultServerSet == false)
		{
			defaultServer = it;
			defaultServerSet = true;
		}
		
		if (isServerNameSet((*it).getDirectives(), hostname))
				return *it;
	}

	return *defaultServer;

}

const char* Searcher::getLocationPrefix(int sockFd, const char* host, const char* url) const {
	const ServerBlock& serverBlock = _getDefaultServer(sockFd, host);
	const LocationBlock* locationBlock = serverBlock.search(url);
	if (!locationBlock) 
	{
		std::cout << "aaa";
		return NULL;
	}
	return locationBlock->getPrefix().c_str();
}

const ConfigType::DirectiveValue* Searcher::findLocationDirective(int sockFd,
	const std::string& key, const char* host, const char* route) const 
{
		// Get the default server for a specific host
		const ServerBlock& serverBlock = _getDefaultServer(sockFd, host);

		// Get route configuration
		const LocationBlock* locationBlock = serverBlock.search(route);
		if (!locationBlock)
		{
			LOG_ERROR << "\'" << route << "\' " << ErrorMessages::E_BAD_ROUTE;
			return NULL;
		}

		// Access route configuration to get the directive
		ConfigType::DirectiveMapIt it = locationBlock->getDirectives().find(key);
		if (it == locationBlock->getDirectives().end())
		{
			LOG_INFO << "\'" + key + "\' " << ErrorMessages::KEY_NOT_FOUND;
			return NULL;
		}

		return &(it->second);
}

const ConfigType::DirectiveValue* Searcher::findServerDirective(int sockFd,
	const std::string& key, const char* host) const
{

	// Get the default server for a specific host
	const ServerBlock& serverBlock = 
	_getDefaultServer(sockFd, host);

	// Get directive
	ConfigType::DirectiveMapIt it = serverBlock.getDirectives().find(key);
	if (it == serverBlock.getDirectives().end())
	{
		LOG_INFO << "\'" + key + "\' " << ErrorMessages::KEY_NOT_FOUND;
		return NULL;
	}

	LOG_INFO << "\'" + key + "\' " << SuccessMessages::KEY_FOUND;

	// Return directive value
	return &(it->second);
}

Searcher::Searcher(const Config& config) : _config(config) 
{LOG_DEBUG << "Searcher created";}