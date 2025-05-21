#pragma once
#include <cstdlib>
#include "AConfig.hpp"
#include "Logger.hpp"
class Config : public AConfig {
	public:
		enum blockType {SERVER, LOCATION};

		const std::vector<std::string>& search(const blockType block, const std::string& key) const;

		template<typename T>
		Config(T tokens): AConfig(tokens) {
			LOG_DEBUG << "Config created";
			
			char* configFile = std::getenv("WEBSERV_SAVE_CONFIG_FILE");
			if (configFile)
			{
				std::ofstream os(configFile);
				os << toJson(0) << std::endl;
				os.close();
			}
		}
		~Config();
};