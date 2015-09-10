#ifndef BCC_COMPILER_H
#define BCC_COMPILER_H

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
			void startup(),shutdown();
			vm::bcExecContext* compile(std::vector<std::string>*);	//opcodes from source code

			lex::bcLexer l;
			parse::bcParser p;
			vm::bcByteCodeGen g;
		};
	}
}

#endif