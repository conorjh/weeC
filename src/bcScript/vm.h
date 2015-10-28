#ifndef BC_VM_H
#define BC_VM_H

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
			
			bcExecContext* con;
		};

		//instructions
		void ocNop(bcExecContext*);	
		void ocMov(bcExecContext*);			//copy new memory
		void ocSf(bcExecContext*);			//set flag
		void ocRf(bcExecContext*);			//read flag to stack
		void ocLr(bcExecContext*);			//load value into a register
		void ocLs(bcExecContext*);			//load value into the stack
		void ocLrfs(bcExecContext*);		//load value from stack into a register
		void ocLsfr(bcExecContext*);		//load value from register into the stack
		void ocPush(bcExecContext*);		//push onto the stack
		void ocPushfs(bcExecContext*);		//push onto the stack, from a given stackindex
		void ocPushfr(bcExecContext*);		//push onto the stack, from a given register
		void ocPop(bcExecContext*);			//pop from stack
		void ocPopr(bcExecContext*);		//pop from stack to a given register
		void ocCmp(bcExecContext*);			//compare top 2 stack items, store in cmp1
		void ocJmp(bcExecContext*);			//uncondtional jump
		void ocJe(bcExecContext*);		
		void ocJne(bcExecContext*);
		void ocJg(bcExecContext*);
		void ocJl(bcExecContext*);
		void ocJge(bcExecContext*);
		void ocJle(bcExecContext*);
		void ocAssign(bcExecContext*);
		void ocPlus(bcExecContext*);
		void ocMinus(bcExecContext*);
		void ocMult(bcExecContext*);
		void ocDiv(bcExecContext*);
		void ocExpo(bcExecContext*);
		void ocMod(bcExecContext*);
		void ocInc(bcExecContext*);
		void ocDec(bcExecContext*);
		void ocAnd(bcExecContext*);
		void ocOr(bcExecContext*);
		void ocXor(bcExecContext*);
		void ocNot(bcExecContext*);
		void ocShfl(bcExecContext*);
		void ocShfr(bcExecContext*);
		void ocCall(bcExecContext*);
		void ocCallvm(bcExecContext*);
		void ocPause(bcExecContext*);
		void ocHalt(bcExecContext*);
		void ocRet(bcExecContext*);
		void ocPushsf(bcExecContext*);
		void ocPopsf(bcExecContext*);
	}
}

#endif