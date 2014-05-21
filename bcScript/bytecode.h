#pragma once
#include "parser.h"
#include "config.h"
#include <bitset>
#include <stack>

namespace bc
{
	namespace vm
	{
		enum bcReg
		{
			pc,
			sp,
			tos,
			eax,
			ret,
			cmp1,
			cmp2,
			t1 = 30,
			t2 = 31
		};

		enum bcOpCode
		{
			oc_nop,		
			//load memory
			oc_mov,oc_lr,oc_lrfs,oc_ls,oc_lsfr,
			//stack operations
			oc_push,oc_pop,
			//conditional jumps
			oc_cmp,oc_jmp,oc_je,oc_jne,oc_jg,oc_jl,oc_jge,oc_jle,
			//arithmetic
			oc_plus,oc_minus,oc_mult,oc_div,oc_expo,oc_mod,oc_inc,oc_dec,
			//logical operations
			oc_and,oc_or,oc_xor,oc_not,oc_shfl,oc_shfr,
			//function calls
			oc_call,oc_ret,oc_callvm,
			oc_pause,oc_halt
		};

		struct bcByteCode
		{
			bcByteCode();
			bcOpCode op;
			int arg1,arg2;
		};

		class bcExecContext;

		class bcStack
		{
		public:
			bcStack();			
			void push(int);	
			void push(bcExecContext*,int);
			int pop();
			int pop(bcExecContext*);
			int top();
			int size();
			int* at(int);
			void clear();
			void clear(bcExecContext*);
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
			unsigned int addByteCode(bcOpCode,int);
			unsigned int addByteCode(bcOpCode,int,int);
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