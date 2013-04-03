#pragma once
#include <stack>
#include <vector>
#include "types.h"

namespace bc
{
	namespace vm
	{
		class bcVM
		{
		public:
			bcVM();
			bcVM(vector<bcInstr>);

			int Init(vector<bcInstr>);
			int Run();
			int RunInstr();
			int RunInstr_mov();
			int RunInstr_push();
			int RunInstr_pop();
			int RunInstr_call();
			int RunInstr_add();
			int RunInstr_sub();
			int RunInstr_div();
			int RunInstr_mult();
			int RunInstr_pow();
			int RunInstr_mod();
			int RunInstr_inc();
			int RunInstr_dec();
			int RunInstr_ltn();
			int RunInstr_gtn();
			int RunInstr_leq();
			int RunInstr_geq();
			int RunInstr_eq();
			int RunInstr_neq();
			int RunInstr_land();
			int RunInstr_lor();
	
			//bcVal operations
			bool Add(bcVal*,bcVal*,bcVal*);

			//helper
			bcValType ottort(bcOperandType);

			stack<bcVal> stk;					//runtime stack
			vector<bcInstr> istream;			//instruction stream
			vector<bcInstr>::iterator pc;		//pointer to current instruction
		};
	}
}