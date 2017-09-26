#include "simple.h"
#include <vector>
using namespace std;
using namespace wc;
using namespace wc::bytecode;
using namespace wc::parse;
using namespace wc::api;
using namespace wc::lex;
using namespace wc::codegen;

wcExecContext wc::codegen::wcSimpleBytecodeGen::gen(wcAST& p_ast)
{
	wcExecContext output;
	output.target = ct_simple_bytecode;



	return output;
}

wc::vm::wcSimpleVM::wcSimpleVM()
{

}

int wc::vm::wcSimpleVM::execInstruction(wcExecContext& p_context, wcInstruction p_instruction)
{
	switch (p_instruction.opCode)
	{
		case oc_nop:

			break;
	}
	return 1;
}

wc::compile::wcSimpleCompiler::wcSimpleCompiler()
{

}

wcScript wc::compile::wcSimpleCompiler::compile(vector<string> p_source)
{
	wcScript output;

	//lex
	wcLexer lex;
	vector<wcToken> tokens = lex.lex(p_source);
	if (lex.getError().code)
		return wcScript(lex.getError());

	//parse
	wcParser parser;
	wcAST tree = parser.parse(tokens);
	if (parser.getError().code)
		return wcScript(parser.getError());

	//code gen
	wcSimpleBytecodeGen gen;
	output.con = gen.gen(tree);
	if (gen.getError().code)
		return wcScript(parser.getError());

	//return what we've created
	return output;
}

