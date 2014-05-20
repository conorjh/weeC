#pragma once
#include <stack>
#include <bitset>
#include "config.h"
#include "bytecode.h"

namespace bc
{
	namespace vm
	{
		struct bcRTVal
		{
			unsigned int type;
			int val;
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

		enum bcReg
		{
			pc,
			sp,
			eax,
			ret,
			cmp1,
			cmp2,
			t1 = 30,
			t2 = 31
		};


		class bcVM
		{
		public:
			bcVM();
			unsigned int exec(unsigned int instructions);
			//void load(bcExecContext*);

			//runtime data
			bcExecContext* con;

			//debug data
			std::unordered_map<std::string,parse::bcSymbol>	symtab;			//symbol table
		};

	}
}