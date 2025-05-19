#include "../inc/AConfig.hpp"
#include "../inc/Logger.hpp"
#include <sstream>

const ConfigType::ServerBlocks& AConfig::getServerBlocks() const {return _serverBlocks;}

void	AConfig::locationBlockHandler(const std::string& prefix) {
	_serverBlocks[_serverBlocks.size() - 1]
		.insert(new LocationBlock(prefix));
}

void	AConfig::serverBlockHandler() {
	if (_serverBlocks.empty()) {
		// _serverBlocks.reserve(10);
	}
	_serverBlocks.push_back(ServerBlock());
}

std::string	AConfig::toJson(int indentLevel) const {
	std::stringstream ss;

	for (ServerBlocks::const_iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); ++it) {
		ss << "\"server\" " << (*it).toJson(indentLevel);
		if (static_cast<unsigned int>(it - _serverBlocks.begin()) < _serverBlocks.size() - 1) 
			ss << ',';
		ss << '\n';
	}
	return ss.str();
}

AConfig::~AConfig() {LOG_DEBUG << "AConfig destroyed";}