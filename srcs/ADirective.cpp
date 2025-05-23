#include "../inc/ADirective.hpp"
#include "../inc/Logger.hpp"
#include <sstream>

const ConfigType::DirectiveMap&	ADirective::getDirectives() const {return _directives;}

ADirective::ADirective(const ADirective& other):
_directives(other.getDirectives()),
_cgiParams(other.getCgiParams())
{LOG_DEBUG << "ADirective copied\n";}

ADirective& ADirective::operator=(const ADirective& other) {
	if (this == &other)
		return *this;
	_directives = other.getDirectives();
	_cgiParams = other.getCgiParams();
	return *this;
}

ADirective::ADirective(): ToJson()
{LOG_DEBUG << "ADirective created";}

ADirective::~ADirective()
{LOG_DEBUG << "ADirective destroyed";}

void	ADirective::addCgiParams(std::string first, std::string last) {
	(void)last;
	_cgiParams.push_back(std::make_pair(first, last));
}

const ConfigType::CgiParams& ADirective::getCgiParams() const
{return _cgiParams;}

/**
 * @brief (C++98) Serializes the stored directives into a JSON string fragment.
 * @details Generates a comma-separated list of "key": [value_array] pairs suitable
 * for embedding within a larger JSON object.
 * @param indentLevel The base indentation level for formatting. Keys/arrays are indented one level deeper.
 * @return std::string A string containing the JSON representation of the directives.
 * @note Assumes existence of an `indent(int)` helper function.
 * @note Does NOT add enclosing curly braces `{}` for a complete JSON object.
 * @note Does NOT escape special characters within keys or values (potential for invalid JSON).
 */
 std::string ADirective::toJson(int indentLevel) const {

    // Use stringstream for efficient string building in C++98
    std::stringstream ss;
    // Assume indent() provides appropriate indentation string (e.g., spaces)
    std::string ind = indent(indentLevel);

    bool firstDirective = true; // Flag to manage leading commas between directives

    // Iterate through the map of directives (_directives)
    for (DirectiveMap::const_iterator it = _directives.begin(); it != _directives.end(); ++it) {
        // Add comma separator before subsequent directives
        if (!firstDirective) {
            ss << ",\n";
        }
        // Output key (in quotes) and start of the value array
        ss << ind << "\"" << it->first << "\": [";

        bool firstValue = true; // Flag to manage leading commas within the value array
        // Iterate through the vector of values for the current directive
        DirectiveValueIt valIt = it->second.begin();
        DirectiveValueIt valEnd = it->second.end();

        for (; valIt != valEnd; ++valIt) {
            // Add comma separator before subsequent values
            if (!firstValue) {
                ss << ", ";
            }
            // Output value string in quotes (NOTE: No escaping applied)
            ss << "\"" << *valIt << "\"";
            firstValue = false;
        }
        // Close the value array
        ss << "]";
        firstDirective = false; // Mark that the first directive has been processed
    }
    // Return the accumulated JSON string fragment
    return ss.str();
}