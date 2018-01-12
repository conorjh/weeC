#ifndef WC_SIMPLE_H
#define WC_SIMPLE_H
#include <unordered_map>
#include <xmemory>
#include "vm.h"
#include "bytecode.h"
#include "compiler.h"
#include "api_types.h"

namespace wc
{
	namespace bytecode
	{
		struct wcSimpleStackFrame : public wcStackFrame
		{
		};

		struct wcSimpleExecContext : public wcExecContext
		{
			wcSimpleExecContext();
			wcSimpleExecContext(wcExecContext&&);
			//int a;
		};

		enum wcSimpleOpcode
		{
			soc_nop,
			//stack 
			soc_push, soc_pop, soc_pushstk, soc_pushr, soc_popstk, soc_popr,
			//conditional jumps
			soc_cmp, soc_jmp, soc_je, soc_jne, soc_jg, soc_jl, soc_jge, soc_jle,
			//stack math/logical operations
			soc_assign, soc_plus, soc_minus, soc_mult, soc_div, soc_expo, soc_mod, soc_inc, soc_dec,
			soc_and, soc_or, soc_xor, soc_not, soc_shfl, soc_shfr,
			//function calls
			soc_halt, soc_call, soc_ret, soc_callvm,
		};

		const std::unordered_multimap<wcSimpleOpcode, int> wcSimpleOpcodeOperandCounts=
		{
			{soc_nop,0},{ soc_push,1 },{ soc_pop,0 },{soc_cmp,0}, {soc_jmp,1}, {soc_je,1}, {soc_jne,1}, {soc_jg,1}, {soc_jl,1}, {soc_jge,1}, {soc_jle,1},
			{soc_assign,0}, {soc_plus,0}, {soc_minus,0}, {soc_mult,0}, {soc_div,0}, {soc_expo,0}, {soc_mod,0}, {soc_inc,0}, {soc_dec,0},
			{soc_and,0}, {soc_or,0}, {soc_xor,0}, {soc_not,0}, {soc_shfl,0}, {soc_shfr,0},{soc_call,0}, {soc_ret,0}, {soc_callvm,0}, {soc_halt,1},
		};

		int getSimpOpCount(wcSimpleOpcode);
	}

	namespace codegen
	{
		int genSimpStatement(wc::parse::wcParseIndex&, bytecode::wcSimpleExecContext& output);
		std::vector<std::shared_ptr<bytecode::wcInstruction>> genSimpExpression(wc::parse::wcParseIndex&, bytecode::wcSimpleExecContext& output, std::vector<parse::wcParseNode>* rpnOutput);
		int genSimpIf(wc::parse::wcParseIndex&, bytecode::wcSimpleExecContext& output);
		int genSimpDecVar(wc::parse::wcParseIndex&, bytecode::wcSimpleExecContext& output);

		class wcSimpleBytecodeGen : public wcBaseBytecodeGen
		{
		public:
			virtual bytecode::wcSimpleExecContext genSimple(parse::wcAST&);
		};
	}

	namespace vm
	{
		class wcSimpleVM : public wcBaseVM
		{
		public:
			wcSimpleVM();
			virtual int exec(int handle);
			virtual int execInstruction(bytecode::wcSimpleExecContext& context, std::shared_ptr<bytecode::wcInstruction> instr);

		private:
		};

		//simple bytecode execution methods
		inline void exec_s_push(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_pop(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_pushstk(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_pushr(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_popstk(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_popr(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_cmp(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_jmp(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_je(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_jne(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_jg(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_jl(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_jge(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_jle(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_assign(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_plus(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_minus(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_mult(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_div(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_expo(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_mod(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_inc(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_dec(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_and(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_or(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_xor(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_not(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_shfl(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_shfr(bytecode::wcSimpleExecContext&, bytecode::wcInstruction);
		inline void exec_s_halt(bytecode::wcSimpleExecContext&, bytecode::wcInstructionPlusOperand);
	}

	namespace compile
	{
		class wcSimpleCompiler : public wcBaseCompiler
		{
		public:
			wcSimpleCompiler();

			virtual api::wcScript compile(std::vector<std::string> input);
		};
	}
}
#endif