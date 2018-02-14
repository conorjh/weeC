#include "wasm.h"
#include "lex.h"
#include <vector>

using namespace std;
using namespace wc;
using namespace wc::lex;
using namespace wc::parse;

namespace wc
{
	namespace wasm
	{
		namespace lex
		{
			wcLexer asmLexer;
		}
	}
}

wc::wasm::lex::wasmLexer::wasmLexer()
{

}

wc::wasm::lex::wasmLexer::~wasmLexer()
{

}

vector<wcToken> wc::wasm::lex::wasmLexer::lex(vector<string> p_input)
{
	vector<wcToken> out;
	lexIndex.source = &p_input;

	//loop through characters making tokens until end of stream
	while (lexIndex.isValid())
		switch (asmLexer.deriveTokenType(lexIndex.getChar()))
		{
			//string literal
		case tt_dquote:
			if (!lex_stringLiteral(out, lexIndex, error))
				return out;
			break;

			//integer literals
		case tt_intlit:
			if (!lex_intLiteral(out, lexIndex, error))
				return out;
			break;

			//all 2 part tokens (operators etc)
		case tt_greater: case tt_less: case tt_lognot:
		case tt_assign:	case tt_plus: case tt_minus:
		case tt_amper: case tt_pipe: case tt_colon:
			if (!lex_2step(out, lexIndex, error))
				return out;
			break;

			//whitespace
			CASE_TT_WS
				if (!lex_ws(out, lexIndex, error))
					return out;
			break;

			//comments
		case tt_div:
			if (!lex_comment(out, lexIndex, error))
				return out;
			break;

			//possible delimiter or identifier etc
		default:
			if (!lex_default(out, lexIndex, error))
				return out;
			break;
		}

	return out;
}

wcAST wc::wasm::parse::wasmParser::parse(vector<wcToken> p_tokens)
{
	init();

	wcAST output;
	index.setTokens(&p_tokens);
	index.setNode(output.parseTree.begin());
	data.currentScope = output.symTab.getSymbol("$global");

	wcParseParams params(index, output, error, data);

	/*while (index.isLexIndexValid() && !getError().code)
		if (!parseStatement(params))
			return output;*/

	return output;
}
