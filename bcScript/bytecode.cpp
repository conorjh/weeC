#include "bytecode.h"
#include "util.h"

using namespace bc::vm;
using namespace bc::parse;
using namespace bc::util;
using bc::parse::bcAST;
using bc::parse::bcParseNodeType;
using bc::vm::bcValType;

bcExecContext::bcExecContext()
{
	this->offset=0;
	this->pc=0;
	this->regFlags=0;
	for(int t=0;t<32;t++)
	{
		reg[t].val=0;
		regFlags[t]=0;
	}
	halt=false;
}
int bc::vm::getValTypeSize(bcValType t)
{
	switch(t)
	{
	default:
	case vt_bool:
	case vt_string:
	case vt_int:
		return 1;
	case vt_float:
		return 2;
	}
}
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

unsigned int bcByteCodeGen::addByteCode(bcByteCode bc)
{
	if(!inDecFunc)
	{
		istream->push_back(bc);
		return istream->size();
	}
	else
	{
		fstream->push_back(bc);
		return fstream->size();
	}
}

unsigned int bcByteCodeGen::addByteCode(bcOpCode oc)
{
	bcByteCode bc;
	bc.op=oc;
	return addByteCode(bc);
}

unsigned int bcByteCodeGen::addByteCode(bcOpCode oc,bcVal a1)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1=a1;
	return addByteCode(bc);
}

unsigned int bcByteCodeGen::addByteCode(bcOpCode oc,bcVal a1,bcVal a2)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1=a1;
	bc.arg2=a2;
	return addByteCode(bc);
}

unsigned int bcByteCodeGen::addByteCode(bcOpCode oc,bcValType vt1)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1.type=vt1;
	return addByteCode(bc);
}

unsigned int bcByteCodeGen::addByteCode(bcOpCode oc,bcValType vt1,bcValType vt2)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1.type=vt1;
	bc.arg2.type=vt2;
	return addByteCode(bc);
}
unsigned int bcByteCodeGen::addByteCode(bcOpCode oc,bcValType vt1,unsigned int v1)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1.type=vt1;
	bc.arg1.val=v1;
	return addByteCode(bc);
}

unsigned int bcByteCodeGen::addByteCode(bcOpCode oc,bcValType vt1,unsigned int v1,bcValType vt2,unsigned int v2)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1.type=vt1;
	bc.arg1.val=v1;
	bc.arg2.type=vt2;
	bc.arg2.val=v2;
	return addByteCode(bc);
}

bcByteCode* bcByteCodeGen::getByteCode(unsigned int ind)
{
	if(inDecFunc)
		return &fstream->at(ind);
	return &istream->at(ind);
}

bcByteCode* bcByteCodeGen::getByteCode(unsigned int ind, bool isFunc)
{
	if(isFunc)
		return &fstream->at(ind);
	return &istream->at(ind);
}

std::vector<bcByteCode>* bcByteCodeGen::getCurrentStream()
{
	if(inDecFunc)
		return fstream;
	return istream;
}
bcExecContext* bcByteCodeGen::gen()
{
	bcExecContext* ec=new bcExecContext();
	bcByteCode bc;
	istream=new std::vector<bcByteCode>;
	fstream=new std::vector<bcByteCode>;
	pi=ast->tree->begin();

	//push command line args

	//push global stackframe variables
	for(int t=0;t<ast->stackframes.at(0).size();++t)
	{
		bc.op = oc_push;
		bc.arg1.type = getValType(&ast->symtab->at(ast->stackframes.at(0).at(t)));	
		bc.arg1.val = 0;
		addByteCode(bc);
	}

	//parse body
	while(pi!=ast->tree->end())
		genStatement(this);

	ec->istream=*this->istream;
	ec->fstream=*this->fstream;
	return ec;
}

void bc::vm::genStatement(bcByteCodeGen* bg)
{
	int olddepth=bg->ast->tree->depth(bg->pi);
	bg->pi++;

	while(bg->pi!=bg->ast->tree->end() && bg->ast->tree->depth(bg->pi) >= olddepth)
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
			//bg->pi++;
			genExp(bg);
			break;
		case pn_namespacedec:
			genDecNamespace(bg);
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
		case pn_if:
			genIf(bg);
			break;
		default:
			++bg->pi;
		}
}

void bc::vm::genDecParamList(bcByteCodeGen* bg)
{
	int olddepth=bg->ast->tree->depth(bg->pi);	++bg->pi;
	while(bg->ast->tree->depth(bg->pi) > olddepth)
		bg->pi++;
}

