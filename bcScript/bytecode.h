#pragma once
#include "parser.h"
#include "config.h"
#include <bitset>
#include <stack>

namespace bc
{
	namespace vm
	{
		enum bcOpCode
		{
			oc_nop,		
			oc_mov,			
			oc_push,oc_pop,	
			oc_cmp,oc_je,oc_jne,oc_jg,oc_jl,oc_jge,oc_jle,
			oc_plus,oc_minus,oc_mult,oc_div,oc_expo,oc_mod,oc_inc,oc_dec,
			oc_and,oc_or,oc_xor,oc_not,oc_shfl,oc_shfr,
			oc_call,oc_ret,oc_callvm,
			oc_pause,oc_halt
		};

		struct bcByteCode
		{
			bcByteCode();
			bcOpCode op;
			int arg1,arg2;
		};

		class bcStack
		{
		public:
			bcStack();			
			void push(int);
			int pop();
			int top();
			int size();
			void clear();
			std::vector<int> cont;
		};

		class bcExecContext
		{
		public:
			bcExecContext();
			int										reg[bcMaxRegisters];	//vm registers
			std::bitset<bcMaxRegisters>				regFlags;				//flags for each register
			std::vector<bcByteCode>					istream;				//instruction stream
			bcStack									stack;					//runtime stack
			std::unordered_map<std::string,int>		newstore;				//dynamic memory
			bool									halt;					//has execution halted
		};

		class bcByteCodeGen
		{
		public:
			bcByteCodeGen();
			bcExecContext* gen();

			unsigned int addByteCode(bcByteCode);
			unsigned int addByteCode(bcOpCode);
			unsigned int addByteCode(bcOpCode,bcValType);
			unsigned int addByteCode(bcOpCode,bcValType,bcValType);
			unsigned int addByteCode(bcOpCode,bcValType,unsigned int);
			unsigned int addByteCode(bcOpCode,bcValType,unsigned int,bcValType,unsigned int);
			unsigned int addByteCode(bcOpCode,bcVal);
			unsigned int addByteCode(bcOpCode,bcVal,bcVal);
			bcByteCode* getByteCode(unsigned int instr);
			bcByteCode* getByteCode(unsigned int instr,bool isFunc);
			
			tree<parse::bcParseNode>::iterator pi;	//parse index
			parse::bcAST* ast;
			std::vector<bcByteCode>* istream;
			bool inDecFunc;
		};

		void genStatement(bcByteCodeGen*);
		void genDecFunc(bcByteCodeGen*);
		void genBlock(bcByteCodeGen*);
		void genDecVar(bcByteCodeGen*);
		void genDecFunc(bcByteCodeGen*);
		void genDecParamList(bcByteCodeGen*);
		void genDecNamespace(bcByteCodeGen*);
		void genIf(bcByteCodeGen*);
		void genExp(bcByteCodeGen*);
		void genRpnToByteCode(bcByteCodeGen*,std::vector<parse::bcParseNode*>*);
		void genNodeToByteCode(bcByteCodeGen*,parse::bcParseNode*);
	
		bcValType getValType(parse::bcSymbol*);
		int getValTypeSize(bcValType);
	}
}