#ifndef WC_CLASSIC_H
#define WC_CLASSIC_H
#include "compiler.h"
#include "codegen.h"
#include "lex.h"
#include "vm.h"

namespace wc
{
	namespace codegen
	{
		class wcClassicBytecodeGen : public wcBaseBytecodeGen
		{
		public:
			virtual bytecode::wcExecContext gen(parse::wcAST&);
		};
	}

	namespace compile
	{
		class wcClassicCompiler : public wcBaseCompiler
		{
		public:
			wcClassicCompiler();

			virtual api::wcScript compile(std::vector<std::string> input, parse::wcAST* p_outputAST = nullptr);
		};
	}


	namespace vm
	{
		class wcClassicVM : public wcBaseVM
		{
		public:
			wcClassicVM();
			virtual int execInstruction(bytecode::wcExecContext& context, bytecode::wcInstruction instr);
		};
	}
	
}


#endif