void bc::vm::genDecFunc(bcByteCodeGen* bg)
{
	int fOffset;
	tree<bcParseNode>::iterator oldpi;
	bcFuncInfo fi;
	std::string paramString;
	bg->inDecFunc=true;

	//collect func dec info from current node
	int olddepth=bg->ast->tree->depth(bg->pi);	++bg->pi;
	while(bg->ast->tree->depth(bg->pi) > olddepth)
		switch(bg->pi->type)
		{
		case pn_funcident:
			fi=bg->ast->functab->at( bg->pi.node->data.tokens.at(0).data );
			++bg->pi;
			break;

		case pn_block:
			oldpi=bg->pi;
			bg->pi = fi.body[paramString];
			fOffset = bg->istream->size();
			genBlock(bg);
			break;

		case pn_decparamlist:
			paramString = bg->pi->tokens.at(0).data;
			genDecParamList(bg);
			//++bg->pi;
			break;

		case pn_type:
		default:
			++bg->pi;
			break;
		}
	bg->inDecFunc=false;
}

void bc::vm::genDecNamespace(bcByteCodeGen* bg)
{
	//collect func dec info from current node
	int olddepth=bg->ast->tree->depth(bg->pi);	++bg->pi;
	while(bg->ast->tree->depth(bg->pi) > olddepth)
		
	genBlock(bg);
}

void bc::vm::genDecVar(bcByteCodeGen* bg)
{
	int olddepth=bg->ast->tree->depth(bg->pi);	++bg->pi;
	while(bg->ast->tree->depth(bg->pi) > olddepth)while(bg->ast->tree->depth(bg->pi) > olddepth)
		switch(bg->pi->type)
		{
		case pn_exp:
			genExp(bg);
			break;
		
		case pn_type:	
		case pn_ident:
		default:
			++bg->pi;
			break;
		}

}

void bc::vm::genIf(bcByteCodeGen* bg)
{
	//collect func dec info from current node
	unsigned int trueindex,truejump;
	int olddepth=bg->ast->tree->depth(bg->pi);	++bg->pi;
	while(bg->ast->tree->depth(bg->pi) > olddepth)
		switch(bg->pi->type)
		{
		case pn_exp:
			genExp(bg);
			break;
			
		case pn_if_trueblock:
			//trueindex = bg->addByteCode(oc_jne,vt_astack,0,vt_instr,0);
			genBlock(bg);
			//bg->getByteCode(trueindex,bg->inDecFunc)->arg2.val=truejump=bg->getCurrentStream()->size()+1;
			break;

		case pn_if_elseblock:
			genBlock(bg);
			break;

		default:
			bg->pi++;
			break;
		}
}

void bc::vm::genExp(bcByteCodeGen* bg)
{
	bool foundParen=false;
	std::vector<bcParseNode*> out,stk;
	int olddepth=bg->ast->tree->depth(bg->pi);	++bg->pi;
	while(bg->ast->tree->depth(bg->pi) > olddepth)
	{
		switch(bg->pi->type)
		{
		case pn_exp:
			genExp(bg);
			break;

			//literals
		case pn_strlit:	case pn_fltlit:	case pn_intlit:
		case pn_true:	case pn_false:
			out.push_back(&bg->pi.node->data);
			break;
		
			//variables
		case pn_ident:	case pn_varident:	case pn_funcident:
			out.push_back(&bg->pi.node->data);
			break;

			//operators
		case pn_logor:	case pn_logand:	case pn_equal:	case pn_notequal:	case pn_assign:
		case pn_greater:case pn_less:	case pn_lessequal:	case pn_greaterequal:
		case pn_mult:	case pn_div:	case pn_plus:	case pn_minus:	case pn_lognot:	case pn_expo:	case pn_mod:
			while( 
				stk.size()>0 && (getPrecedence(bg->pi.node->data.tokens[0]) == getPrecedence(stk[stk.size()-1]->tokens.at(0)) && !getAssociativity(bg->pi.node->data.tokens[0])  || 
				getAssociativity(bg->pi.node->data.tokens[0]) && getPrecedence(bg->pi.node->data.tokens[0])< getPrecedence(stk[stk.size()-1]->tokens.at(0))) 
				)
			{
				out.push_back(stk[stk.size()-1]);
				stk.erase(stk.end()-1,stk.end());
			}
			stk.push_back(&bg->pi.node->data);
			break;

		case pn_oparen:
				stk.push_back(&bg->pi.node->data);
				break;

		case pn_cparen:
			foundParen=false;
			//pop stack to ouput til we find a (, error if not
			while(!foundParen && stk.size())
			{
				if(stk[stk.size()-1]->type==pn_oparen)
				{
					foundParen=true;
				}
				else
				{
					out.push_back(stk[stk.size()-1]);
					stk.erase(stk.end()-1,stk.end());
				}
			}
			
			//mismatched params - error
			if(!foundParen)
				return;
			stk.erase(stk.end()-1,stk.end());
			
			//func ident
			if(stk.size() && stk[stk.size()-1]->type==pn_funcident)
			{
				out.push_back(stk[stk.size()-1]);
				stk.erase(stk.end()-1,stk.end());
			}
			break;

		case pn_type:	
		default:
			break;
		}
		++bg->pi;
	}

	//pop off remaining operators
	while(stk.size()>0)
	{
		out.push_back(stk[stk.size()-1]);
		stk.erase(stk.end()-1,stk.end());
	}

	genRpnToByteCode(bg,&out);
}

