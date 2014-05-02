#include "bytecode.h"
#include "util.h"

using namespace bc::vm;
using namespace bc::parse;
using namespace bc::util;
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

void bcByteCodeGen::addByteCode(bcByteCode bc)
{
	if(inDecFunc)
		fistream->push_back(bc);
	else
		istream->push_back(bc);
}

void bcByteCodeGen::addByteCode(bcOpCode oc)
{
	bcByteCode bc;
	bc.op=oc;
	addByteCode(bc);
}

void bcByteCodeGen::addByteCode(bcOpCode oc,bcVal a1)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1=a1;
	addByteCode(bc);
}

void bcByteCodeGen::addByteCode(bcOpCode oc,bcVal a1,bcVal a2)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1=a1;
	bc.arg2=a2;
	addByteCode(bc);
}

void bcByteCodeGen::addByteCode(bcOpCode oc,bcValType vt1)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1.type=vt1;
	addByteCode(bc);
}

void bcByteCodeGen::addByteCode(bcOpCode oc,bcValType vt1,bcValType vt2)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1.type=vt1;
	bc.arg2.type=vt2;
	addByteCode(bc);
}

void bcByteCodeGen::addByteCode(bcOpCode oc,bcValType vt1,unsigned int v1)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1.type=vt1;
	bc.arg1.val=v1;
	addByteCode(bc);
}

void bcByteCodeGen::addByteCode(bcOpCode oc,bcValType vt1,unsigned int v1,bcValType vt2,unsigned int v2)
{
	bcByteCode bc;
	bc.op=oc;
	bc.arg1.type=vt1;
	bc.arg1.val=v1;
	bc.arg2.type=vt2;
	bc.arg2.val=v2;
	addByteCode(bc);
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
		addByteCode(bc);
	}

	//
	
	while(pi!=ast->tree->end())
		genStatement(this);
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
			bg->pi++;
			genExp(bg);
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
		case pn_mult:	case pn_div:	case pn_plus:	case pn_minus:	case pn_lognot:
			while(stk.size()>0)
				// either o1 is left-associative and its precedence is equal to that of o2,
				// or o1 has precedence less than that of o2,
				if(getPrecedence(bg->pi.node->data.tokens[0]) < getPrecedence(stk[stk.size()-1]->tokens.at(0)))
				{
					out.push_back(stk[stk.size()-1]);
					stk.erase(stk.end()-1,stk.end());
				}
			
				stk.push_back(&bg->pi.node->data);
			break;

			
  /*  If the token is a left parenthesis, then push it onto the stack.
    If the token is a right parenthesis:

        Until the token at the top of the stack is a left parenthesis, pop operators off the stack onto the output queue.
        Pop the left parenthesis from the stack, but not onto the output queue.
        If the token at the top of the stack is a function token, pop it onto the output queue.
        If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.
*/		
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
			if(stk[stk.size()-1]->type!=pn_funcident)
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
	bcByteCode bc;
	while(rpn->size())
	{
		switch(rpn->at(0)->type)
		{
		case pn_strlit:	case pn_fltlit:	case pn_intlit:
		case pn_true:	case pn_false:
			bc.op = oc_push;
			bc.arg1.type = bcValType::vt_bool;	
			bc.arg1.val = 0;
			bg->addByteCode(bc);
			break;
		
			//variables
		case pn_ident:	case pn_varident:	case pn_funcident:
			break;

			//operators
		case pn_logor:	case pn_logand:	case pn_equal:	case pn_notequal:	case pn_assign:
		case pn_greater:case pn_less:	case pn_lessequal:	case pn_greaterequal:
		case pn_mult:	case pn_div:	case pn_plus:	case pn_minus:	case pn_lognot:
			break;
				
		//case pn_ident:
		case pn_type:	
		default:
			break;
		}
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
		case pn_logand:	
		case pn_equal:	
		case pn_notequal:	
		case pn_assign:
		case pn_greater:
		case pn_less:	
		case pn_lessequal:	
		case pn_greaterequal:
		case pn_mult:	
		case pn_div:	
		case pn_minus:	
		case pn_lognot:
			
			break;
		case pn_plus:
			bg->addByteCode(oc_plus);
			break;	
	}
}