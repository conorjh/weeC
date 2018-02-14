#include "compiler.h"

using namespace std;
using namespace wc;
using namespace wc::lex;
using namespace wc::parse;
using namespace wc::compile;
using namespace wc::bytecode;
using namespace wc::codegen;
using namespace wc::api;
using namespace wc::error;

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
