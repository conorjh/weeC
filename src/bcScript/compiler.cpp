#include "compiler.h"

using namespace std;
using namespace bc;
using namespace bc::comp;
using namespace bc::vm;

bc::comp::bcCompiler::bcCompiler()
{

}

bc::comp::bcCompiler::bcCompiler(vector<string>* p_sc)
{
	startup();
	compile(p_sc);
}

void bc::comp::bcCompiler::startup()
{
	l.startup();

	p.lexer = &l;
	p.startup();
}

void bc::comp::bcCompiler::shutdown()
{
	p.shutdown();
	l.shutdown();
}

bcExecContext* bc::comp::bcCompiler::compile(vector<string>* p_sc)
{
	//parse the code into an AST
	l.source = p_sc;
	p.parse();
	if (p.getError())
	{
		//handle parse errors
	}

	//build AST into bytecode
	g.ast = &p.ast;
	bcExecContext* ec = g.gen();
	if (g.getError())
	{

	}
	return ec;
}