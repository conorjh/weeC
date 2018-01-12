#ifndef WC_BYTECODE_H
#define WC_BYTECODE_H
#include <vector>
#include <unordered_map>
#include <memory>
#include "debug.h"

namespace wc
{
	namespace vm
	{

	}

	namespace bytecode
	{
		enum wcOpcode : unsigned short int
		{
			oc_nop, oc_push, oc_pop, oc_pushstk, oc_pushr, oc_popstk, oc_popr,
			//conditional jumps
			oc_cmp, oc_jmp, oc_je, oc_jne, oc_jg, oc_jl, oc_jge, oc_jle,
			//stack math/logical operations
			oc_assign, oc_plus, oc_minus, oc_mult, oc_div, oc_expo, oc_mod, oc_inc, oc_dec,
			oc_and, oc_or, oc_xor, oc_not, oc_shfl, oc_shfr,
			//function calls
			oc_halt, oc_call, oc_ret, oc_callvm,

			//load memory
			oc_mov, oc_lr, oc_lrfs, oc_ls, oc_lsfr,
			//flags
			oc_setflag, oc_readflag,
			//stack io
			oc_pushfs, oc_pushfr,  
			//stackframes 
			oc_pushsf, oc_popsf,
			oc_pause
		};


		struct wcInstruction
		{
		public:
			~wcInstruction();
			wcInstruction();
			wcInstruction(unsigned short int);
			wcOpcode opcode;
		};

		struct wcInstructionPlusOperand : wcInstruction
		{
		public:
			~wcInstructionPlusOperand();
			wcInstructionPlusOperand();
			wcInstructionPlusOperand(unsigned short int,int);
			int operand1;
		};

		struct wcInstructionPlusOperands : wcInstruction
		{
		public:
			~wcInstructionPlusOperands();
			wcInstructionPlusOperands();
			wcInstructionPlusOperands(unsigned short int, int, int);
			int operand1,operand2;
		};

		enum wcTargetPlatform
		{
			ct_bytecode, ct_simple_bytecode, ct_hosted_exe, ct_ansi_c, ct_x86
		};

		struct wcExecContextRegisters
		{
			int& operator[](int);
			int pc,t1,t2,cmp,instr, halt;
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

		struct wcChunk
		{
			wcChunk();
			virtual int i();
			virtual float f();
			virtual std::string s();
		};

		struct wcChunki : public wcChunk
		{
			wcChunki(int);
			virtual int i();
			virtual float f();
			virtual std::string s();
			int _i;
		};

		struct wcChunkf : public wcChunk
		{
			wcChunkf(float);
			virtual int i();
			virtual float f();
			virtual std::string s();
			float _f;
		};

		struct wcChunks : public wcChunk
		{
			wcChunks(std::string);
			virtual int i();
			virtual float f();
			virtual std::string s();
			std::string _s;
		};

		struct wcExecStack
		{
			void push(wcChunk);
			wcChunk pop();
			wcChunk& peek(int);
			wcChunk top();
			void set(int index, wcChunk value);
			int size();
			void clear();

			std::vector<std::shared_ptr<wcChunk>> container;
		};

		struct wcStackFrame
		{
			unsigned int id;
			std::string fullyQualifiedIdent;
			std::unordered_map<int, wcChunk> symbols;
		};

		struct wcExecContext
		{
			wcExecContext();
			std::shared_ptr<wcInstruction> getInstr()	{return instructions[registers.pc];};
			bool execStopped()			{return registers.halt;};

			int contextID;
			std::vector<std::shared_ptr<wcInstruction>> instructions;
			wcExecStack stack;
			wcExecContextRegisters registers;
			wcTargetPlatform targetPlatform;
			wcStringTable stringTable;
			debug::wcDebugSymbolTable debugSymbolTable;
		};


	}
}
#endif