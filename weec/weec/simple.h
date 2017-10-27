#ifndef WC_SIMPLE_H
#define WC_SIMPLE_H
#include <unordered_map>
#include "vm.h"
#include "bytecode.h"
#include "compiler.h"
#include "api_types.h"

namespace wc
{
	namespace bytecode
	{
		enum wcSimpleOpcode
		{
			//stack
			soc_nop, soc_push, soc_pop,
			//conditional jumps
			soc_cmp, soc_jmp, soc_je, soc_jne, soc_jg, soc_jl, soc_jge, soc_jle,
			//stack math/logical operations
			soc_assign, soc_plus, soc_minus, soc_mult, soc_div, soc_expo, soc_mod, soc_inc, soc_dec,
			soc_and, soc_or, soc_xor, soc_not, soc_shfl, soc_shfr,
			//function calls
			soc_call, soc_ret, soc_callvm,
		};

		const std::unordered_multimap<wcSimpleOpcode, int> wcSimpleOpcodeOperandCounts=
		{
			{soc_nop,0},{ soc_push,1 },{ soc_pop,0 },{soc_cmp,0}, {soc_jmp,1}, {soc_je,1}, {soc_jne,1}, {soc_jg,1}, {soc_jl,1}, {soc_jge,1}, {soc_jle,1},
			{soc_assign,0}, {soc_plus,0}, {soc_minus,0}, {soc_mult,0}, {soc_div,0}, {soc_expo,0}, {soc_mod,0}, {soc_inc,0}, {soc_dec,0},
			{soc_and,0}, {soc_or,0}, {soc_xor,0}, {soc_not,0}, {soc_shfl,0}, {soc_shfr,0},{soc_call,0}, {soc_ret,0}, {soc_callvm,0}
		};
	}

	namespace codegen
	{
		int genSimpStatement(wc::parse::wcParseIndex&, bytecode::wcExecContext& output);
		std::vector<bytecode::wcInstruction> genSimpExpression(wc::parse::wcParseIndex&, bytecode::wcExecContext& output, std::vector<parse::wcParseNode>* rpnOutput);
		int genSimpIf(wc::parse::wcParseIndex&, bytecode::wcExecContext& output);
		int genSimpDecVar(wc::parse::wcParseIndex&, bytecode::wcExecContext& output);

		class wcSimpleBytecodeGen : public wcBaseBytecodeGen
		{
		public:
			virtual bytecode::wcExecContext gen(parse::wcAST&);
		};
	}

	namespace vm
	{
		class wcSimpleVM : wcBaseVM
		{
			wcSimpleVM();
			virtual int execInstruction(bytecode::wcExecContext& context, bytecode::wcInstruction instr);

		private:

		};

		//simple bytecode 
		inline void exec_s_push(bytecode::wcExecContext&, bytecode::wcInstructionPlusOperand);
		inline void exec_s_pop(bytecode::wcExecContext&, bytecode::wcInstruction);
		inline void exec_s_cmp(bytecode::wcExecContext&, bytecode::wcInstruction);

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