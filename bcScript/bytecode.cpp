#include "bytecode.h"

using namespace bc::vm;
using namespace bc::parse;
using bc::parse::bcAST;
using bc::parse::bcParseNodeType;

std::vector<bcByteCode>* bcByteCodeGen::gen()
{
	
	std::vector<bcByteCode>* istream;
	pi=ast->tree->begin();

	//load global stackframe
	for(int t=0;t<ast->stackframes.at(0).size();++t)
	{
		//ast->stackframes.at(0).at(t)
			
	}
	
	while(pi!=ast->tree->end())
	{
		switch(pi->type)
		{
		case pn_null:
		case pn_head:
			pi++;
			break;
		case pn_exp:
			pi++;
			break;
		case pn_funcdec:
			genFuncDec(this);
			break;
		default:
			++pi;
		}
	}
	return istream;
}

void bc::vm::genFuncDec(bcByteCodeGen* bg)
{
	int olddepth=bg->ast->tree->depth(bg->pi);
	
	while(bg->ast->tree->depth(bg->pi))
		switch(bg->pi->type)
		{
		case pn_ident:
			++bg->pi;
			break;

		default:
			++bg->pi;
			break;
		}
}