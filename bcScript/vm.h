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

		//instructions
		inline void ocNop(bcExecContext*);	
		inline void ocMov(bcExecContext*);		//copy new memory
		inline void ocLr(bcExecContext*);		//load value into a register
		inline void ocLs(bcExecContext*);		//load value into the stack
		inline void ocLrfs(bcExecContext*);		//load value from stack into a register
		inline void ocLsfr(bcExecContext*);		//load value from register into the stack
		inline void ocPush(bcExecContext*);		//push onto the stack
		inline void ocPop(bcExecContext*);		//pop from stack
		inline void ocCmp(bcExecContext*);
		inline void ocJmp(bcExecContext*);
		inline void ocJe(bcExecContext*);
		inline void ocJne(bcExecContext*);
		inline void ocJg(bcExecContext*);
		inline void ocJl(bcExecContext*);
		inline void ocJge(bcExecContext*);
		inline void ocJle(bcExecContext*);
		inline void ocPlus(bcExecContext*);
		inline void ocMinus(bcExecContext*);
		inline void ocMult(bcExecContext*);
		inline void ocDiv(bcExecContext*);
		inline void ocExpo(bcExecContext*);
		inline void ocMod(bcExecContext*);
		inline void ocInc(bcExecContext*);
		inline void ocDec(bcExecContext*);
		inline void ocAnd(bcExecContext*);
		inline void ocOr(bcExecContext*);
		inline void ocXor(bcExecContext*);
		inline void ocNot(bcExecContext*);
		inline void ocShfl(bcExecContext*);
		inline void ocShfr(bcExecContext*);
	}
}