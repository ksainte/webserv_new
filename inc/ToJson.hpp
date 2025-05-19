#ifndef TO_JSON_H
#define TO_JSON_H
#include <string>
class ToJson {
	public:
		
		virtual ~ToJson();
		virtual std::string toJson(int indentLevel = 0) const = 0;
	protected:
		std::string indent(int level) const;
};
#endif