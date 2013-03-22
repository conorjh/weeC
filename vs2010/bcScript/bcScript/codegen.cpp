#include "codegen.h"
#include <stack>
#include "util.h"
//#include "error.h"
/*
bcByteCodeGen::bcByteCodeGen()
{
	index=NULL;
	safe=false;
	error=false;

}

bcByteCodeGen::bcByteCodeGen(tree<bcParseNode>* _in, map<string,bcSymbol>* _symtab,map<string,vector<map<string,bcSymbol>::iterator>>* _funcsigs,
	vector<bcToken>* _tokens,map<string,vector<map<string,bcSymbol>::iterator>>* _stackframes)
{
	if(_in!=NULL&&_symtab!=NULL&&_funcsigs!=NULL)
	{
		symtab=_symtab;
		in=_in;
		funcsigs=_funcsigs;
		tokens=_tokens;
		stackframes=_stackframes;
		bcByteCodeGen();
		index=in->begin();
	}
	else
	{
		//SetError(ec_cg_emptyinput,0,0);
	}
}

int bcByteCodeGen::Init(tree<bcParseNode>* _in,  map<string,bcSymbol>* _symtab,map<string,vector<map<string,bcSymbol>::iterator>>* _funcsigs,
	vector<bcToken>* _tokens,map<string,vector<map<string,bcSymbol>::iterator>>* _stackframes)
{
	istream.clear();
	if(_in!=NULL&&_symtab!=NULL&&_funcsigs!=NULL)
	{
		symtab=_symtab;
		in=_in;
		funcsigs=_funcsigs;
		tokens=_tokens;
		stackframes=_stackframes;
		index=in->begin();
		safe=true;
	}
	return 1;
}

int bcByteCodeGen::GenStatement()
{
	
	switch(index->type)
	{
	case pn_declaration:
		GenDec();
		break;

	case pn_assignment:
		GenAssignment();
		break;

	case pn_funccall:
		GenFuncCall();
		break;

	case pn_funcdec:
		GenDecFunc();
		break;			

	case pn_if:
		GenIf();
		break;
	}
	return 1;
}

int bcByteCodeGen::GenAssignment()
{
	tree<bcParseNode>::iterator it;
	if(CurrentNode()->type!=pn_assignment)
	{
		//error
		return 0;
	}
	IncNode();
	it=CurrentNode();
	IncNode();
	if(CurrentNode()->type==pn_decr||CurrentNode()->type==pn_incr)
	{
		IncNode();
		AddInstr(oc_inc,ot_stackindex,bcitos(symtab->at(it->tag).stackindex ));
		return 1;
	}
	
	GenExp();
	AddInstr( oc_mov, ot_stackindex, bcitos(symtab->at(it->tag).stackindex ) );

	return 1;

}

int bcByteCodeGen::GenIf()
{
	if(CurrentNode()->type!=pn_if)
	{
		//error
		return 0;
	}
	
	if(!IncNode(pn_exp))	return 0;
	GenExp();
	if(CurrentNode()->type!=pn_block)	return 0;
	GenBlock();

	if(CurrentNode()->type==pn_else)
	{
		IncNode();
		GenBlock();
	}

	return 1;
}

int bcByteCodeGen::Gen()
{
	if(bcCodeGen_CheckEmptyInput && in->size()<1)
	{
		//error
		return 0;
	}
	
	//check for head
	if(CurrentNode()->type==pn_head)
	{
		//SetError(ec_cg_nohead);
	}	
	IncNode();

	//global label
	AddInstr(oc_null,ot_null,"$global");
	
	//write global stackframe;
	GenStackFrame("$global");

	while(CurrentNode()!=in->end()) 
	{
		GenStatement();
    }
	
	GenStackFramePops("$global");
	
	return 1;
}

int bcByteCodeGen::AddInstr(bcOpcode oc, vector<bcOperand> ops)
{
	bcInstr ibuff(oc,ops);
	istream.push_back(ibuff);
	return istream.size()-1;
}

int bcByteCodeGen::AddInstr(bcOpcode oc, bcOperandType ot, string data)
{
	bcOperand opbuff;
	opbuff.type=ot;
	opbuff.data=data;

	bcInstr ibuff(oc,opbuff);

	istream.push_back(ibuff);
	return istream.size()-1;
}

int bcByteCodeGen::GenBlock()
{
	tree<bcParseNode>::iterator it=CurrentNode();
	int olddepth;
	olddepth=in->depth(it);

	if(CurrentNode()->type!=pn_block)
	{
		//error
		return 0;
	}
	IncNode();

	//if block is empty, this wont execute
	while(in->depth(CurrentNode())>olddepth)
	{
		GenStatement();
	}
	return 1;

}


int bcByteCodeGen::GenDec()
{	
	tree<bcParseNode>::iterator it;
	//opening node
	if(CurrentNode()->type!=pn_declaration)
	{
		return 0;
	}
	
	//get type
	if(!IncNode(pn_type))	return 0;	
	//get ident
	if(!IncNode(pn_ident))	return 0;	
	it=CurrentNode();
	IncNode();	
	//check for initialization
	if(CurrentNode()->type==pn_exp)
	{
		GenExp();
		AddInstr( oc_mov, ot_stackindex, bcitos( symtab->at(it->tag).stackindex ) );
		return 1;
	}
	else
	{
		return 1;
	}
}

int bcByteCodeGen::GenDecFunc()
{
	tree<bcParseNode>::iterator it;
	string strbuff="";
	//opening node
	if(CurrentNode()->type!=pn_funcdec)
	{
		return 0;
	}
	IncNode();

	if(CurrentNode()->type==pn_type)
	{
		IncNode();
	}	
	if(CurrentNode()->type==pn_ident)
	{
		//add label
		it=CurrentNode();
		AddInstr(oc_null,ot_null,it->tag);
		functab.insert(	make_pair( it->tag , istream.size()-1 ) );	//point iternal ident to the blank instruction
		IncNode();
	}
	GenDecParamList();
	
	//add stackframe
	GenStackFrame(it->tag);
	
	GenBlock();

	//pop the stackframe off
	GenStackFramePops(it->tag);

	return 1;
}

int bcByteCodeGen::GenStackFrame(string _tag)
{
	for(int t=0;t<stackframes->at(_tag).size();++t)
	{
		if(stackframes->at(_tag).at(t)->second.datatype=="int")
		{
			AddInstr(oc_push,ot_int,stackframes->at(_tag).at(t)->second.init);
		}
		else if(stackframes->at(_tag).at(t)->second.datatype=="float")
		{
			AddInstr(oc_push,ot_float,stackframes->at(_tag).at(t)->first);
		}
		else if(stackframes->at(_tag).at(t)->second.datatype=="string")
		{
			AddInstr(oc_push,ot_string,stackframes->at(_tag).at(t)->first);
		}
		else if(stackframes->at(_tag).at(t)->second.datatype=="bool")
		{
			AddInstr(oc_push,ot_bool,stackframes->at(_tag).at(t)->first);
		}
		else if(stackframes->at(_tag).at(t)->second.datatype=="object")
		{
			AddInstr(oc_push,ot_object,stackframes->at(_tag).at(t)->first);
		}
	}	
	return 1;
}

int bcByteCodeGen::GenStackFramePops(string _tag)
{
	for(int t=0;t<stackframes->at(_tag).size();++t)
	{
		AddInstr(oc_pop,ot_null,"");
	}
	return 1;
}

int bcByteCodeGen::GenDecParamList()
{
	tree<bcParseNode>::iterator it=CurrentNode();
	int olddepth;
	olddepth=in->depth(it);

	if(CurrentNode()->type!=pn_decparamlist)
	{
		//error
		return 0;
	}
	IncNode();

	
	while(in->depth(CurrentNode())>olddepth)
	{
		switch(CurrentNode()->type)
		{
		case pn_comma:
			IncNode();
			break;

		default:
			GenDecInParam();
			break;
		}
	}
	return 1;
}

int bcByteCodeGen::GenDecInParam()
{
	//opening node
	if(CurrentNode()->type!=pn_paramdeclaration)
	{
		return 0;
	}
	
	//get type
	if(!IncNode(pn_type))	return 0;		
	//get ident
	if(!IncNode(pn_ident))	return 0;	
	//check for initialization
	IncNode();
	return 1;
}

int bcByteCodeGen::GenFuncCall()
{
	if(CurrentNode()->type!=pn_funccall)
	{
		//error
		return 0;
	}
	IncNode(pn_function);
	AddInstr(oc_push,ot_register,"pc");		//push the current instruction index
	AddInstr(oc_inc,ot_null,"");			//add one to it
	AddInstr(oc_call,ot_instrindex,bcitos(functab[CurrentNode()->tag]));	//push return address;
	IncNode();
	GenParamList();
	
	return 1;
}

int bcByteCodeGen::GenParamList()
{
	tree<bcParseNode>::iterator it=CurrentNode();
	int olddepth=in->depth(it);
	
	if(CurrentNode()->type!=pn_paramlist)
	{
		//error
		return 0;
	}
	IncNode();

	//will not execute if ident() or empty
	while(in->depth(CurrentNode())>olddepth)
	{
		GenExp();
	}
	return 1;
}

int bcByteCodeGen::GenExp()
{
	//check current node is pn_exp
	if(CurrentNode()->type!=pn_exp) return 0;

	tree<bcParseNode>::iterator start=CurrentNode();
	tree<bcParseNode>::iterator ident,type;
	bcInstr ibuff;
	bcToken tokbuff;
	stack<bcToken> opstack;
	vector<bcToken> output;
	map<string,bcSymbol>::iterator it;
	int olddepth=in->depth(start);

	IncNode();

	//Convert to RPN
	while(in->depth(CurrentNode())>olddepth)
	{
		
		switch(CurrentNode()->type)
		{
		case pn_var:
			tokbuff=tokens->at(CurrentNode()->tokens.at(0));
			tokbuff.data=bcitos(symtab->at(CurrentNode()->tag).stackindex);	//add the stackindex by giving the nodes tag to symtab map
			output.push_back(tokbuff);
			IncNode();
			break;

		case pn_funccall:
			IncNode(pn_function);
			output.push_back(tokens->at(CurrentNode()->tokens.at(0)));
			IncNode(pn_paramlist);
			GenParamList();
			break;		

		//add numbers/strings/floats straight to output
		case pn_strlit:
			//if not in string table, add it and later update
			if( find( strlittab.begin(), strlittab.end(), tokens->at(CurrentNode()->tokens.at(0)).data)==strlittab.end() )
			{
				strlittab.push_back(tokens->at(CurrentNode()->tokens.at(0)).data);
			}
			output.push_back(tokens->at(CurrentNode()->tokens.at(0)));
			IncNode();
			break;

		case pn_intlit:
		case pn_fltlit:		
		case pn_true:
		case pn_false:
			output.push_back(tokens->at(CurrentNode()->tokens.at(0)));
			IncNode();
			break;

		//operator
		case pn_plus:
		case pn_minus:
		case pn_div:
		case pn_mult:			
		case pn_mod:
		case pn_pow:
		case pn_less:
		case pn_greater:
		case pn_lessequal:
		case pn_greaterequal:
		case pn_equal:
		case pn_notequal:
		case pn_logand:
		case pn_logor:
		case pn_lognot:
			while(opstack.size()!=0 && IsOperator(opstack.top()))
			{
				if( GetPrecedence(opstack.top()) <= GetPrecedence(tokens->at(CurrentNode()->tokens.at(0))) )
				{
					output.push_back(opstack.top());
					opstack.pop();
				}				
			}
			opstack.push(tokens->at(CurrentNode()->tokens.at(0)));
			IncNode();
			break;

		case pn_oparen:
			opstack.push(tokens->at(CurrentNode()->tokens.at(0)));
			IncNode();
			break;

		case pn_cparen:
			while(opstack.top().type!=tt_oparen)
			{
				output.push_back(opstack.top());
				opstack.pop();
			}
			opstack.pop();
			if(opstack.size()!=0 && opstack.top().type==tt_ident)
			{
				output.push_back(opstack.top());
				opstack.pop();
			}
			IncNode();
			break;
		}
		
	}
	
	//add anything left in opstack to output
	if(opstack.size()!=0)
	{
		while(opstack.size()>0)
		{
			output.push_back(opstack.top());
			opstack.pop();
		}
	}
	
	//create instructions from the RPN
	while(output.size()>0)
	{
		switch(output.at(0).type)
		{
			//lits
		case tt_intlit:
			AddInstr(oc_push,ot_int,output.at(0).data);
			break;
		case tt_fltlit:
			AddInstr(oc_push,ot_float,output.at(0).data);
			break;
		case tt_strlit:
			for(int t=0;t<strlittab.size();++t)
			{
				if(strlittab.at(t)==output.at(0).data)
				{
					AddInstr(oc_push,ot_strlit,bcitos(t));
					t=strlittab.size();
				}
			}			
			break;
		case tt_ident:
			AddInstr(oc_push,ot_ident,output.at(0).data);
			break;
			//operators
		case tt_plus:
			AddInstr(oc_add,ot_null,output.at(0).data);
			break;
		case tt_minus:
			AddInstr(oc_sub,ot_null,output.at(0).data);
			break;
		case tt_div:
			AddInstr(oc_div,ot_null,output.at(0).data);
			break;
		case tt_mult:		
			AddInstr(oc_mult,ot_null,output.at(0).data);
			break;		
		case tt_mod:		
			AddInstr(oc_mod,ot_null,output.at(0).data);
			break;		
		case tt_pow:		
			AddInstr(oc_pow,ot_null,output.at(0).data);
			break;		
		case tt_less:
			AddInstr(oc_ltn,ot_null,output.at(0).data);
			break;
		case tt_greater:
			AddInstr(oc_gtn,ot_null,output.at(0).data);
			break;
		case tt_lessequal:
			AddInstr(oc_leq,ot_null,output.at(0).data);
			break;
		case tt_greaterequal:
			AddInstr(oc_geq,ot_null,output.at(0).data);
			break;
		case tt_equal:
			AddInstr(oc_eq,ot_null,output.at(0).data);
			break;
		case tt_notequal:
			AddInstr(oc_neq,ot_null,output.at(0).data);
			break;
		case tt_logand:
			AddInstr(oc_land,ot_null,output.at(0).data);
			break;
		case tt_logor:
			AddInstr(oc_lor,ot_null,output.at(0).data);
			break;
		}
		output.erase(output.begin());
	}
	
	return 1;	
}

tree<bcParseNode>::iterator bcByteCodeGen::CurrentNode()
{
	if(safe)
	{
		return index;
	}
	else
	{
		return in->end();
	}
}

tree<bcParseNode>::iterator bcByteCodeGen::NextNode()
{
	if(IncNode())
	{
		return index;
	}
	else
	{
		return in->end();
	}
}

int bcByteCodeGen::IncNode()
{
	tree<bcParseNode>::iterator indexplus=index;
	indexplus++;
	if(indexplus!=in->end())
	{
		++index;
		return 1;
	}
	safe=false;
	return 0;
}

int bcByteCodeGen::IncNode(bcParseNodeType expect)
{
	tree<bcParseNode>::iterator indexplus=index;
	indexplus++;
	if(indexplus!=in->end())
	{
		if(indexplus->type==expect)
		{
			++index;
			return 1;
		}
		else
		{
			SetError(ec_cg_unexpectednode);
			safe=false;
			return 0;
		}
	}	
	else
	{
		safe=false;
		return 0;
	}
}

void bcByteCodeGen::SetError(bcErrorCode input)
{
	errorcode=input;
	error=true;
}

bool bcByteCodeGen::IsError()
{
	return error;
}

bcErrorCode bcByteCodeGen::GetError()
{
	return errorcode;
}*/