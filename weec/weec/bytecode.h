#ifndef WC_BYTECODE_H
#define WC_BYTECODE_H
#include <vector>
#include <unordered_map>
#include "debug.h"

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
			wcInstruction();
			wcInstruction(unsigned short int);
		public:
			unsigned short int opcode;
		};

		struct wcInstructionPlusOperand : wcInstruction
		{
			wcInstructionPlusOperand();
			wcInstructionPlusOperand(unsigned short int,int);
		public:
			int operand1;
		};

		struct wcInstructionPlusOperands : wcInstruction
		{
			wcInstructionPlusOperands();
			wcInstructionPlusOperands(unsigned short int, int, int);
		public:
			int operand1,operand2;
		};

		enum wcTargetPlatform
		{
			ct_bytecode, ct_simple_bytecode, ct_hosted_exe, ct_ansi_c, ct_x86
		};

		struct wcExecContextRegisters
		{
			int pc,t1,t2,cmp;
			bool halt;
		}; 

		struct wcStringTable
		{
			std::string getString(unsigned int); 
			unsigned int getIndex(std::string);
			bool doesIndexExist(unsigned int);
			bool doesStringExist(std::string);
			unsigned int addEntry(std::string);

			std::unordered_multimap<std::string, unsigned int> strTable;
			std::unordered_multimap<unsigned int, std::string> intTable;
		};

		struct wcStack
		{
			void push(int);
			int pop();
			int peek(int);
			int top();
			int size();
			void clear();

			std::vector<int> container;
		};

		struct wcExecContext
		{
			wcExecContext();

			wcInstruction getInstr()	{return instructions[registers.pc];};
			bool execStopped()			{return registers.halt;};

			int contextID;
			std::vector<wcInstruction> instructions;
			wcStack stack;
			wcExecContextRegisters registers;
			wcTargetPlatform targetPlatform;
			wcStringTable stringTable;
			debug::wcDebugSymbolTable debugSymbolTable;
		};


	}
}
#endif