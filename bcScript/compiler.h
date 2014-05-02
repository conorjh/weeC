#include "parser.h"
#include "bytecode.h"

namespace bc
{
	namespace comp
	{
		struct bcProgram
		{
			std::vector<vm::bcOpCode> istream;
			std::vector<vm::bcOpCode> fistream;
		};

		class bcCompiler
		{
			void startup(),shutdown();
			void load();
			void compile();
		};
	}
}