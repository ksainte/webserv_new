#ifndef ADIRECTIVE_HPP	
#define ADIRECTIVE_HPP
#include "../inc/ToJson.hpp"
#include "../inc/types/TokenType.hpp"
#include "../inc/types/ConfigType.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/Logger.hpp"
#include <cassert>
#include <vector>
#include <string>
/**
 * @brief (C++98) Abstract Base Class for managing configuration directives.
 * @details Provides storage and parsing logic for key-value directives.
 * Inherits virtually from ToJson to support JSON serialization via an interface.
 * Cannot be instantiated directly; must be subclassed.
 */
class ADirective: public virtual ToJson, public virtual ConfigType {
	protected:
		ADirective();
		ADirective(const ADirective& other);
		ADirective& operator=(const ADirective& other);
		virtual	~ADirective() = 0;
	public:

	/**
	* @brief (C++98) Parses a directive from token range [first, last) and adds/overwrites it in _directives.
	* @details Expects first token as key, subsequent tokens up to ';' as value list.
	* Requires C++98 Forward Iterators. Tokens need '.value' (string convertible) and '.type' (int comparable).
	* Range must contain key + value/semicolon (>= 2 tokens) and end with ';'.
	* @param first Iterator to the key token.
	* @param last Iterator past the end of the relevant token range.
	* @throw std::invalid_argument If ';' is missing before 'last'. Can throw std::invalid_argument.
	* @note Map assignment uses copy (potentially costly in C++98). std::distance can be O(N).
	*/
	template<typename IteratorType>
	void addDirective(const IteratorType& first, const IteratorType& last) {
	
		std::ptrdiff_t rangeDistance = std::distance(first, last); // O(N) for non-random-access iterators
		assert(rangeDistance > 1 && "Directive range needs at least 2 tokens (key + value/semicolon)");

		std::vector<std::string> directiveValues;
		if (rangeDistance > 1) {
			// Reserve approx space to avoid reallocations (key token isn't stored)
			directiveValues.reserve(static_cast<typename std::vector<std::string>::size_type>(rangeDistance - 1));
		}

		IteratorType currentTokenIter = first;
		++currentTokenIter; // Start processing after the key token
	
		// Iterate through value tokens until semicolon
		while (currentTokenIter != last && (*currentTokenIter).type != static_cast<int>(Token::SEMICOLON)) {
			directiveValues.push_back((*currentTokenIter).value); // Copy token value
			++currentTokenIter;
		}

		// Ensure loop finished on a semicolon, not by reaching the end
		if (currentTokenIter == last) {
			LOG_CRITICAL << ErrorMessages::E_BAD_ARG;
			throw std::invalid_argument(ErrorMessages::E_BAD_ARG);
		}

		// Add/overwrite entry in the map.
		// C++98: operator[] + assignment performs a full copy of directiveValues.
		_directives[(*first).value] = directiveValues;
	}

	const CgiParams& getCgiParams() const;
	void	addCgiParams(std::string first, std::string last);

		virtual const DirectiveMap&	getDirectives() const;
		std::string	toJson(int indentLevel) const;
	private:
		DirectiveMap	_directives;
		CgiParams			_cgiParams;
};
#endif