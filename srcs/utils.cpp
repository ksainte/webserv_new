#include "../inc/utils.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <sstream>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

/**
 * @brief Convert a ipV4 address in dot notation into binary in network-bytes order
 * 
 * @param ipV4dotNotation ipV4 in string dot notation e.g. "127.0.0.1"
 * @return unsigned long 
 */

unsigned long ipV4ToNl(std::string ipV4dotNotation)
{
  unsigned long byte1, byte2, byte3, byte4;

  std::replace(ipV4dotNotation.begin(),
               ipV4dotNotation.end(), '.', ' ');
  std::stringstream ss(ipV4dotNotation);

  ss >> byte1 >> byte2 >> byte3 >> byte4;

  char remaining;
  if (ss.fail()
    || byte1 > 255
    || byte2 > 255
    || byte3 > 255
    || byte4 > 255
    || ss >> remaining)
    throw std::invalid_argument(ErrorMessages::E_BAD_ARG);

  unsigned long host_bytes = 0;
  host_bytes = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;

  return htonl(host_bytes);
}

bool ipV4ToNl(std::string ipV4dotNotation, unsigned int& dst)
{
  unsigned long byte1, byte2, byte3, byte4;

  std::replace(ipV4dotNotation.begin(),
               ipV4dotNotation.end(), '.', ' ');
  std::stringstream ss(ipV4dotNotation);

  ss >> byte1 >> byte2 >> byte3 >> byte4;

  char remaining;
  if (ss.fail()
    || byte1 > 255
    || byte2 > 255
    || byte3 > 255
    || byte4 > 255
    || ss >> remaining)
    return false;
  // throw std::invalid_argument(ErrorMessages::E_BAD_ARG);

  // unsigned long host_bytes = 0;
  dst = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  dst = htonl(dst);
  // host_bytes = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;

  // return htonl(host_bytes);
  return true;
}

std::string nlToipv4(unsigned int netLong)
{
  unsigned int hostLong = ntohl(netLong);
  std::ostringstream oss;
  if (!oss) return "";

  unsigned int mask = 0;
  for (int start = 24, end = 31; end >= 7; start -= 8, end -= 8)
  {
    for (int i = start; i <= end; ++i)
    {
      mask = mask | (1 << i);
    }
    unsigned int bits = (hostLong & mask) >> start;
    oss << bits;
    if (end > 7)
      oss << ".";
    mask = 0;
  }
  return oss.str();
}

// Helper since we first store the body size as a string
ssize_t bodySize(const std::string& s)
{
  std::stringstream iss(s);

  std::map<std::string, ssize_t> conversions;
  conversions["B"] = 1;
  conversions["KB"] = 1000;
  conversions["MB"] = 1000 * 1000;

  ssize_t n;
  std::string suffix;

  iss >> n;

  if (n < 0) return -1;

  if (iss.fail()) return -1;

  if (iss.peek() == -1) return n;

  iss >> suffix;

  if (iss.peek() != -1) return -1;

  const std::map<std::string, ssize_t>::const_iterator it =
    conversions.find(suffix);

  if (it == conversions.end()) return -1;

  const ssize_t overflow = n * it->second;

  if (overflow / it->second != n) return -1;

  return n * it->second;
}

std::string listDir(const std::string& name) {

  std::stringstream ss;
  std::stringstream res;
  DIR* dirp = opendir(name.c_str());

  ss << "<!DOCTYPE html>\n";
  ss << "<html>\n<head>\n<title>Directory Listing</title>\n</head>\n<body>\n";
  ss << "<h1>Contents of " << name << "</h1>\n";
  ss << "<ul>\n";

  dirent* dirent;

  while ((dirent = readdir(dirp)))
  {
    if (std::string(dirent->d_name) == "." || std::string(dirent->d_name) == "..")
      continue;
    ss << "<li><p>" << dirent->d_name << "</li>\n";
  }

  ss << "</ul>\n";
  ss << "</body>\n</html>\n";

  closedir(dirp);

  res << "HTTP/1.1 200 OK\r\n"
  << "content-length: " << ss.str().size() << "\r\n"
  << "content-type: text/html\r\n\r\n"
  << ss.str();

  return res.str();
}

bool isDir(const char* path)
{
  struct stat stats = {};
  stat(path, &stats);
  return S_ISDIR(stats.st_mode);
}

bool& getSigIntFlag()
{
  static bool sigIntFlag = false;
  return sigIntFlag;
}
