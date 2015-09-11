#include "compiler.h"

using namespace std;
using namespace bc;
using namespace bc::comp;
using namespace bc::vm;

bc::vm::bcExecContext* bc::comp::bcCompiler::compile(vector<string>* p_sc)
{
	//lexer
	l.startup();
	l.source = p_sc;

	//parser
	p.lexer = &l;
	p.startup();
	p.parse();

	if (p.getError())
	{
		//handle parse errors
	}

	//build AST into bytecode
	g.ast = &p.ast;
	bcExecContext* ec = g.gen();

	return ec;
}