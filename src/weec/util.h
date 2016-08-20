#ifndef WEEC_UTIL_H
#define WEEC_UTIL_H
#include <string>
#include <sstream>
#include <vector>

namespace wc
{
	namespace lex { struct wcToken; }
	namespace util
	{
		//conversion
		std::string wcitos(int in);
		int wcstoi(std::string in);
		std::string wcftos(float in);
		float wcstof(std::string in);
		std::string getFileExt(std::string);

		//str format
		std::string toLower(std::string in);
		std::string toLower(char in);

		//file
		bool fileExists(const char*);
		bool readFile(const char*, std::vector<std::string>*);

	}
}
#endif //WEEC_UTIL_H
