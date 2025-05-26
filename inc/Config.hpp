#pragma once
#include <cstdlib>
#include "AConfig.hpp"
#include "Logger.hpp"

struct Token;

class Config : public AConfig<std::list<Token> >
{
public:

  template <typename T>
  explicit Config(T tokens): AConfig(tokens)
  {
    LOG_DEBUG << "Config created";

    const char* configFile = std::getenv("WEBSERV_SAVE_CONFIG_FILE");
    if (configFile)
    {
      std::ofstream os(configFile);
      os << toJson(0) << std::endl;
      os.close();
    }
  }

  ~Config();
};
