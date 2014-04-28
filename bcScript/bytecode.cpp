#include "bytecode.h"

using namespace bc::vm;
using namespace bc::parse;
using bc::parse::bcAST;
using bc::parse::bcParseNodeType;
using bc::vm::bcValType;

bcValType bc::vm::getValType(bcSymbol* sym)
{
	switch(sym->type)
	{
	case st_type:
	case st_var:
		if(sym->datatype == "int")
			return vt_int;
		else if(sym->datatype == "float")
			return vt_float;
		else if(sym->datatype == "string")
			return vt_string;
		else if(sym->datatype == "bool")
			return vt_bool;
		else
			return vt_mem;
		break;
	}
	return vt_null;
}

bcByteCodeGen::bcByteCodeGen()
{
	inDecFunc=false;
}

void bcByteCodeGen::gen()
{
	bcByteCode bc;
	istream=new std::vector<bcByteCode>;
	fistream=new std::vector<bcByteCode>;
	pi=ast->tree->begin();

	//push command line args

	//push global stackframe
	for(int t=0;t<ast->stackframes.at(0).size();++t)
	{
		bc.op = oc_push;
		bc.arg1.type = getValType(&ast->symtab->at(ast->stackframes.at(0).at(t)));	
		bc.arg1.val = 0;
		istream->push_back(bc);
	}

	//
	
	while(pi!=ast->tree->end())
		genStatement(this);
}

void bc::vm::genStatement(bcByteCodeGen* bg)
{
	int olddepth=bg->ast->tree->depth(bg->pi);
	bg->pi++;

	while(bg->ast->tree->depth(bg->pi) >= olddepth)
		switch(bg->pi->type)
		{
		case pn_null:
		case pn_head:
			bg->pi++;
			break;
		case pn_statement:
			genStatement(bg);
			break;
		case pn_exp:
			bg->pi++;
			break;
		case pn_funcdec:
			genDecFunc(bg);
			break;
		case pn_vardec:
			genDecVar(bg);
			break;
		case pn_block:
			genBlock(bg);
			break;
		default:
			++bg->pi;
		}
}

void bc::vm::genDecFunc(bcByteCodeGen* bg)
{
	bcFuncInfo fi;
	bg->inDecFunc=true;

	//collect func dec info from current node
	int olddepth=bg->ast->tree->depth(bg->pi);	++bg->pi;
	while(bg->ast->tree->depth(bg->pi) > olddepth)
		switch(bg->pi->type)
		{
		case pn_ident:
			fi=bg->ast->functab->at( bg->pi.node->data.tokens.at(0).data );
			++bg->pi;
			break;

		case pn_block:
			genBlock(bg);
			break;

		case pn_paramlist:
		case pn_type:
		default:
			++bg->pi;
			break;
		}

	//build body into fstream, take note of offset
	tree<bcParseNode>::iterator oldpi=bg->pi;
	bg->pi = *fi.body[fi.fullident];
	int fOffset = bg->fistream->size();
	genBlock(bg);
}

void bc::vm::genDecVar(bcByteCodeGen* bg)
{
	int olddepth=bg->ast->tree->depth(bg->pi);	++bg->pi;
	while(bg->ast->tree->depth(bg->pi) > olddepth)
		bg->pi++;
}

void bc::vm::genBlock(bcByteCodeGen* bg)
{
	int olddepth=bg->ast->tree->depth(bg->pi); bg->pi++;
	while(bg->ast->tree->depth(bg->pi) > olddepth)
			genStatement(bg);

}