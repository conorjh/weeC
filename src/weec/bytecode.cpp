#include "bytecode.h"
#include "lexer.h"
#include "util.h"
#include <vector>
#include <unordered_map>

using namespace std;
using namespace wc::lex;
using namespace wc::vm;
using namespace wc::parse;
using namespace wc::util;

wc::vm::wcStack::wcStack()
{

}

void wc::vm::wcStack::clear()
{
	cont.clear();
}

void wc::vm::wcStack::clear(wcExecContext* ec)
{
	cont.clear();
	if (ec)
		ec->reg[tos] = 0;
}

int wc::vm::wcStack::pop()
{
	int ret = cont[cont.size() - 1];
	cont.pop_back();
	return ret;
}

//pops top stack element, updates top of stack register
int wc::vm::wcStack::pop(wcExecContext* ec)
{
	int ret = 0;
	if (cont.size())
		ret = cont[cont.size() - 1];
	else
		return 0;

	cont.pop_back();

	//change top of stack register
	if (ec)
		ec->reg[tos]--;
	return ret;
}

int* wc::vm::wcStack::at(int p_i)
{
	//if p_i is minus, treat as a relative index from top of stack
	if (p_i < 0)
		return &cont[cont.size() - 1 + p_i];
	//otherwise, absolute reference
	return &cont[p_i];
}

int wc::vm::wcStack::top()
{
	return cont[cont.size() - 1];
}

int wc::vm::wcStack::size()
{
	return cont.size();
}

void wc::vm::wcStack::push(int a)
{
	cont.push_back(a);
}

void wc::vm::wcStack::push(wcExecContext* ec, int a)
{
	cont.push_back(a);
	if (ec)
		ec->reg[tos]++;
}

wc::vm::wcByteCode::wcByteCode()
{
	arg1 = 0;
	arg2 = 0;
	op = oc_nop;
}

wc::vm::wcByteCode::wcByteCode(wcOpCode p_oc)
{
	op = p_oc;
	arg1 = arg2 = 0;
}

wc::vm::wcByteCode::wcByteCode(wcOpCode p_oc, int p_arg1)
{
	op = p_oc;
	arg1 = p_arg1;
}

wc::vm::wcByteCode::wcByteCode(wcOpCode p_oc, int p_arg1, int p_arg2)
{
	op = p_oc;
	arg1 = p_arg1;
	arg2 = p_arg2;
}

wc::vm::wcByteCode::wcByteCode(int p_oc)
{
	op = wcOpCode(p_oc);
}

wc::vm::wcByteCode::wcByteCode(int p_oc, int p_arg1)
{
	op = wcOpCode(p_oc);
	arg1 = p_arg1;
}

wc::vm::wcByteCode::wcByteCode(int p_oc, int p_arg1, int p_arg2)
{
	op = wcOpCode(p_oc);
	arg1 = p_arg1;
	arg2 = p_arg2;
}

wc::vm::wcExecContext::wcExecContext()
{
	this->regFlags = 0;
	for (int t = 0; t < wcMaxRegisters; t++)
		reg[t] = 0;
	halt = false;
}

void wc::vm::wcExecContext::clear()
{
	halt = false;
	istream.clear();
	stack.clear();
	symTab.clear();
	for (int t = 0; t < wcMaxRegisters; ++t)
	{
		reg[t] = 0;
		regFlags[t] = 0;
	}
}

