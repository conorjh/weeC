#ifndef WC_CODEGEN_H
#define WC_CODEGEN_H
#include "parse.h"
#include "bytecode.h"

namespace wc
{
	namespace codegen
	{
		int genStatement(wc::parse::wcParseIndex&);
		int genExpression(wc::parse::wcParseIndex&);

		class wcBaseBytecodeGen
		{
		public:
			wcError getError();
			void setError(wcError);

			virtual bytecode::wcExecContext gen(parse::wcAST&);

		private:
			wcError error;
		};

		class wcClassicBytecodeGen : public wcBaseBytecodeGen
		{
		public:
			virtual bytecode::wcExecContext gen(parse::wcAST&);
		};

		
		
	}
}

#endif
