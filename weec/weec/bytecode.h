#ifndef WC_BYTECODE_H
#define WC_BYTECODE_H
#include <vector>

namespace wc
{
	namespace bytecode
	{
		enum wcOpcode
		{
			oc_nop,
			//load memory
			oc_mov, oc_lr, oc_lrfs, oc_ls, oc_lsfr,
			//flags
			oc_setflag, oc_readflag,
			//stack io
			oc_push, oc_pushfs, oc_pushfr, oc_pop, oc_popr,
			//conditional jumps
			oc_cmp, oc_jmp, oc_je, oc_jne, oc_jg, oc_jl, oc_jge, oc_jle,
			//stack math/logical operations
			oc_assign, oc_plus, oc_minus, oc_mult, oc_div, oc_expo, oc_mod, oc_inc, oc_dec,
			oc_and, oc_or, oc_xor, oc_not, oc_shfl, oc_shfr,
			//function calls
			oc_call, oc_ret, oc_callvm,
			//stackframes 
			oc_pushsf, oc_popsf,
			oc_pause, oc_halt
		};

		struct wcInstruction
		{
		public:
			unsigned short int opCode;
		};

		struct wcInstructionPlusOperand : wcInstruction
		{
		public:
			int operand1;
		};

		struct wcInstructionPlusOperands : wcInstruction
		{
		public:
			int operand1,operand2;
		};

		enum wcTargetPlatform
		{
			ct_bytecode, ct_simple_bytecode, ct_hosted_exe, ct_ansi_c, ct_x86
		};

		struct wcExecContext
		{
			wcExecContext();

			wcInstruction getInstr()
				{return iStream[pc];};
			bool execStopped()
				{return halt;};

			int contextID;
			int pc;
			bool halt;
			std::vector<wcInstruction> iStream;
			wcTargetPlatform target;
		};


	}
}
#endif