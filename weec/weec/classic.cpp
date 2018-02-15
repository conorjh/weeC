#include "classic.h"
#include "api_types.h"

using namespace std;
using namespace wc;
using namespace wc::codegen;
using namespace wc::api;
using namespace wc::bytecode;
using namespace wc::parse;
using namespace wc::lex;

wc::vm::wcClassicVM::wcClassicVM()
{

}

int wc::vm::wcClassicVM::execInstruction(wcExecContext& p_context, wcInstruction p_instr)
{

	return 1;
}

wcExecContext wc::codegen::wcClassicBytecodeGen::gen(wcAST& p_ast)
{
	wcExecContext output;
	output.targetPlatform = ct_bytecode;

	return output;
}


wc::compile::wcClassicCompiler::wcClassicCompiler()
{

}

wcScript wc::compile::wcClassicCompiler::compile(vector<string> p_source, wcAST* p_outputAST)
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

	if (p_outputAST != nullptr)
		*p_outputAST = tree;

	return output;
}