int wc::vm::getValTypeSize(wcValType t)
{
	switch (t)
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

wcValType wc::vm::getValType(wcSymbol* sym)
{
	switch (sym->type)
	{
	case st_type:
	case st_var:
		if (sym->dataType == "int")
			return vt_int;
		else if (sym->dataType == "float")
			return vt_float;
		else if (sym->dataType == "string")
			return vt_string;
		else if (sym->dataType == "bool")
			return vt_bool;
		else
			return vt_mem;
		break;
	}
	return vt_null;
}

wc::vm::wcByteCodeGen::wcByteCodeGen()
{
	inDecFunc = false;
	errorCode = ec_null;
}

unsigned int wc::vm::wcByteCodeGen::addByteCode(wcByteCode wc)
{
	istream->push_back(wc);
	return istream->size() - 1;
}

unsigned int wc::vm::wcByteCodeGen::addByteCode(wcOpCode oc)
{
	wcByteCode wc;
	wc.op = oc;
	return addByteCode(wc);
}

unsigned int wc::vm::wcByteCodeGen::addByteCode(wcOpCode oc, int a1)
{
	wcByteCode wc;
	wc.op = oc;	wc.arg1 = a1;	wc.arg2 = 0;
	return addByteCode(wc);
}

unsigned int wc::vm::wcByteCodeGen::addByteCode(wcOpCode oc, int a1, int a2)
{
	wcByteCode wc;
	wc.op = oc;	wc.arg1 = a1;	wc.arg2 = a2;
	return addByteCode(wc);
}

wcByteCode* wc::vm::wcByteCodeGen::getByteCode(unsigned int ind)
{
	return &istream->at(ind);
}

wcByteCode* wc::vm::wcByteCodeGen::getByteCode(unsigned int ind, bool isFunc)
{
	return &istream->at(ind);
}

int wc::vm::wcByteCodeGen::getError()
{
	return errorCode;
}

wcExecContext* wc::vm::wcByteCodeGen::gen()
{
	output = new wcExecContext();
	wcByteCode wc;
	istream = new std::vector<wcByteCode> ;
	pi = ast->tree.begin();

	//push command line args


	//gen script functions and global statements
	while (pi != ast->tree.end())
		genStatement(this);
	
	addByteCode(wcByteCode(oc_halt));

	//add all functions below main;
	genAppendFuncIstreams(this,istream);

	output->istream = *this->istream;
	return output;
}

void wc::vm::genStatement(wcByteCodeGen* bg)
{
	int olddepth = bg->ast->tree.depth(bg->pi);
	bg->pi++;

	while (bg->pi != bg->ast->tree.end() && bg->ast->tree.depth(bg->pi) >= olddepth)
		switch (bg->pi->type)
	{
		case pn_null:
		case pn_head:
		case pn_statement:
			bg->pi++;
			break;
		
		case pn_exp:
			genExp(bg);
			bg->addByteCode(oc_popr, eax);
			break;
		case pn_namespacedec:
			genDecNamespace(bg);
			break;
		case pn_funcdec:
			genDecFunc(bg);
			break;
		case pn_funccall:
			genFuncCall(bg);
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
		case pn_return:
			genReturn(bg);
			break;
		default:
			++bg->pi;
	}
}

void wc::vm::genDecParamList(wcByteCodeGen* bg)
{
	int olddepth = bg->ast->tree.depth(bg->pi);	++bg->pi;
	while (bg->ast->tree.depth(bg->pi) > olddepth)
		bg->pi++;
}

void wc::vm::genDecFunc(wcByteCodeGen* bg)
{
	tree<wcParseNode>::iterator oldpi;
	int oldStackOffset = bg->stackOffset;
	wcFuncInfo* fi=nullptr;
	std::string paramString;
	bg->inDecFunc = true;
	vector<wcByteCode> funcIstream;
	vector<wcByteCode>* oldIstream;

	//collect func dec info from current node
	int olddepth = bg->ast->tree.depth(bg->pi);	++bg->pi;
	while (bg->ast->tree.depth(bg->pi) > olddepth)
		switch (bg->pi->type)
		{
			case pn_funcident:
				fi = &bg->ast->funcTab.at(bg->pi.node->data.tokens.at(0).data);
				++bg->pi;
				break;

			case pn_block:
				oldpi = bg->pi;
				bg->pi = fi->body;
				bg->stackOffset = 0;
				
				//generate instructions into a seperate container from $global
				oldIstream = bg->istream;
				bg->istream = &funcIstream;
				genBlock(bg);
				bg->fistream.insert(make_pair(fi->fullIdent, funcIstream));
				bg->istream = oldIstream;
				bg->stackOffset = oldStackOffset;
				break;

			case pn_decparamlist:
				paramString = bg->pi->tokens.at(0).data;
				genDecParamList(bg);
				break;

			case pn_type:
			default:
				++bg->pi;
				break;
		}
	bg->inDecFunc = false;
}

void wc::vm::genDecNamespace(wcByteCodeGen* bg)
{
	//collect func dec info from current node
	int olddepth = bg->ast->tree.depth(bg->pi);	++bg->pi;
	while (bg->ast->tree.depth(bg->pi) > olddepth)
		genBlock(bg);
}

void wc::vm::genFuncCall(wcByteCodeGen *bg)
{
	std::string ident;
	int olddepth = bg->ast->tree.depth(bg->pi);
	++bg->pi;
	while (bg->ast->tree.depth(bg->pi) > olddepth)
		switch (bg->pi->type)
		{
		case pn_exp:
			genExp(bg);
			break;

		case pn_type:
			break;
		case pn_ident:
			ident = bg->pi->tokens.at(0).data;
			break;
		default:
			bg->pi->type;
			++bg->pi;
			break;
		}
}

void wc::vm::genDecVar(wcByteCodeGen* bg)
{
	bool hasExp = false;
	int varOffset = 0;
	int varSize = 0;
	int olddepth = bg->ast->tree.depth(bg->pi);
	++bg->pi;
	while (bg->ast->tree.depth(bg->pi) > olddepth)
		switch (bg->pi->type)
		{
		case pn_exp:
			genExp(bg);
			bg->addByteCode(oc_popr, eax);
			hasExp = true;
			break;

		case pn_type:
			++bg->pi;
			break;

		case pn_ident:
			varOffset = bg->stackOffset;
			varSize = parse::getSymbolStackSize(*bg->ast->getSymbol(bg->pi->tokens.at(0).data));
			for (int t = 0; t < varSize; ++t)
			{
				bg->addByteCode(oc_push, 0);	//reserve space for this variable
				bg->stackOffset++;
			}
			//set the offset of this symbol to the current local stack size, noted in bg->stackOffset
			bg->ast->symTab.at(getTokenFromNode(tt_ident, &bg->pi.node->data).data).offset = varOffset;
			++bg->pi;
			break;

		default:
			++bg->pi;
			break;
		}
}

void wc::vm::genReturn(wcByteCodeGen* bg)
{
	int olddepth = bg->ast->tree.depth(bg->pi);
	++bg->pi;

	genExp(bg);

	bg->addByteCode(oc_popr, eax);		//set expression register with the result on the top of stack
	bg->addByteCode(oc_mov, eax, ret);	//copy expression register to return register
	bg->addByteCode(oc_halt);			//TODO remove this after we get functions working
}

void wc::vm::genIf(wcByteCodeGen* bg)
{
	//collect func dec info from current node
	unsigned int truejump = 0; unsigned int elsejump, ifEnd;
	unsigned int ifJump;
	bool hasElse = false;
	int ifBegin = bg->istream->size() - 1;
	int olddepth = bg->ast->tree.depth(bg->pi);	++bg->pi;
	while (bg->ast->tree.depth(bg->pi) > olddepth)
		switch (bg->pi->type)
		{
			case pn_exp:
				genExp(bg);
				//this bytecode is equivalent to if()
				bg->addByteCode(oc_push, 1);
				bg->addByteCode(oc_cmp);
				ifJump = bg->addByteCode(oc_jne, 0);
				break;

			case pn_if_trueblock:
				genBlock(bg);
				//truejump = bg->addByteCode(oc_jmp);
				break;


			case pn_if_elseblock:
				hasElse = true;
				elsejump = bg->istream->size();
				genBlock(bg);
				break;

			default:
				bg->pi++;
				break;
		}

	//special case - if this is the first line of code in istream;
	if (ifBegin < 0)
		ifBegin = 0;
	
	//if this is the last statement, we need an address to jump to if the "if" statement was false
	ifEnd = bg->addByteCode(oc_nop);
	
	//adjust the if statement jump - no else means jump past the whole bit of code we just generated,
	//and if it has an else, jump to the else block instead
	if(hasElse)
		bg->getByteCode(ifJump)->arg1 = elsejump;
	else
		bg->getByteCode(ifJump)->arg1 = ifEnd;

}

//initially generates parsenodes in RPN order, then 
void wc::vm::genExp(wcByteCodeGen* bg)
{
	bool foundParen = false;
	std::vector<wcParseNode*> out, stk;
	int olddepth = bg->ast->tree.depth(bg->pi);	++bg->pi;
	while (bg->ast->tree.depth(bg->pi) > olddepth)
	{
		switch (bg->pi->type)
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
		case pn_ident:	case pn_varident:	case pn_funcident:	case pn_funccall:
			out.push_back(&bg->pi.node->data);
			//bg->pi.node->data
			break;

			//operators
		case pn_logor:	case pn_logand:	case pn_equal:	case pn_notequal:	case pn_assign:
		case pn_greater:case pn_less:	case pn_lessequal:	case pn_greaterequal:
		case pn_mult:	case pn_div:	case pn_plus:	case pn_minus:	case pn_lognot:	case pn_expo:	case pn_mod:
			while (
				stk.size() > 0 && (getPrecedence(bg->pi.node->data.tokens[0]) == getPrecedence(stk[stk.size() - 1]->tokens.at(0)) && !getAssociativity(bg->pi.node->data.tokens[0]) ||
				getAssociativity(bg->pi.node->data.tokens[0]) && getPrecedence(bg->pi.node->data.tokens[0]) < getPrecedence(stk[stk.size() - 1]->tokens.at(0)))
				)
			{
				out.push_back(stk[stk.size() - 1]);
				stk.erase(stk.end() - 1, stk.end());
			}
			stk.push_back(&bg->pi.node->data);
			break;

		case pn_oparen:
			stk.push_back(&bg->pi.node->data);
			break;

		case pn_cparen:
			foundParen = false;
			//pop stack to ouput til we find a (, error if not
			while (!foundParen && stk.size())
				if (stk[stk.size() - 1]->type == pn_oparen)
				{
					foundParen = true;
				}
				else
				{
					out.push_back(stk[stk.size() - 1]);
					stk.erase(stk.end() - 1, stk.end());
				}

			//mismatched params - error
			if (!foundParen)
				return;
			stk.erase(stk.end() - 1, stk.end());

			//func ident
			if (stk.size() && stk[stk.size() - 1]->type == pn_funcident)
			{
				out.push_back(stk[stk.size() - 1]);
				stk.erase(stk.end() - 1, stk.end());
			}
			break;

		case pn_type:
		default:
			break;
		}
		++bg->pi;
	}

	//pop off remaining operators
	while (stk.size() > 0)
	{
		out.push_back(stk[stk.size() - 1]);
		stk.erase(stk.end() - 1, stk.end());
	}
	genRpnToByteCode(bg, &out);
}

