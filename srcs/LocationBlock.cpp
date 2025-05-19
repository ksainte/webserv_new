#include "../inc/LocationBlock.hpp"
#include "../inc/Logger.hpp"

const std::string&	LocationBlock::getPrefix() const {return _prefix;}

LocationBlock::LocationBlock(const LocationBlock& other) :
	ADirective(other),
	_prefix(other.getPrefix())
{
	LOG_DEBUG << "LocationBlock copied";
}

LocationBlock::LocationBlock(const std::string& prefix): _prefix(prefix)
{LOG_DEBUG << "LocationBlock created";}

LocationBlock::~LocationBlock()
{LOG_DEBUG << "LocationBlock detroyed";}