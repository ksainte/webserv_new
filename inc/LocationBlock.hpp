#pragma once
#include <string>
#include "ADirective.hpp"
/**
 * @brief (C++98) Represents a location-specific configuration block (e.g., for URL paths).
 * @details Manages directives specific to a given path prefix, inheriting general
 * directive handling from ADirective.
 */
class LocationBlock : public ADirective
{
public:
  explicit LocationBlock(const std::string& prefix);
  LocationBlock(const LocationBlock& other);
  ~LocationBlock();
  const std::string& getPrefix() const;

private:
  std::string _prefix;
};
