#pragma once
#include "../inc/Logger.hpp"
#include "../inc/types/ConfigType.hpp"
#include "../inc/types/TokenType.hpp"
#include "../inc/constants/ErrorMessages.hpp"
#include "../inc/ServerBlock.hpp"
#include "Tokenizer.hpp"
#include <cassert>
#include <typeinfo>

class AConfig : public ConfigType
{
	public:

	const ServerBlocks&	getServerBlocks() const;
	std::string	toJson(int indentLevel) const;

	protected:
	
	virtual	~AConfig() = 0;

	ServerBlocks _serverBlocks;
	
	template <typename T>
	AConfig(T tokens) {
		iterateThroughTokens(tokens);
	}

	template <typename T>
	void	iterateThroughTokens(T tokens) {
		for (typename T::iterator it = tokens.begin(); it != tokens.end(); it++)
		{
			/*
				Runtime check to be sure the dereferenced iterator
				access to a supported type.
				This could cause runtime overhead.
			*/
			// if (typeid(*it) != typeid(Token)) {
			// 	LOG_CRITICAL << "Unsupported element type inside the container.";
			// 	throw std::runtime_error(ErrorMessages::E_BAD_TYPE);
			// }
			checkTokenType(it, tokens.end());
		}
	}

	/**
	* @brief (C++98) Processes a token, dispatches to handlers, and manages block context.
	* @param it Iterator to the current token.
	* @param last Iterator past end (needed for directiveHandler).
	*/
	template <typename IteratorType>
	void checkTokenType(const IteratorType& it, const IteratorType& last) {

		static int blockType;
		static std::string	prefix;
	
		switch ((*it).type) {
			case Token::SERVER:
				serverBlockHandler();
				break;
			case Token::DIRECTIVE:
				directiveHandler(it, last, blockType, prefix);

				// Quick fix to store the ip and port 
				// in decimal in network bytes order
				if ((*it).value == "listen")
				{
					IteratorType tmp = it;
					std::advance(tmp, 1);
					_serverBlocks[_serverBlocks.size() - 1].setIpPort((*tmp).value);
				}
				break;
				
			case Token::LOCATION:
				locationBlockHandler((*it).value);
				prefix = (*it).value;
				break;
			case Token::RBRACE:
				--blockType;
				break;
			case Token::LBRACE:
				++blockType;
				break;
			default:
				break;
		}
	};

	/**
	* @brief (C++98) Routes a directive to be added based on context (block type/prefix).
	* @param it Iterator range containing the directive tokens.
	* @param last Iterator range containing the directive tokens.
	* @param blockType Identifier determining the target context (e.g., server vs location block).
	* @param prefix Used with certain block types (e.g., 2) to find the specific sub-context.
	*/
	template<typename IteratorType>
	void	directiveHandler(const IteratorType& it, const IteratorType& last,
				int blockType, const std::string& prefix) {

		assert(_serverBlocks.size() > 0 && "size must not be 0");
		switch(blockType) {
			case 1:
				// Add directive directly to the last server block
				_serverBlocks[_serverBlocks.size() - 1]
				.addDirective(it, last);
				break;
			case 2:
				// Add directive to a location sub-block in the last server block
				_serverBlocks[_serverBlocks.size() - 1]
				.search(prefix)->addDirective(it, last);
				break;
			default:
				break;
		}
	}

	void	locationBlockHandler(const std::string& prefix);
	void	serverBlockHandler();
};