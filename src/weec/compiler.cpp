#include "compiler.h"

using namespace std;
using namespace wc;
using namespace wc::comp;
using namespace wc::vm;

namespace wc
{
	namespace comp
	{
		wcErrorCode convertParserToCompilerError(wcErrorCode);
		wcErrorCode convertGenToCompilerError(wcErrorCode);
	}
}

wc::comp::wcCompiler::wcCompiler()
{
	startup();
	output = nullptr;
}

wc::comp::wcCompiler::wcCompiler(vector<string>* p_sc)
{
	startup();
	output = compile(p_sc);
}

void wc::comp::wcCompiler::startup()
{
	p.lexer = &l;
	p.startup();
	this->g = wcByteCodeGen();
}

void wc::comp::wcCompiler::shutdown()
{
	p.shutdown();
}

wcExecContext* wc::comp::wcCompiler::compile(vector<string>* p_sc)
{
	//parse the code into an AST
	l.data.source = p_sc;
 	p.parse();
	
	//handle parse errors
	if (p.getError().code > 0)
		return nullptr;	

	//build AST into bytecode
	g.ast = &p.ast;
	output = g.gen();

	if (g.getError().code > 0)
		return nullptr;
	
	return output;
}

wcError wc::comp::wcCompiler::getError()
{
	//return wcError();	
	if (p.getError().code > 0)
		return p.getError();
	if (g.getError().code > 0)
		return g.getError();
}

std::string wc::comp::getCompilerErrorString(int p_ec)
{
	return parse::getErrorCodeAsString(wcErrorCode(p_ec));
}

wcErrorCode wc::comp::convertParserToCompilerError(wcErrorCode p_ec)
{
	return p_ec;
}

wcErrorCode wc::comp::convertGenToCompilerError(wcErrorCode p_ec)
{

	return wcErrorCode(ec_null);
}