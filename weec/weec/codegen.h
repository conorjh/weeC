#ifndef WC_CODEGEN_H
#define WC_CODEGEN_H
#include <map>
#include <string>
#include "parse.h"
#include "bytecode.h"

namespace wc
{
	namespace codegen
	{
		int genStatement(wc::parse::wcParseIndex&);
		int genExpression(wc::parse::wcParseIndex&);
		
		struct wcGenParams
		{
			wcGenParams(wc::parse::wcParseIndex&, wc::parse::wcAST&, bytecode::wcExecContext&);
			wc::parse::wcParseIndex& pindex;
			wc::parse::wcAST& ast;
			bytecode::wcExecContext& output;
		};

		class wcBaseBytecodeGen
		{
		public:
			error::wcError getError();
			void setError(error::wcError);

			virtual bytecode::wcExecContext gen(parse::wcAST&);

		private:
			error::wcError error;
		};

		
		
	}
}

#endif
