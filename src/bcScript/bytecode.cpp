#include "bytecode.h"
#include "util.h"

using namespace bc::vm;
using namespace bc::parse;
using namespace bc::util;
using bc::parse::bcAST;
using bc::parse::bcParseNodeType;
using bc::vm::bcValType;

bcStack::bcStack()
{

}

void bcStack::clear()
{
	cont.clear();
}

void bcStack::clear(bcExecContext* ec)
{
	cont.clear();
	if(ec)
		ec->reg[tos]=0;
}

int bcStack::pop()
{
	int ret = cont[cont.size()-1];
	cont.pop_back();
	return ret;
}

int bcStack::pop(bcExecContext* ec)
{
	int ret = cont[cont.size()-1];
	cont.pop_back();
	if(ec)
		ec->reg[tos]--;
	return ret;
}

int* bcStack::at(int i)
{
	return &cont[i];
}

int bcStack::top()
{
	return cont[cont.size()-1];	
}

int bcStack::size()
{
	return cont.size();
}

void bcStack::push(int a)
{
	cont.push_back(a);
}

void bcStack::push(bcExecContext* ec,int a)
{
	cont.push_back(a);
	if(ec)
		ec->reg[tos]++;
}

bcByteCode::bcByteCode()
{
	arg1=0;
	arg2=0;
	op=oc_nop;
}

bcExecContext::bcExecContext()
{
	this->regFlags=0;
	for(int t=0;t<32;t++)
	{
		reg[t]=0;
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
		if(sym->dataType == "int")
			return vt_int;
		else if(sym->dataType == "float")
			return vt_float;
		else if(sym->dataType == "string")
			return vt_string;
		else if(sym->dataType == "bool")
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
	istream->push_back(bc);
	return istream->size()-1;
}

unsigned int bcByteCodeGen::addByteCode(bcOpCode oc)
{
	bcByteCode bc;
	bc.op=oc;
	return addByteCode(bc);
}

unsigned int bcByteCodeGen::addByteCode(bcOpCode oc,int a1)
{
	bcByteCode bc;
	bc.op=oc;	bc.arg1=a1;	bc.arg2=0;
	return addByteCode(bc);
}

unsigned int bcByteCodeGen::addByteCode(bcOpCode oc,int a1,int a2)
{
	bcByteCode bc;
	bc.op=oc;	bc.arg1=a1;	bc.arg2=a2;
	return addByteCode(bc);
}

bcByteCode* bcByteCodeGen::getByteCode(unsigned int ind)
{
	return &istream->at(ind);
}

bcByteCode* bcByteCodeGen::getByteCode(unsigned int ind, bool isFunc)
{
	return &istream->at(ind);
}

int bcByteCodeGen::getError()
{
	return errorCode;
}

bcExecContext* bcByteCodeGen::gen()
{
	bcExecContext* ec=new bcExecContext();
	bcByteCode bc;
	istream=new std::vector<bcByteCode>;
	pi=ast->tree->begin();

	//push command line args

	//push global stackframe variables
	for(int t=0;t<ast->stackFrames.at(0).size();++t)
	{
		bc.op = oc_push;
		bc.arg1 = 0;
		addByteCode(bc);
	}

	//parse body
	while(pi!=ast->tree->end())
		genStatement(this);

	ec->istream=*this->istream;
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
			bg->addByteCode(oc_lrfs,eax);
			bg->addByteCode(oc_pop);
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
			fi=bg->ast->funcTab->at( bg->pi.node->data.tokens.at(0).data );
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
	//pop the result of the expression into eax
	bg->addByteCode(oc_lrfs,eax);
	bg->addByteCode(oc_pop);

}

void bc::vm::genIf(bcByteCodeGen* bg)
{
	//collect func dec info from current node
	unsigned int trueindex,truejump,elseindex,elsejump,iend;
	unsigned int ibegin = bg->istream->size()-1;
	int olddepth=bg->ast->tree->depth(bg->pi);	++bg->pi;
	while(bg->ast->tree->depth(bg->pi) > olddepth)
		switch(bg->pi->type)
		{
		case pn_exp:
			genExp(bg);
			break;
			
		case pn_if_trueblock:			
			genBlock(bg); 
			truejump = bg->addByteCode(oc_jmp);
			break;

		case pn_if_elseblock:
			genBlock(bg);
			//point the jmp at the end of the true block past the else block
			bg->getByteCode(truejump,bg->inDecFunc)->arg1 = elsejump = bg->istream->size();	break; 
		default:
			bg->pi++;
			break;
		}

	//if this is the last statement, we need an address to land on after else
	iend=bg->addByteCode(oc_nop);
	//pop stack to expression result register	
	//bg->addByteCode(oc_lrfs,eax);
	//now that we know the instruction, edit all prior je/jne/jg/jge to the right block
	adjustJumps(bg,ibegin,iend,truejump+1);
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

//rpn is provided as a vector of parsenodes
void bc::vm::genRpnToByteCode(bcByteCodeGen* bg,std::vector<bcParseNode*>* rpn)
{
	bcParseNode* lastnode;
//	bcByteCode bc;
	while(rpn->size())
	{
		switch(rpn->at(0)->type)
		{
		case pn_strlit:	
			//bg->addByteCode(oc_push,vt_string,bcstoi(rpn->at(0)->tokens.at(0).data));
			break;
		case pn_fltlit:	
			bg->addByteCode(oc_push,bcstof(rpn->at(0)->tokens.at(0).data));
			break;
		case pn_intlit:
			bg->addByteCode(oc_push,bcstoi(rpn->at(0)->tokens.at(0).data));
			break;
		case pn_true:	
			bg->addByteCode(oc_push,1);
			break;
		case pn_false:
			bg->addByteCode(oc_push,0);
			break;
		
			//variables
		case pn_ident:	case pn_varident:	
			//copy stored value from stack, to the top of stack 
			bg->addByteCode(oc_pushfs,bcstoi(rpn->at(0)->tokens.at(1).data));	//push value from stack, using stackindex from tokens[1]
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
			bg->addByteCode(oc_cmp);	
			bg->addByteCode(oc_jne);	break;
		case pn_notequal:
			bg->addByteCode(oc_cmp);	
			bg->addByteCode(oc_je);	break;
		case pn_greater:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jle);	break;
		case pn_less:	
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jge);	break;
		case pn_lessequal:	
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jg);	break;
		case pn_greaterequal:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jl);	break;
		case pn_assign:
			bg->addByteCode(oc_assign,bcstoi(rpn->at(0)->tokens.at(1).data));	break;
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
		lastnode=rpn->at(0);
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
		case pn_assign:
			bg->addByteCode(oc_mov);
			break;
		case pn_equal:	
			bg->addByteCode(oc_jne);
		case pn_notequal:
			bg->addByteCode(oc_je);		
		case pn_greater:	
			bg->addByteCode(oc_jle);
		case pn_less:
			bg->addByteCode(oc_jge);
		case pn_lessequal:
			bg->addByteCode(oc_jg);
		case pn_greaterequal:
			bg->addByteCode(oc_jl);
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
			bg->addByteCode(oc_not);
			break;
		case pn_plus:
			bg->addByteCode(oc_plus);
			break;	
	}
}

void bc::vm::adjustJumps(bcByteCodeGen* bg, int beg, int end, int add)
{
	for(int t=beg;t<=end;t++)
		switch(bg->istream->at(t).op)
		{
		case oc_je:	case oc_jne:	
		case oc_jl:	case oc_jle:
		case oc_jg:	case oc_jge:
			bg->istream->at(t).arg1 = add;
			break;
		default:
			break;
		}
}