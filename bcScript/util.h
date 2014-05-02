#pragma once
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

		//str format
		std::string bctolower(std::string in);
		std::string bctolower(char in);

		//file
		bool bcfexists(const char*);
		bool bcreadfile(const char*,std::vector<std::string>*);

	}
}