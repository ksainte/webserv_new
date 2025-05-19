#ifndef TRIE_HPP
#define TRIE_HPP
#include "../inc/ToJson.hpp"
#include "LocationBlock.hpp"
#include <string>
#include <map>

struct TrieNode {
	std::map<char, TrieNode*> 	children;
	bool 						isEndOfPrefix;
	LocationBlock*				location;
	TrieNode();
	TrieNode(const TrieNode& other);
	TrieNode& operator=(const TrieNode& other);
	~TrieNode();
};
/**
 * @brief (C++98) Abstract Base Class for a Prefix Trie.
 * @details Used for efficient prefix-based lookup (e.g., finding LocationBlocks by URI path).
 * Inherits virtually from ToJson for serialization capabilities.
 */
class Trie: public virtual ToJson {
	public:
		Trie();
		Trie(const Trie& other);
		Trie& operator=(const Trie& other);
		virtual ~Trie() = 0;
		void			insert(LocationBlock* location);
		LocationBlock*	search(const std::string& uri) const;
		TrieNode*		getRoot() const;
		std::string		toJson(int indentLevel) const;
		void			printTriePathsJson(std::stringstream& ss, TrieNode* node,
							const std::string& currentPath, int indentLevel) const;
	protected:
		TrieNode* _root;
};
#endif