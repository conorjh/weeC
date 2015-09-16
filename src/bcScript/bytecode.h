#ifndef BC_BYTECODE_H
#define BC_BYTECODE_H
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
			pc,			//program counter
			sp,			//stack pointer to start of local scope
			tos,		//top of stack
			eax,		//
			ret,		//return address
			cmp,		//compare result
			sti,		//stack index
			hlt,		//halt register
			r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,	//general registers
			t1 = 30,	//temp var1
			t2 = 31		//temp var2
		};

		enum bcOpCode
		{
			oc_nop,		
			//load memory
			oc_mov,oc_lr,oc_lrfs,oc_ls,oc_lsfr,
			//flags
			oc_sf,oc_rf,
			//stack io
			oc_push,oc_pushfs,oc_pop,
			//conditional jumps
			oc_cmp,oc_jmp,oc_je,oc_jne,oc_jg,oc_jl,oc_jge,oc_jle,
			//stack math/logical operations
			oc_assign,oc_plus,oc_minus,oc_mult,oc_div,oc_expo,oc_mod,oc_inc,oc_dec,
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
			int getError();

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
			int ifJmpIndex;
			int errorCode;
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
		void adjustJumps(bcByteCodeGen*,int,int,int);
	}
}
#endif