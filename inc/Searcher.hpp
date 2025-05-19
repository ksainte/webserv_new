#ifndef SEARCHER_HPP
# define SEARCHER_HPP
#include "../inc/types/ConfigType.hpp"
#include "../inc/Config.hpp"
class Searcher
{
	private:

	const Config& _config;

	static bool 	isServerNameSet(const ConfigType::DirectiveMap& directives,
											const char* hostname);

	static void		iterateThroughServerBlock(const ConfigType::ServerBlockIt& first,
												const ConfigType::ServerBlockIt& last);
	
	const ServerBlock& _getDefaultServer(int sockFd, const char* hostname) const;

	public:

	Searcher(const Config&);

	const char* getLocationPrefix(int sockFd, const char* host, const char* url) const;

	const ConfigType::DirectiveValue* findServerDirective(int sockFd,
		const std::string& key, const char* host) const;

	const ConfigType::DirectiveValue* findLocationDirective(int sockFd,
		const std::string& key, const char* hostname, const char* route) const;
};
#endif