#ifndef WEEC_VM_H
#define WEEC_VM_H
#include <stack>
#include <bitset>
#include "config.h"
#include "bytecode.h"

namespace wc
{
	namespace vm
	{
		struct wcRTVal
		{
			unsigned int type;
			int val;
		};

		struct wcRTInt : public wcRTVal
		{
			int i;
		};

		struct wcRTFloat : public wcRTVal
		{
			float f;
		};

		struct wcRTString : public wcRTVal
		{
			std::string s;
		};

		struct wcRTBool : public wcRTVal
		{
			bool b;
		};


		class wcVM
		{
		public:
			wcVM();
			unsigned int exec(unsigned int instructions);

			wcExecContext* con;
		};

		//instructions
		void ocNop(wcExecContext*);
		void ocMov(wcExecContext*);			//copy new memory
		void ocSf(wcExecContext*);			//set flag
		void ocRf(wcExecContext*);			//read flag to stack
		void ocLr(wcExecContext*);			//load value into a register
		void ocLs(wcExecContext*);			//load value into the stack
		void ocLrfs(wcExecContext*);		//load value from stack into a register
		void ocLsfr(wcExecContext*);		//load value from register into the stack
		void ocPush(wcExecContext*);		//push onto the stack
		void ocPushfs(wcExecContext*);		//push onto the stack, from a given stackindex
		void ocPushfr(wcExecContext*);		//push onto the stack, from a given register
		void ocPop(wcExecContext*);			//pop from stack
		void ocPopr(wcExecContext*);		//pop from stack to a given register
		void ocCmp(wcExecContext*);			//compare top 2 stack items, store in cmp1
		void ocJmp(wcExecContext*);			//uncondtional jump
		void ocJe(wcExecContext*);
		void ocJne(wcExecContext*);
		void ocJg(wcExecContext*);
		void ocJl(wcExecContext*);
		void ocJge(wcExecContext*);
		void ocJle(wcExecContext*);
		void ocAssign(wcExecContext*);
		void ocPlus(wcExecContext*);
		void ocMinus(wcExecContext*);
		void ocMult(wcExecContext*);
		void ocDiv(wcExecContext*);
		void ocExpo(wcExecContext*);
		void ocMod(wcExecContext*);
		void ocInc(wcExecContext*);
		void ocDec(wcExecContext*);
		void ocAnd(wcExecContext*);
		void ocOr(wcExecContext*);
		void ocXor(wcExecContext*);
		void ocNot(wcExecContext*);
		void ocShfl(wcExecContext*);
		void ocShfr(wcExecContext*);
		void ocCall(wcExecContext*);
		void ocCallvm(wcExecContext*);
		void ocPause(wcExecContext*);
		void ocHalt(wcExecContext*);
		void ocRet(wcExecContext*);
	}
}

#endif //WEEC_VM_H
