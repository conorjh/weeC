#ifndef BC_UTIL_H
#define BC_UTIL_H

#include <string>
#include <sstream>
#include <vector>


namespace bc
{
	namespace lex{struct bcToken;}
	namespace util
	{
		//conversion
		std::string bcitos(int in);
		int bcstoi(std::string in);
		std::string bcftos(float in);
		float bcstof(std::string in);
		std::string getFileExt(std::string);

		//str format
		std::string toLower(std::string in);
		std::string toLower(char in);

		//file
		bool fileExists(const char*);
		bool readFile(const char*,std::vector<std::string>*);

	}
}
#endif