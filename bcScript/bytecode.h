#pragma once
#include "parser.h"

namespace bc
{
	namespace vm
	{
		enum bcOpCode
		{
			oc_null,
			oc_mov,
			oc_push,oc_pop,
			oc_je,oc_jne,oc_jg,oc_jl,oc_jge,oc_jle,
			oc_plus,oc_minus,oc_mult,oc_div,oc_exp,oc_mod,oc_inc,oc_dec,
			oc_and,oc_or,oc_xor,oc_not,oc_shfl,oc_shfr,
			oc_call,oc_ret,oc_callvm,
			oc_pause,oc_halt
		};

		enum bcOperandType
		{
			ot_mem,ot_stack,ot_astack,ot_int,ot_float,int_string,ot_funccall,ot_vmcall
		};

		struct bcVal
		{
			unsigned int v;
		};

		struct bcOperand
		{
			bcOperandType type;
			bcVal val;	
		};

		struct bcByteCode
		{
			bcOpCode op;
			bcOperand arg1,arg2;
		};

		class bcByteCodeGen
		{
		public:
			void gen();

			parse::bcAST ast;
		};
	}
}