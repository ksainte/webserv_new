#include "../inc/ADirective.hpp"
#include <cassert>
#include <cstdlib>
#include <sstream>
#include "../inc/Logger.hpp"

const ConfigType::DirectiveMap& ADirective::getDirectives() const { return _directives; }

ADirective::ADirective(const ADirective& other):
  _directives(other.getDirectives()),
  _cgiParams(other.getCgiParams()),
  _errorPages(*other.getErrorPages())
{
  LOG_DEBUG << "ADirective copied\n";
}

ADirective& ADirective::operator=(const ADirective& other)
{
  if (this == &other)
    return *this;
  _directives = other.getDirectives();
  _cgiParams = other.getCgiParams();
  _errorPages = *other.getErrorPages();
  return *this;
}

ADirective::ADirective(): ToJson()
{
  LOG_DEBUG << "ADirective created";
}

ADirective::~ADirective()
{
  LOG_DEBUG << "ADirective destroyed";
}

void ADirective::addErrorPage(std::list<Token>::const_iterator first,
                              std::list<Token>::const_iterator last)
{
  std::list<Token>::const_iterator it = first;

  while (it != last && it->type != Token::SEMICOLON)
    ++it;

  assert(it != last && "index out of bound");

  --it;
  std::string filename;

  for (; it != first; --it)
  {
    unsigned int errnum = 0;
    std::stringstream ss(it->value);
    if (!ss)
      continue;
    ss >> std::dec >> errnum;

    if (!ss.eof())
      filename = it->value;
    else if (!filename.empty() && 200 <= errnum && errnum < 600)
      _errorPages[errnum] = filename;
  }
}

void ADirective::addCgiParams(std::string first, std::string last)
{
  (void)last;
  _cgiParams.push_back(std::make_pair(first, last));
}

const ConfigType::CgiParams& ADirective::getCgiParams() const
{
  return _cgiParams;
}

const ConfigType::ErrorPage* ADirective::getErrorPages() const
{
  return &_errorPages;
}

std::string ADirective::toJson(int indentLevel) const
{
  std::stringstream ss;
  std::string ind = indent(indentLevel);

  bool firstElement = true; // Flag to manage leading commas

  // 1. Iterate through the map of directives (_directives)
  for (DirectiveMap::const_iterator it = _directives.begin(); it != _directives.end(); ++it)
  {
    if (!firstElement)
    {
      ss << ",\n";
    }
    ss << ind << "\"" << it->first << "\": [";

    bool firstValue = true;
    DirectiveValueIt valIt = it->second.begin();
    DirectiveValueIt valEnd = it->second.end();

    for (; valIt != valEnd; ++valIt)
    {
      if (!firstValue)
      {
        ss << ", ";
      }
      ss << "\"" << *valIt << "\""; // NOTE: No escaping applied
      firstValue = false;
    }
    ss << "]";
    firstElement = false;
  }

  // 2. Add the cgi_params list if it's not empty
  if (!_cgiParams.empty())
  {
    // Add comma separator if directives were present
    if (!firstElement)
    {
      ss << ",\n";
    }

    // Output key ("cgi_params") and start of the value array
    ss << ind << "\"cgi_params\": [";

    bool firstPair = true; // Flag for commas within cgi_params array
    // Iterate through the list of cgi_params pairs
    for (CgiParamsIt cgiIt = _cgiParams.begin(); cgiIt != _cgiParams.end(); ++cgiIt)
    {
      if (!firstPair)
      {
        ss << ", ";
      }
      // Output each pair as a [key, value] JSON array
      // NOTE: No escaping applied to key or value strings
      ss << "[\"" << cgiIt->first << "\", \"" << cgiIt->second << "\"]";
      firstPair = false;
    }
    // Close the value array
    ss << "]";
    firstElement = false; // Mark that an element has been processed
  }

  // Return the accumulated JSON string fragment
  return ss.str();
}