void bc::vm::genBlock(bcByteCodeGen* bg)
{
	int olddepth=bg->ast->tree->depth(bg->pi); bg->pi++;
	while(bg->ast->tree->depth(bg->pi) > olddepth)
			genStatement(bg);

}

void bc::vm::genRpnToByteCode(bcByteCodeGen* bg,std::vector<bcParseNode*>* rpn)
{
//	bcByteCode bc;
	while(rpn->size())
	{
		switch(rpn->at(0)->type)
		{
		case pn_strlit:	
			//bg->addByteCode(oc_push,vt_string,bcstoi(rpn->at(0)->tokens.at(0).data));
			break;
		case pn_fltlit:	
			bg->addByteCode(oc_push,vt_float,bcstof(rpn->at(0)->tokens.at(0).data));
			break;
		case pn_intlit:
			bg->addByteCode(oc_push,vt_int,bcstoi(rpn->at(0)->tokens.at(0).data));
			break;
		case pn_true:	
			bg->addByteCode(oc_push,vt_bool,1);
			break;
		case pn_false:
			bg->addByteCode(oc_push,vt_bool,0);
			break;
		
			//variables
		case pn_ident:	case pn_varident:	
			break;
		
		case pn_funcident:
			break;

			//operators
		case pn_lognot:
			bg->addByteCode(oc_not);	break;
			break;
		case pn_logor:	
			bg->addByteCode(oc_or);	break;
		case pn_logand:
			bg->addByteCode(oc_and);	break;
		case pn_equal:
			bg->addByteCode(oc_je);	break;
		case pn_notequal:
			bg->addByteCode(oc_jne);	break;	
		case pn_assign:
			bg->addByteCode(oc_mov);	break;
		case pn_greater:
			bg->addByteCode(oc_jg);	break;
		case pn_less:
			bg->addByteCode(oc_jl);	break;
		case pn_lessequal:
			bg->addByteCode(oc_jle);	break;
		case pn_greaterequal:
			bg->addByteCode(oc_jge);	break;
		case pn_mult:
			bg->addByteCode(oc_mult);	break;
		case pn_div:
			bg->addByteCode(oc_div);	break;
		case pn_plus:
			bg->addByteCode(oc_plus);	break;
		case pn_minus:
			bg->addByteCode(oc_minus);	break;
		case pn_mod:
			bg->addByteCode(oc_mod);	break;
		case pn_expo:
			bg->addByteCode(oc_expo);	break;
				
		//case pn_ident:
		case pn_type:	
		default:
			break;
		}
		rpn->erase(rpn->begin());
	}

}

void bc::vm::genNodeToByteCode(bcByteCodeGen* bg,bcParseNode* pn)
{
	switch(pn->type)
	{
		case pn_strlit:	
			bg->addByteCode(oc_push,vt_string,0);	break;
		case pn_fltlit:	
			bg->addByteCode(oc_push,vt_float,bcstoi(pn->tokens.at(0).data));	break;
		case pn_intlit:
			bg->addByteCode(oc_push,vt_int,bcstoi(pn->tokens.at(0).data));		break;
		case pn_true:	
			bg->addByteCode(oc_push,vt_bool,1);		break;
		case pn_false:
			bg->addByteCode(oc_push,vt_bool,0);		break;
		
			//variables
		case pn_ident:	
		case pn_varident:
		case pn_funcident:
		
			break;

			//operators
		case pn_logor:
			bg->addByteCode(oc_or);
			break;			
		case pn_logand:
			bg->addByteCode(oc_and);
			break;			
		case pn_equal:
			bg->addByteCode(oc_je);
			break;			
		case pn_notequal:
			bg->addByteCode(oc_jne);
			break;			
		case pn_assign:
		case pn_greater:
			bg->addByteCode(oc_jg);
			break;		
		case pn_less:
			bg->addByteCode(oc_jl);
			break;			
		case pn_lessequal:
			bg->addByteCode(oc_jle);
			break;			
		case pn_greaterequal:
			bg->addByteCode(oc_jge);
			break;		
		case pn_mult:
			bg->addByteCode(oc_mult);
			break;		
		case pn_div:
			bg->addByteCode(oc_div);
			break;		
		case pn_minus:
			bg->addByteCode(oc_minus);
			break;		
		case pn_lognot:
			
			break;
		case pn_plus:
			bg->addByteCode(oc_plus);
			break;	
	}
}