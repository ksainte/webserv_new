#include "../inc/ToJson.hpp"
#include "../inc/Logger.hpp"

std::string ToJson::indent(int level) const {
	std::string s;
	for (int i = 0; i < level; i++) {
		s += "\t";
	}
	return s;
}

ToJson::~ToJson() {LOG_DEBUG << "ToJson deleted";}