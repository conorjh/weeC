#ifndef WEEC_BYTECODE_H
#define WEEC_BYTECODE_H
#include "parser.h"
#include "config.h"
#include <stack>
#include <bitset>

namespace wc
{
	namespace vm
	{
		enum wcReg
		{
			pc,			//program counter
			sp,			//stack pointer to start of local scope
			tos,		//top of stack
			eax,		//last expression result
			ret,		//return value
			cmp,		//compare result
			sti,		//stack index
			hlt,		//halt register
			r20, r21, r22, r23, r24, r25, r26, r27, r28, r29,	//general registers
			t1 = 30,	//temp var1
			t2 = 31		//temp var2
		};

		enum wcOpCode
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

		struct wcByteCode
		{
			wcByteCode();
			wcByteCode(wcOpCode);
			wcByteCode(wcOpCode, int);
			wcByteCode(wcOpCode, int, int);
			wcByteCode(int);
			wcByteCode(int, int);
			wcByteCode(int, int, int);
			wcOpCode op;
			int arg1, arg2;
		};

		class wcExecContext;

		class wcStack
		{
		public:
			wcStack();
			void push(int);
			void push(wcExecContext*, int);
			int pop();
			int pop(wcExecContext*);
			int top();
			int size();
			int* at(int);
			void clear();
			void clear(wcExecContext*);
			std::vector<int> cont;
		};

		struct wcStoredStackFrame
		{
			wcStack data;
		};

		struct wcVMSymbol
		{
			std::string ident;		//variable identifier
			int stackFrame;			//id of the stack frame in which it belongs
			int offset;				//offset within the stackframe
		};

		class wcExecContext
		{
		public:
			wcExecContext();
			void clear();
			unsigned int							id;
			int										reg[wcMaxRegisters];		//vm registers
			std::bitset<wcMaxRegisters>				regFlags;					//flags for each register
			std::vector<wcByteCode>					istream;					//instruction stream
			wcStack									stack;						//runtime stack
			std::unordered_map<std::string, int>		newstore;					//dynamic memory
			bool									halt;						//has execution halted

																				//debug data
			std::unordered_map<std::string, vm::wcVMSymbol>	symTab;				//symbol table
			std::unordered_map<int, std::vector<int>>		stackFrames;		//stackframe id to vector of stack values
			std::unordered_map<int, wcStoredStackFrame>		storedFrames;		//stackframe data - maps stackframe id to storedStackFrames
		};

		struct wcByteCodeChunk
		{
			unsigned int start, end;
			std::vector<wcByteCode>* istream;
		};

		class wcByteCodeGen
		{
		public:
			wcByteCodeGen();
			wcExecContext* gen();
			int getError();

			unsigned int addByteCode(wcByteCode);
			unsigned int addByteCode(wcOpCode);
			unsigned int addByteCode(wcOpCode, int);
			unsigned int addByteCode(wcOpCode, int, int);
			wcByteCode* getByteCode(unsigned int instr);
			wcByteCode* getByteCode(unsigned int instr, bool isFunc);

			tree<parse::wcParseNode>::iterator pi;	//parse index
			parse::wcAST* ast;
			std::vector<wcByteCode>* istream;
			std::unordered_map<std::string, std::vector<wcByteCode>*> fistream;	//method string signature to functions istream
			wcExecContext* output;
			bool inDecFunc;
			int ifJmpIndex;
			int errorCode;
		};

		void genStatement(wcByteCodeGen*);
		void genDecFunc(wcByteCodeGen*);
		void genBlock(wcByteCodeGen*);
		void genDecVar(wcByteCodeGen*);
		void genDecFunc(wcByteCodeGen*);
		void genDecParamList(wcByteCodeGen*);
		void genDecNamespace(wcByteCodeGen*);
		void genFuncCall(wcByteCodeGen*);
		void genIf(wcByteCodeGen*);
		void genReturn(wcByteCodeGen*);
		void genExp(wcByteCodeGen*);
		void genRpnToByteCode(wcByteCodeGen*, std::vector<parse::wcParseNode*>*);
		void genNodeToByteCode(wcByteCodeGen*, parse::wcParseNode*);
		void genStackFrames(wcByteCodeGen*, wcExecContext*);
		void genFuncIstreams(wcByteCodeGen*, wcExecContext*);
		wcValType getValType(parse::wcSymbol*);
		int getValTypeSize(wcValType);
		void adjustJumps(wcByteCodeGen*, int, int, int);
	}
}
#endif