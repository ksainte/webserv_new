#pragma once
#include "AConfig.hpp"
#include "Logger.hpp"
class Config : public AConfig {
	public:
		enum blockType {SERVER, LOCATION};

		const std::vector<std::string>& search(const blockType block, const std::string& key) const;

		template<typename T>
		Config(T tokens): AConfig(tokens) {LOG_DEBUG << "Config created";}
		~Config();
};