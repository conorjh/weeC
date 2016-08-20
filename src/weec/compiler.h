#ifndef wc_COMPILER_H
#define wc_COMPILER_H
#include "parser.h"
#include "bytecode.h"
#include "vm.h"

namespace wc
{
	namespace comp
	{		
		class wcCompiler
		{
		public:
			wcCompiler();
			wcCompiler(std::vector<std::string>*);

			void startup(), shutdown();
			vm::wcExecContext* compile(std::vector<std::string>*);	//opcodes from source code

			lex::wcLexer l;
			parse::wcParser p;
			vm::wcByteCodeGen g;
			vm::wcExecContext* output;
		};
	}
}

#endif