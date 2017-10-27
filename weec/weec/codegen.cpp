#include "codegen.h"

using namespace wc;
using namespace wc::bytecode;
using namespace wc::parse;

wcExecContext wc::codegen::wcBaseBytecodeGen::gen(wcAST& p_ast)
{
	return wcExecContext();
}

void wc::codegen::wcBaseBytecodeGen::setError(wcError p_error)
{
	error = p_error;
}

wcError wc::codegen::wcBaseBytecodeGen::getError()
{
	return error;
}

wcExecContext wc::codegen::wcClassicBytecodeGen::gen(wcAST& p_ast)
{
	wcExecContext output;
	output.targetPlatform = ct_bytecode;

	return output;
}

