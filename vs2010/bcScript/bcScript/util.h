#pragma once
#include <string>
#include <sstream>
#include <vector>
#include "types.h"

namespace bc
{
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

		//RPN helpers
		int GetPrecedence(bcToken);
		int IsOperator(bcToken);
	}
}