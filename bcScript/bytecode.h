#pragma once
#include "parser.h"

namespace bc
{
	namespace vm
	{
		enum bcOpCode
		{
			oc_nop,
			oc_mov,
			oc_push,oc_pop,
			oc_je,oc_jne,oc_jg,oc_jl,oc_jge,oc_jle,
			oc_plus,oc_minus,oc_mult,oc_div,oc_exp,oc_mod,oc_inc,oc_dec,
			oc_and,oc_or,oc_xor,oc_not,oc_shfl,oc_shfr,
			oc_call,oc_ret,oc_callvm,
			oc_pause,oc_halt
		};

		enum bcValType
		{
			vt_null,vt_mem,vt_reg,vt_stack,vt_astack,vt_int,vt_float,vt_string,vt_bool,vt_funccall,vt_vmcall
		};

		struct bcVal
		{
			bcValType type;
			int val;	
		};

		struct bcByteCode
		{
			bcOpCode op;
			bcVal arg1,arg2;
		};

		class bcByteCodeGen
		{
		public:
			bcByteCodeGen();
			void gen();

			void addByteCode(bcByteCode);
			void addByteCode(bcOpCode);
			void addByteCode(bcOpCode,bcValType);
			void addByteCode(bcOpCode,bcValType,bcValType);
			void addByteCode(bcOpCode,bcValType,unsigned int);
			void addByteCode(bcOpCode,bcValType,unsigned int,bcValType,unsigned int);
			void addByteCode(bcOpCode,bcVal);
			void addByteCode(bcOpCode,bcVal,bcVal);
			
			tree<parse::bcParseNode>::iterator pi;	//parse index
			parse::bcAST* ast;
			std::vector<bcByteCode>* istream;
			std::vector<bcByteCode>* fistream;
			bool inDecFunc;
		};

		void genStatement(bcByteCodeGen*);
		void genDecFunc(bcByteCodeGen*);
		void genBlock(bcByteCodeGen*);
		void genDecVar(bcByteCodeGen*);
		void genDecFunc(bcByteCodeGen*);
		void genExp(bcByteCodeGen*);
		void genRpnToByteCode(bcByteCodeGen*,std::vector<parse::bcParseNode*>*);
		void genNodeToByteCode(bcByteCodeGen*,parse::bcParseNode*);
		bcValType getValType(parse::bcSymbol*);
	}
}