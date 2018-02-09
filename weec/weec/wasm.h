#ifndef WC_WASM_H
#define WC_WASM_H
#include <vector>
#include "lex.h"
#include "parse.h"
#include "codegen.h"
#include "compiler.h"

namespace wc
{
	namespace wasm
	{
		namespace lex
		{
			class wasmLexer : public wc::lex::wcLexer
			{
			public:
				wasmLexer();
				~wasmLexer();

				virtual std::vector<wc::lex::wcToken> lex(std::vector<std::string>);
			};
		}

		namespace parse
		{
			class wasmParser : public wc::parse::wcParser
			{
			public:
				wasmParser();
				~wasmParser();

				virtual wc::parse::wcAST parse(std::vector<wc::lex::wcToken>);
			};
		}

		namespace codegen
		{

		}
	}
}

#endif
