#pragma once
#include <stack>
#include "bytecode.h"

namespace bc
{
	namespace vm
	{
		class bcVM
		{
		public:
			unsigned int exec(unsigned int instructions);

			std::vector<bcByteCode>	istream;
			std::stack<bcVal>		stack;
			unsigned int pc;
		};

	}
}