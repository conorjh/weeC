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
	
	//parse
	
	//code gen

	return output;
}