void wc::vm::genBlock(wcByteCodeGen* bg)
{
	int olddepth = bg->ast->tree.depth(bg->pi); bg->pi++;
	while (bg->ast->tree.depth(bg->pi) > olddepth)
		genStatement(bg);

}

//rpn is provided as a vector of parsenodes
void wc::vm::genRpnToByteCode(wcByteCodeGen* bg, std::vector<wcParseNode*>* rpn)
{
	wcParseNode* lastnode;
	wcSymbol* identSym;
	//	wcByteCode wc;
	while (rpn->size())
	{
		switch (rpn->at(0)->type)
		{
		case pn_strlit:
			//bg->addByteCode(oc_push,vt_string,wcstoi(rpn->at(0)->tokens.at(0).data));
			break;
		case pn_fltlit:
			bg->addByteCode(oc_push, wcstof(rpn->at(0)->tokens.at(0).data));
			break;
		case pn_intlit:
			bg->addByteCode(oc_push, wcstoi(rpn->at(0)->tokens.at(0).data));
			break;
		case pn_true:
			bg->addByteCode(oc_push, 1);
			break;
		case pn_false:
			bg->addByteCode(oc_push, 0);
			break;
			
			//variables
		case pn_ident:
		case pn_varident:
			identSym = bg->ast->getSymbol(rpn->at(0)->tokens.at(0).data);
			if (identSym->isArray)
				bg->addByteCode(oc_pushfs, bg->ast->getSymbol(rpn->at(0)->tokens.at(0).data)->offset + wcstoi(rpn->at(0)->tokens.at(2).data));
			else
				bg->addByteCode(oc_pushfs, bg->ast->getSymbol(rpn->at(0)->tokens.at(0).data)->offset);	//push value from stack, using fullident from tokens[0]
			break;

		case pn_funcident: case pn_funccall:
			bg->addByteCode(oc_call, wcstoi(rpn->at(0)->tokens.at(0).data));
			break;
			//operators
		case pn_lognot:
			bg->addByteCode(oc_not);break;
		case pn_logor:
			bg->addByteCode(oc_or);break;
		case pn_logand:
			bg->addByteCode(oc_and);break;
		case pn_equal:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jne, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_notequal:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_je, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_greater:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jle, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_less:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jge, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_lessequal:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jg, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_greaterequal:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jl, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_assign:
			identSym = bg->ast->getSymbol(rpn->at(0)->tokens.at(1).data);
			if (identSym->isArray)
				bg->addByteCode(oc_assign, identSym->offset + wcstoi(rpn->at(0)->tokens.at(2).data));
			else
				bg->addByteCode(oc_assign, identSym->offset);
			break;
		case pn_mult:
			bg->addByteCode(oc_mult);break;
		case pn_div:
			bg->addByteCode(oc_div);break;
		case pn_plus:
			bg->addByteCode(oc_plus);break;
		case pn_minus:
			bg->addByteCode(oc_minus);break;
		case pn_mod:
			bg->addByteCode(oc_mod);break;
		case pn_expo:
			bg->addByteCode(oc_expo);break;
		case pn_type:
		default:
			break;
		}
		lastnode = rpn->at(0);
		rpn->erase(rpn->begin());
	}

}


void wc::vm::genAppendFuncIstreams(wcByteCodeGen *bg, std::vector<wcByteCode> *istream)
{
	for (auto t = bg->fistream.begin(); t != bg->fistream.end(); ++t)
		for (auto y = 0; y < t->second.size(); ++y)
			istream->push_back(t->second.at(y));
}

void wc::vm::adjustJumps(wcByteCodeGen* bg, int beg, int end, int add)
{
	for (int t = beg; t <= end; t++)
		switch (bg->istream->at(t).op)
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

//search all the tokens attached to a node for a given type, and return the wcToken
//with that type, return a blank wcToken if it wasnt found
wcToken wc::vm::getTokenFromNode(wc::lex::wcTokenType p_type, wcParseNode* p_node)
{
	for (auto t = p_node->tokens.begin(); t != p_node->tokens.end(); ++t)
		if (t->type == p_type)
			return *t;
	return wcToken();
}