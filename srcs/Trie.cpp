#include "../inc/Trie.hpp"
#include "../inc/Logger.hpp"
#include <sstream>

/**
 * @brief (C++98) Inserts a LocationBlock into the Trie based on its prefix path.
 * @details Traverses the Trie character by character according to the prefix.
 * Creates intermediate TrieNodes if the path doesn't exist. Replaces and deletes
 * any existing LocationBlock* at the exact same prefix path.
 * @param location Pointer to the LocationBlock to be inserted. The Trie assumes
 * ownership of this pointer upon successful insertion.
 * @note The Trie takes ownership of the passed 'location' pointer. Ensure the
 * pointer is valid and not managed elsewhere after this call.
 */

void	Trie::insert(LocationBlock* location) {
	
	TrieNode* current = _root;
	
	for (std::string::const_iterator it = location->getPrefix().begin();
		it != location->getPrefix().end();
		++it) {
		char ch = *it;
		if (current->children.find(ch) == current->children.end()) {
			current->children[ch] = new TrieNode();
		}
		current = current->children[ch];
	}

	if (current->location != NULL) {
		LOG_DEBUG << "Overwriting existing LocationBlock for prefix: " + location->getPrefix();
		delete current->location;
		current->location = NULL;
	}
	current->isEndOfPrefix = true;
	current->location = location;
}

/**
 * @brief (C++98) Finds the LocationBlock matching the longest possible prefix of the given URI.
 * @details Traverses the Trie using the URI characters. If multiple prefixes match
 * (e.g., "/" and "/images" for URI "/images/logo.png"), it returns the LocationBlock
 * associated with the longer prefix ("/images" in the example) if it exists.
 * @param uri The URI/path string to search for.
 * @return Pointer to the LocationBlock for the longest valid prefix found, or NULL if no match.
 */

LocationBlock* Trie::search(const std::string& uri) const {

	TrieNode* current = _root;
	LocationBlock* longestMatch = NULL;

	for (std::string::const_iterator it = uri.begin();
		it != uri.end();
		++it)
	{
		char ch = *it;
		if (current->children.find(ch) == current->children.end())
			break;
		current = current->children[ch];
		if(current->isEndOfPrefix)
			longestMatch = current->location;
	}
	return longestMatch;
}

TrieNode::TrieNode(): isEndOfPrefix(false), location(NULL) {LOG_DEBUG << "TrieNode created";}

TrieNode::TrieNode(const TrieNode& other) {
	LOG_DEBUG << "TrieNode deep copied";
	if (this != &other) {
		isEndOfPrefix = other.isEndOfPrefix;
		if (other.location)
			location = new LocationBlock(*other.location);
		else
			location = NULL;
		for (std::map<char, TrieNode*>::const_iterator it = other.children.begin();
			it != other.children.end();
			++it) {
			children[it->first] = new TrieNode(*(it->second));
		}
	}
}

TrieNode& TrieNode::operator=(const TrieNode& other) {
	LOG_DEBUG << "TrieNode copy assignment operator called";
	if (this == &other)
		return *this;

	for (std::map<char, TrieNode*>::iterator it = children.begin(); it != children.end(); ++it) {
		delete it->second;
	}
	children.clear();

	delete location;
	location = NULL;

	isEndOfPrefix = other.isEndOfPrefix;
	if (other.location != NULL)
		location = new LocationBlock(*other.location);
	else
	 	location = NULL;

	for (std::map<char, TrieNode*>::const_iterator it = other.children.begin();
		it != other.children.end();
		++it) {
		children[it->first] = new TrieNode(*(it->second));
	}
	return *this;
}

TrieNode::~TrieNode() {
	for (std::map<char, TrieNode*>::iterator it = children.begin();
	it != children.end();
	++it)
	{
		delete it->second;
		it->second = NULL;
	}
	delete location;
	location = NULL;
	LOG_DEBUG << "TrieNode deleted";
	children.clear();
}

Trie::Trie() : _root(new TrieNode()) {LOG_DEBUG << "Trie created";}

Trie& Trie::operator=(const Trie& other) {
	LOG_DEBUG << "Trie copy assignment operator called";
	if (this == &other)
		return *this;

	delete _root;
	_root = NULL;

	if (other._root != NULL)
		_root = new TrieNode(*other.getRoot());
	else
		_root = new TrieNode();
	return *this;
}

Trie::Trie(const Trie& other): _root(new TrieNode(*other.getRoot())) {LOG_DEBUG << "Trie deep copied";}

Trie::~Trie() {LOG_DEBUG << "Trie deleted"; delete _root;}


TrieNode*	Trie::getRoot() const {return _root;}

std::string Trie::toJson(int indentLevel) const {
    std::stringstream ss;
    std::string ind = indent(indentLevel);
    std::string ind2 = indent(indentLevel + 1);
    std::string ind3 = indent(indentLevel + 2);

    printTriePathsJson(ss, _root, "", indentLevel + 1);
    return ss.str();
}

void Trie::printTriePathsJson(std::stringstream& ss, TrieNode* node, const std::string& currentPath, int indentLevel) const {
    if (!node) {
        return;
    }

    std::string ind = indent(indentLevel);
    std::string ind2 = indent(indentLevel + 1);

    if (node->isEndOfPrefix && node->location) {
        ss << ind << "\"" << currentPath << "\": {\n";
        ss << ind2 << "\"directives\": {\n" << node->location->toJson(indentLevel + 1) << "\n";
        ss << ind2 << "}\n";
		ss << ind << "}";
        if (!node->children.empty()) {
            ss << ",\n";
        }
    }

    bool firstChild = true;
    for (std::map<char, TrieNode*>::const_iterator it = node->children.begin(); it != node->children.end(); ++it) {
        std::string nextPath = currentPath + it->first;
        if (!firstChild && !(node->isEndOfPrefix && node->location)) {
            ss << ",\n";
        }
        printTriePathsJson(ss, it->second, nextPath, indentLevel);
        firstChild = false;
    }
}