// #include "../inc/Cgi.hpp"
// #include <cstring>
// #include "../inc/Logger.hpp"
//
// Cgi::Cgi(): _env()
// {
//   std::memset(_env, 0, size);
//   LOG_DEBUG << "Cgi created\n";
// }
//
// Cgi::Cgi(const Cgi& other): _env()
// {
//   _path = other.getPath();
//   _params = other.getParams();
//   std::memcpy(_env, other.getEnv(), size);
// }
//
// Cgi& Cgi::operator=(const Cgi& other)
// {
//   if (this == &other)
//     return *this;
//   std::memcpy(_env, other.getEnv(), size);
//   _path = other.getPath();
//   _params = other.getParams();
//   return *this;
// }
//
// Cgi::~Cgi()
// {
//   LOG_DEBUG << "Cgi deleted\n";
// }
//
// void Cgi::addCgiParams(const std::string& first, const std::string& second)
// {
//   _params.push_back(std::make_pair<std::string, std::string>(first, second));
// }
//
// void Cgi::execute(const int fd) const
// {
//
// }
//
// void Cgi::setCgiEnv()
// {
//
// }
//
// void Cgi::setPath(const std::string& path)
// {
//   _path = path;
// }
//
// char* const* Cgi::getEnv() const
// {
//   return _env;
// }
//
// const ConfigType::CgiParams& Cgi::getParams() const
// {
//   return _params;
// }
//
// const std::string& Cgi::getPath() const
// {
//   return _path;
// }