#pragma once
#include <stack>
#include "bytecode.h"
#include "config.h"

namespace bc
{
	namespace vm
	{
		struct bcRTVal
		{
			unsigned int type;
		};
		
		struct bcRTInt : public bcRTVal
		{
			int i;
		};

		struct bcRTFloat : public bcRTVal
		{
			float f;
		};

		struct bcRTString : public bcRTVal
		{
			std::string s;
		};

		struct bcRTBool : public bcRTVal
		{
			bool b;
		};

		class bcVM
		{
		public:
			unsigned int exec(unsigned int instructions);

			//runtime data
			bcVal									reg[bcMaxRegisters];	//vm registers
			bool									regFlags[bcMaxRegisters];
			std::vector<bcByteCode>					istream;				//(global) instruction stream
			std::vector<bcByteCode>					fistream;				//function instructions
			std::stack<bcVal>						stack;					//runtime stack
			std::unordered_map<int,bcRTVal*>		rtvals;					//runtime values
			unsigned int							pc;						//program counter
			
			//debug data
			std::unordered_map<std::string,parse::bcSymbol>	symtab;			//symbol table
		};

	}
}