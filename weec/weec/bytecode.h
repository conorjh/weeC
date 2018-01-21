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
			oc_nop,
			//stack 
			oc_push, oc_pop, oc_pushstk, oc_pushr, oc_popstk, oc_popr,
			oc_setstk,
			//conditional jumps
			oc_cmp, oc_jmp, oc_je, oc_jne, oc_jg, oc_jl, oc_jge, oc_jle,
			//stack math/logical operations
			oc_assign, oc_plus, oc_minus, oc_mult, oc_div, oc_expo, oc_mod, oc_inc, oc_dec,
			oc_and, oc_or, oc_xor, oc_not, oc_shfl, oc_shfr,
			//function calls
			oc_halt, oc_call, oc_ret, oc_callvm,
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

		enum wcRegisterTitles
		{
			pc, t1, t2, cmp, eax, instr, halt
		};

		struct wcExecContextRegisters
		{
			int& operator[](int);
			int pc,t1,t2,cmp,eax,instr,halt;
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
			void push(wcChunk);		void push(wcChunki);
			void push(wcChunkf);	void push(wcChunks);

			std::shared_ptr<wcChunk> pop();
			std::shared_ptr<wcChunki> popi();
			std::shared_ptr<wcChunkf> popf();
			std::shared_ptr<wcChunks> pops();

			std::shared_ptr<wcChunk> peek(int);
			std::shared_ptr<wcChunki> peeki(int);
			std::shared_ptr<wcChunkf> peekf(int);
			std::shared_ptr<wcChunks> peeks(int);

			std::shared_ptr<wcChunk> top();
			std::shared_ptr<wcChunki> topi();
			std::shared_ptr<wcChunkf> topf();
			std::shared_ptr<wcChunks> tops();

			void set(int index, wcChunk value);
			void set(int index, wcChunki value);
			void set(int index, wcChunkf value);
			void set(int index, wcChunks value);

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