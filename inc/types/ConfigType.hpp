#ifndef CONFIG_TYPE_HPP
# define CONFIG_TYPE_HPP
#include <list>
#include <vector>
#include <string>
#include <map>

class ServerBlock;

class ConfigType {
	public:
		typedef std::vector<std::string> 					Result;

		typedef std::list<std::pair<std::string, std::string> > CgiParams;

		typedef std::list<std::pair<std::string, std::string> >::const_iterator CgiParamsIt;

		typedef std::vector<ServerBlock> 					ServerBlocks;
		typedef std::vector<ServerBlock>::const_iterator 	ServerBlockIt;

		typedef std::vector<std::string> 					DirectiveValue;
		typedef std::vector<std::string>::const_iterator 	DirectiveValueIt;

		typedef std::map<std::string, 
			std::vector<std::string> >						DirectiveMap;
		typedef std::map<std::string, 
			std::vector<std::string> >::const_iterator 		DirectiveMapIt;

		enum BlockType {SERVER, LOCATION};
};
#endif