#ifndef BC_COMPILER_H
#define BC_COMPILER_H
#include "parser.h"
#include "bytecode.h"
#include "vm.h"

namespace bc
{
	namespace comp
	{		
		class bcCompiler
		{
		public:
			bcCompiler();
			bcCompiler(std::vector<std::string>*);

			void startup(), shutdown();
			vm::bcExecContext* compile(std::vector<std::string>*);	//opcodes from source code

			lex::bcLexer l;
			parse::bcParser p;
			vm::bcByteCodeGen g;
			vm::bcExecContext* output;
		};
	}
}

#endif