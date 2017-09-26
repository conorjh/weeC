#include "compiler.h"

using namespace std;
using namespace wc;
using namespace wc::lex;
using namespace wc::parse;
using namespace wc::compile;
using namespace wc::bytecode;
using namespace wc::codegen;
using namespace wc::api;

wc::compile::wcBaseCompiler::wcBaseCompiler()
{
	error = ec_null;
}

void wc::compile::wcBaseCompiler::setError(wcError p_error)
{
	error = p_error;
}

wcError wc::compile::wcBaseCompiler::getError()
{
	return error;
}

wcScript wc::compile::wcBaseCompiler::compile(vector<string> p_source)
{
	
	return wcScript();
}

wc::compile::wcClassicCompiler::wcClassicCompiler()
{

}

wcScript wc::compile::wcClassicCompiler::compile(vector<string> p_source)
{
	wcScript output;

	//lex
	wcLexer lex;
	vector<wcToken> tokens = lex.lex(p_source);
	if (lex.getError().code)
	{
		error = lex.getError();
		return wcScript();
	}

	//parse
	wcParser parser;
	wcAST tree = parser.parse(tokens);
	if (parser.getError().code)
	{
		error = parser.getError();
		return wcScript();
	}

	//code gen
	wcClassicBytecodeGen gen;
	output.con = gen.gen(tree);
	if (gen.getError().code)
	{
		error = gen.getError();
		return wcScript();
	}
	return output;
}

