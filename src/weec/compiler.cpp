#include "compiler.h"

using namespace std;
using namespace wc;
using namespace wc::comp;
using namespace wc::vm;

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
	if (p.getError())
	{
		//handle parse errors
	}

	//build AST into bytecode
	g.ast = &p.ast;
	output = g.gen();
	if (g.getError())
	{
		return nullptr;
	}

	return output;
}