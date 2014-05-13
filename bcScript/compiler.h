#include "parser.h"
#include "bytecode.h"

namespace bc
{
	namespace comp
	{
		struct bcProgram
		{
			std::vector<vm::bcOpCode> istream;
			std::vector<vm::bcOpCode> fstream;
		};

		class bcCompiler
		{
			void startup(),shutdown();
			bcProgram compile(std::vector<std::string>*);	//opcodes from source code
		};
	}
}