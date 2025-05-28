// #ifndef CGI_HPP
// #define CGI_HPP
// #include <string>
// #include "../inc/types/ConfigType.hpp"
// class Cgi
// {
//   std::string _path;
//   ConfigType::CgiParams _params;
//   static const int size = 100;
//   char *_env[size];
//
// public:
//
//   Cgi();
//   Cgi(const Cgi& other);
//   Cgi& operator=(const Cgi& other);
//   ~Cgi();
//
//   const ConfigType::CgiParams& getParams() const;
//   const std::string& getPath() const;
//   char* const* getEnv() const;
//
//   void  addCgiParams(const std::string& first, const std::string& second);
//   void  setPath(const std::string& path);
//   void  setCgiEnv();
//   void  execute(int fd) const;
// };
//
// #endif
