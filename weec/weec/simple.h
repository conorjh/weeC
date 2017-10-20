#ifndef WC_SIMPLE_H
#define WC_SIMPLE_H
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
			soc_nop, soc_push,
			
			//conditional jumps
			soc_cmp, soc_jmp, soc_je, soc_jne, soc_jg, soc_jl, soc_jge, soc_jle,
			
			//stack math/logical operations
			soc_assign, soc_plus, soc_minus, soc_mult, soc_div, soc_expo, soc_mod, soc_inc, soc_dec,
			soc_and, soc_or, soc_xor, soc_not, soc_shfl, soc_shfr,
			
			//function calls
			soc_call, soc_ret, soc_callvm,
		};
	}

	namespace codegen
	{
		int genSimpStatement(wc::parse::wcParseIndex&);
<<<<<<< HEAD
		std::vector<bytecode::wcInstruction> genSimpExpression(wc::parse::wcParseIndex&, std::vector<lex::wcToken>* rpnOutput);
=======
		int genSimpExpression(wc::parse::wcParseIndex&);
>>>>>>> origin/master
		int genSimpIf(wc::parse::wcParseIndex&);
		int genSimpDecVar(wc::parse::wcParseIndex&);

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
		};
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