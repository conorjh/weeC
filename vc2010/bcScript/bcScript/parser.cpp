#include "parser.h"

bcParser::bcParser()
{	
	safe=false;
	index=0;
	parencount=0;
	bracecount=0;
	whilecount=0;
	funccount=0;
	error=false;
	nodec=false;
	errorcode=ec_null;	
	bcSymbol symbuff;
	Init();	
}

void bcParser::ClearPNBuff()
{
	pnbuff.tag="";
	pnbuff.tokens.clear();
	pnbuff.type=pn_unk;	
}

bcToken* bcParser::ReadToken(bcTokenType expected)
{
	static bcToken out;
	out.type=tt_eof;

	if(index+1 > in->size()-1)
	{
		return &out;
	}
	else
	{
		index++;
		if(in->at(index).type!=expected)
		{
			SetError(ec_par_invalidtoken,in->at(index).line,in->at(index).col);
			out.type=tt_null;
			return &out;
		}
		else
		{
			return &in->at(index);
		}
	}
}

bcToken* bcParser::ReadToken()
{
	static bcToken out;
	out.type=tt_eof;

	if(index+1 > in->size()-1)
	{
		safe=false;
		++index;
		return &out;
	}
	else
	{
		++index;
		return &in->at(index);
	}
}

bcToken* bcParser::CurrentToken()
{
	static bcToken out;
	out.type=tt_eof;

	if(safe)
	{
		return &in->at(index);
	}
	else
	{
		return &out;
	}
}

bcToken* bcParser::NextToken()
{
	static bcToken out;
	out.type=tt_eof;

	if(index+1 > in->size()-1)
	{
		return &out;
	}
	else
	{
		return &in->at(index+1);
	}
}

bcToken* bcParser::PreviousToken()
{
	static bcToken out;
	out.type=tt_eof;

	if( (index <1) && (index-1<in->size()-1) )
	{
		return &out;
	}
	else
	{
		return &in->at(index-1);
	}
}

//expand #include etc
int bcParser::PreParse()
{
	
	return 1;
}

int bcParser::Parse(vector<bcToken>* input)
{
	in=input;
	return Parse();
}

int bcParser::Init()
{
	bcSymbol symbuff;
	stackframes.clear();
	ptree.clear();
	symtab.clear();
	funcsigs.clear();
	while(openscopes.size()!=0)	openscopes.pop();

	//create the head of the parse tree
	pindex=ptree.begin();
	pnbuff.type=pn_head;
	pindex=ptree.insert(pindex,pnbuff);
	pnbuff.type=pn_unk;
	//create the global namespace, from which all scopes derive
	symbuff.type=st_namespace;
	symbuff.ident="$global";
	//set the current scope, and function frame to global
	scope=symtab.insert( symtab.begin(),make_pair("$global",symbuff));
	scope->second.scope=scope;	//set global's scope to global lol!
	openscopes.push(scope);
	//add builtin types, function sigs require a vector of symtable references, but Parse() 
	//wont search the symtab when it finds int/float/string etc
	AddSymbol("int",st_type,"int",scope);
	AddSymbol("string",st_type,"string",scope);
	AddSymbol("float",st_type,"float",scope);
	AddSymbol("bool",st_type,"bool",scope);
	AddSymbol("object",st_type,"object",scope);	

	safe=true;
	index=0;
	return 1;
}

int bcParser::Parse()
{	
	Init();
	vector<map<string,bcSymbol>::iterator> stackframe;
	stackframebuff=&stackframe;	

	if(bcParser_CheckEmptyInput && in->size()<1)
	{
		//error
		return 0;
	}
		
	while(CurrentToken()->type!=tt_eof)
	{
		ParseStatement();	
		if(IsError())	return 0;		
	}

	stackframes.insert( make_pair( "$global",stackframe) );

	return 1;
}

int bcParser::ParseStatement()
{
	if(bcParser_CheckEmptyInput && in->size()<1)
	{
		//error
		return 0;
	}
	
	switch(CurrentToken()->type)
	{
		case tt_eof:
			return 1;

		case tt_scolon:
			//empty statement
			ReadToken();
			return 1;
			break;

		case tt_while:
			return ParseWhile();
			break;

		case tt_break:
			return ParseBreak();
			break;

		case tt_return:
			return ParseReturn();
			break;

		case tt_continue:
			return ParseContinue();
			break;

		//symbol detected, 
		case tt_ident:
			//Check to see if symbol exists
			if(GetSymbol(in->at(index).data)!=NULL)
			{
				switch(GetSymbol(in->at(index).data)->type)
				{
				case st_object:
				case st_type:
					if(!nodec)
					{
						return ParseDec();
					}
					else
					{
						//error cannot declare in this block
						SetError(ec_par_illegaldec,CurrentToken()->line,CurrentToken()->col);
						return 0;
					}
					break;

				case st_var:
					return ParseAssignment();
					break;

				case st_function:
					//function call
					ParseFuncCall();
					if(CurrentToken()->type!=tt_scolon)
					{
						SetError(ec_par_noscolon,CurrentToken()->line,CurrentToken()->col);
						return 0;
					}
					return 1;
					break;

				default:
					//Error, 
					SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
					return 0;
					break;
				}
			}
			else
			{
				//might be a new object def
				SetError(ec_par_undeclaredident,CurrentToken()->line,CurrentToken()->col);
				return 0;
			}
			break;

		case tt_var:
		case tt_int:
		case tt_string:
		case tt_float:
		case tt_bool:
		case tt_object:
		case tt_function:
		case tt_dec:
			//in a declaration
			if(!nodec)
			{
				return ParseDec();			
			}
			else
			{
				//error cannot declare in this block
				SetError(ec_par_illegaldec,CurrentToken()->line,CurrentToken()->col);
				return 0;
			}
			break;
	
		case tt_obrace:
			++bracecount;
			ParseBlock();
			--bracecount;
			break;

		case tt_cbrace:
			SetError(ec_par_invalidbrace,CurrentToken()->line,CurrentToken()->col);
			return 0;
			break;

		case tt_if:
			return ParseIf();
			break;

		default:
			SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
			return 0;
			break;
			
	}

	if(IsError())
	{
		return 0;
	}
	return 1;
}

int bcParser::ParseAssignment()
{
	tree<bcParseNode>::iterator oldpindex=pindex;
	tree<bcParseNode>::iterator identnode;	
	bcToken operand1,operand2;
	pnbuff.type=pn_assignment;
	pindex=ptree.append_child(oldpindex,pnbuff);
	ClearPNBuff();
	
	//check for valid ident first
	if(CurrentToken()->type!=tt_ident)
	{
		SetError(ec_par_invalidtoken, CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	
	
	if(GetSymbol(CurrentToken()->data)!=NULL)
	{
		switch(GetSymbol(CurrentToken()->data)->type)
		{
		case st_var:
			pnbuff.type=pn_var;
			pnbuff.tokens.push_back(index);					
			if(GetSymbolIt(CurrentToken()->data)->second.scope->first=="$global")
			{
				pnbuff.tag=CurrentToken()->data;
			}
			else
			{
				pnbuff.tag=GetSymbolIt(CurrentToken()->data)->second.scope->first+"."+CurrentToken()->data;
			}				
			identnode=ptree.append_child(pindex,pnbuff);
			ClearPNBuff();
			operand1=in->at(index);
			break;

		case st_object:
			pnbuff.type=pn_object;
			pnbuff.tokens.push_back(index);		
			if(GetSymbolIt(CurrentToken()->data)->second.scope->first=="$global")
			{
				pnbuff.tag=CurrentToken()->data;
			}
			else
			{
				pnbuff.tag=GetSymbolIt(CurrentToken()->data)->second.scope->first+"."+CurrentToken()->data;
			}
			identnode=ptree.append_child(pindex,pnbuff);
			ClearPNBuff();
			operand1=in->at(index);
			break;

		default:
			//only allow objects/vars
			break;
		}
	}

	//check for ++/--
	ReadToken();
	switch(CurrentToken()->type)
	{
	case tt_assign:
		ReadToken();
		break;

	case tt_incr:
		ReadToken(tt_scolon);
		pnbuff.type=pn_incr;
		pindex=ptree.append_child(pindex,pnbuff);
		ClearPNBuff();			
		ReadToken();
		pindex=oldpindex;
		return 1;
		break;

	case tt_decr:
		ReadToken(tt_scolon);
		pnbuff.type=pn_decr;
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();			
		ReadToken();
		return 1;
		break;

	default:
		//error 
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}	

	//no need to drop back to pre expression depth, exiting the method will handle with oldpindex
	pnbuff.type=pn_exp;
	pindex=ptree.append_child(pindex,pnbuff);
	ClearPNBuff();
	operand2=ParseExp();
	if(IsError())	return 0;	

	//check type compatibility
	if(!CheckOperandTypes(operand1,operand2,tt_assign))
	{
		SetError(ec_par_invalidop,operand1.line,operand1.col);
		return 0;
	}		
	
	if(CurrentToken()->type!=tt_scolon)
	{
		SetError(ec_par_noscolon,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	ReadToken();

	pindex=oldpindex;
	if(IsError())	return 0;
	return 1;
}

bool bcParser::CompareFuncSigs(string sig1name, vector<map<string,bcSymbol>::iterator> sig1)
{
	if(funcsigs.find(sig1name)==funcsigs.end())
	{
		//error, doesnt match any function names
		return false;
	}
	
	if(funcsigs[sig1name]==sig1)
	{
		return true;
	}
	return false;
}

int bcParser::ParseFuncCall()
{
	tree<bcParseNode>::iterator oldpindex=pindex;
	tree<bcParseNode>::iterator ident;
	int identpos=index;
	bcSymbol* symbuff=GetSymbol(CurrentToken()->data);
	
	pnbuff.type=pn_funccall;
	pindex=ptree.append_child(oldpindex,pnbuff);
	ClearPNBuff();

	if(symbuff!=NULL)
	{
		if(symbuff->type!=st_function)
		{
			SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
			return 0;
		}
	}
	else
	{
		SetError(ec_par_undeclaredident,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}

	pnbuff.tokens.push_back(index);
	pnbuff.type=pn_function;
	ident=ptree.append_child(pindex,pnbuff);	
	ClearPNBuff();
	ReadToken();

	ParseParamList();	
	if(IsError())return 0;

	//check parameters match the functions defined parameters
	if(!CompareFuncSigs(in->at(identpos).data,callsigbuff))
	{
		SetError(ec_par_paramsdontmatch,in->at(identpos).line,in->at(identpos).col);
		return 0;
	}
	callsigbuff.clear();
	pindex=oldpindex;
	return 1;
}

int bcParser::ParseWhile()
{
	tree<bcParseNode>::iterator oldpindex=pindex;
	++whilecount;

	if(CurrentToken()->type!=tt_while)
	{
		SetError(ec_par_invalidtoken, CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	else
	{
		pnbuff.type=pn_while;
		pnbuff.tokens.push_back(index);
		pindex=ptree.append_child(oldpindex,pnbuff);
		ClearPNBuff();
		ReadToken();
	}
	ParseFExp();
	if(IsError()) return 0;
	ParseBlockNoDec();	
	if(IsError())	return 0;
	pindex=oldpindex;
	--whilecount;
	return 1;
}

int bcParser::ParseBreak()
{
	tree<bcParseNode>::iterator oldpindex=pindex;
		
	if(whilecount==0)
	{
		SetError(ec_par_invalidbreak,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	if(CurrentToken()->type!=tt_break)
	{
		SetError(ec_par_invalidtoken, CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	else
	{
		pnbuff.type=pn_break;
		pnbuff.tokens.push_back(index);
		ptree.append_child(oldpindex,pnbuff);
		ClearPNBuff();
		ReadToken(tt_scolon);
	}	
	if(IsError())	return 0;
	return 1;
}

int bcParser::ParseReturn()
{
	tree<bcParseNode>::iterator oldpindex=pindex;	
	
	if(funccount==0)
	{
		SetError(ec_par_invalidreturn,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	if(CurrentToken()->type!=tt_return)
	{
		SetError(ec_par_invalidtoken, CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	else
	{
		pnbuff.type=pn_return;
		pnbuff.tokens.push_back(index);
		pindex=ptree.append_child(oldpindex,pnbuff);
		ClearPNBuff();
		ReadToken();
	}
	ParseFExp();
	
	if(CurrentToken()->type!=tt_scolon)
	{
		SetError(ec_par_noscolon,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	ReadToken();
	pindex=oldpindex;
	if(IsError())	return 0;
	return 1;
}

int bcParser::ParseContinue()
{
	tree<bcParseNode>::iterator oldpindex=pindex;
		
	if(CurrentToken()->type!=tt_continue)
	{
		SetError(ec_par_invalidtoken, CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	else
	{
		pnbuff.type=pn_continue;
		pnbuff.tokens.push_back(index);
		ptree.append_child(oldpindex,pnbuff);
		ClearPNBuff();
		ReadToken(tt_scolon);
	}
	if(CurrentToken()->type!= tt_scolon)
	{
		//error
		return 0;
	}

	if(IsError())	return 0;
	return 1;
}

int bcParser::ParseIf()
{
	tree<bcParseNode>::iterator oldpindex=pindex;
		
	if(CurrentToken()->type!=tt_if)
	{
		SetError(ec_par_invalidtoken, CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	else
	{
		pnbuff.type=pn_if;
		pnbuff.tokens.push_back(index);
		pindex=ptree.append_child(oldpindex,pnbuff);
		ClearPNBuff();
		ReadToken();
	}
	
	if(NextToken()->type==tt_cparen)
	{
		//empty if expression
		SetError(ec_par_emptyexpression,CurrentToken()->line, CurrentToken()->col);
		return 0;
	}
	else
	{
		//++parencount;
		ParseFExp();
		if(IsError()) return 0;
		//--parencount;
	}

	ParseBlockNoDec();
	if(IsError()) return 0;
	
	if(CurrentToken()->type==tt_else)
	{
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_else;
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();
		ReadToken();
		ParseBlockNoDec();
	}
	
	pindex=oldpindex;
	if(IsError())	return 0;
	return 1;
}

int bcParser::ParseBlockNoDec()
{
	tree<bcParseNode>::iterator oldpindex=pindex;

	//requisite checks
	if(CurrentToken()->type!=tt_obrace)
	{
		//error
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	else
	{		
		pnbuff.type=pn_block;
		pindex=ptree.append_child(oldpindex,pnbuff);
		ClearPNBuff();
		ReadToken();
		//++bracecount;
	}

	while(CurrentToken()->type!=tt_eof)
	{
		if(CurrentToken()->type==tt_cbrace)
		{
			//++bracecount;
			ReadToken();
			pindex=oldpindex;
			return 1;
		}
		nodec=true;
		ParseStatement();
		nodec=false;
		if(IsError())
		{
			return 0;
		}				
	}

	//unexpected eof
	SetError(ec_par_eof,PreviousToken()->line,PreviousToken()->col);
	pindex=oldpindex;
	return 0;
}

int bcParser::ParseBlock()
{
	tree<bcParseNode>::iterator oldpindex=pindex;

	//requisite checks
	if(CurrentToken()->type!=tt_obrace)
	{
		//error
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	else
	{		
		pnbuff.type=pn_block;
		pindex=ptree.append_child(oldpindex,pnbuff);
		ClearPNBuff();
		ReadToken();
		//++bracecount;
	}

	while(CurrentToken()->type!=tt_eof)
	{
		if(CurrentToken()->type==tt_cbrace)
		{
			//++bracecount;
			ReadToken();
			pindex=oldpindex;
			return 1;
		}
		ParseStatement();
		if(IsError())
		{
			return 0;
		}						
	}

	//unexpected eof
	SetError(ec_par_eof,PreviousToken()->line,PreviousToken()->col);
	pindex=oldpindex;
	return 0;
}

//full expression
int bcParser::ParseFExp()
{
	tree<bcParseNode>::iterator oldpindex=pindex;
	pnbuff.type=pn_exp;	
	pindex=ptree.append_child(oldpindex,pnbuff);
	ClearPNBuff();
	
	ParseExp();

	//add self to the current pnode
	//expressions are only bccTokens (compound tokens), never a pnode
	pindex=oldpindex;
	
	return 1;
}

bcToken bcParser::ParseExp()
{
	bcToken out,operand1,operand2;
	bcTokenType op;	
	
	if(bcParser_CheckEmptyInput && in->size()<1)
	{
		//error
		out.type=tt_eof;
		return out;
	}

	operand1=ParseSubExp();

	if(IsError())
	{
		out.type=tt_eof;
		return out;
	}

	//handle relational or logical, return operand1 if not
	while(index<in->size())
	{
		op=CurrentToken()->type;		
		switch(op)
		{
			case tt_less:
				pnbuff.type=pn_less;
				break;

			case tt_greater:
				pnbuff.type=pn_greater;
				break;

			case tt_lessequal:
				pnbuff.type=pn_lessequal;
				break;

			case tt_greaterequal:
				pnbuff.type=pn_greaterequal;
				break;

			case tt_equal:
				pnbuff.type=pn_equal;
				break;
				
			case tt_notequal:
				pnbuff.type=pn_notequal;
				break;
				
			case tt_logand:
				pnbuff.type=pn_logand;
				break;

			case tt_logor:
				pnbuff.type=pn_logor;
				break;

			default:
				return operand1;
		}
		pnbuff.tokens.push_back(index);
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();

		ReadToken();
		operand2=ParseSubExp();
		if(IsError())	return out;
		
		//execute/check for type issues
		if(CheckOperandTypes(operand1,operand2,op)==false)
		{
			SetError(ec_par_illegaloperation,CurrentToken()->line,CurrentToken()->col);
			return out;
		}
	}

	//Check for parenthesis errors
	if(parencount>0 && CurrentToken()->type==tt_cparen)
	{
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		out.type=tt_eof;
		return out;
	}

	if(parencount!=0)
	{
		//one or more missing closing parenthesis
		SetError(ec_par_nocparen,CurrentToken()->line,CurrentToken()->col);
		out.type=tt_eof;
		return out;
	}

	return out;
}

bcToken bcParser::ParseSubExp()
{
	bcToken out;

	if(bcParser_CheckEmptyInput && in->size()<1)
	{
		//error
		out.type=tt_eof;
		return out;
	}

	//int parencount=0;
	bcToken operand1,operand2;
	bcTokenType op;
	
	//get the first token
	operand1=ParseTerm();
	if(IsError())	return out;

	while(index<in->size())
	{
		//get operator +,-,
		op=CurrentToken()->type;	
		switch(op)
		{
		case tt_plus:
			pnbuff.type=pn_plus;
			break;
			
		case tt_minus:
			pnbuff.type=pn_minus;
			break;

		default:
			return operand1;
		}

		pnbuff.tokens.push_back(index);
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();
		ReadToken();
		operand2=ParseTerm();
		if(IsError())	return out;
		
		//execute/check for type issues
		if(CheckOperandTypes(operand1,operand2,op)==false)
		{
			SetError(ec_par_illegaloperation,CurrentToken()->line,CurrentToken()->col);
			return out;
		}
		//operand1.data=bcitos(bcstoi(operand1.data)+bcstoi(operand2.data));		
	}
	
	return operand1;
}

bcToken bcParser::ParseTerm()
{
	bcToken operand1,operand2;
	bcTokenType op;
	bcToken out;	

	if(bcParser_CheckEmptyInput && in->size()<1)
	{
		//error
		return out;
	}

	//get the first token
	operand1=ParseFactor();
	if(IsError())	return out;
	
	while(index<in->size())
	{		
		op=CurrentToken()->type;

		switch(op)
		{
			case tt_div:
				pnbuff.type=pn_mult;
				break;
			case tt_mult:
				pnbuff.type=pn_mult;
				break;
			case tt_mod:
				pnbuff.type=pn_mod;
				break;
			case tt_pow:
				pnbuff.type=pn_pow;
				break;
			default:
				return operand1;
		}	
		pnbuff.tokens.push_back(index);
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();

		ReadToken();
		operand2=ParseFactor();
		if(IsError())	return out;
		
		//execute/check for type issues
		if(!CheckOperandTypes(operand1,operand2,op))
		{
			SetError(ec_par_illegaloperation,CurrentToken()->line,CurrentToken()->col);
			return out;
		}
		//operand1.data=bcitos(bcstoi(operand1.data)*bcstoi(operand2.data));
	}	

	return operand1;
}

bcToken bcParser::ParseFactor()
{
	tree<bcParseNode>::iterator oldpindex=pindex;
	bcToken out;	
	bool negative=false;
	bool positive=false;

	if(bcParser_CheckEmptyInput && in->size()<1)
	{
		//error
		return out;
	}
	
	//Must check for eof, as index is incremented in ParseTerm()
	if(index>in->size()-1)
	{
		SetError(ec_par_eof,PreviousToken()->line,PreviousToken()->col);
		return out;
	}
		
	//minus or plus unary operators
	if(CurrentToken()->type==tt_minus)
	{
		negative=true;
		//changes made to lexer.data, pnode.links gets boken if lexer.data changes 
		//in->erase(in->begin()+index,in->begin()+index+1);
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_minus;
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();
		ReadToken();	//tok=CurrentToken();
	}
	else 
	{
		if(CurrentToken()->type==tt_plus)
		{
			//changes made to lexer.data, pnode.links gets boken if lexer.data changes 
			//in->erase(in->begin()+index,in->begin()+index+1);
			pnbuff.tokens.push_back(index);
			pnbuff.type=pn_plus;
			ptree.append_child(pindex,pnbuff);
			ClearPNBuff();
			ReadToken();	//tok=CurrentToken();
			positive=true;
		}
		
	}

	switch(CurrentToken()->type)
	{
	case tt_intlit:
		//neg the var todo: do this in lexer
		if(negative)
		{
			CurrentToken()->data=bcitos(0-(bcstoi(CurrentToken()->data)));
		}
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_intlit;
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();
		ReadToken();
		return *PreviousToken();
		

	case tt_strlit:
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_strlit;
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();
		ReadToken();
		return *PreviousToken();
		
	case tt_fltlit:
		if(negative)
		{
			CurrentToken()->data=bcftos(0-(bcstof(CurrentToken()->data)));
		}
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_fltlit;
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();
		ReadToken();
		return *PreviousToken();

	case tt_true:
		if(negative||positive)
		{
			//cant negate true/false
			SetError(ec_par_invalidop,PreviousToken()->line,PreviousToken()->col);
			return out;
		}
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_true;
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();
		
		ReadToken();
		return *PreviousToken();

	case tt_false:
		if(negative||positive)
		{
			//cant negate true/false
			SetError(ec_par_invalidop,PreviousToken()->line,PreviousToken()->col);
			return out;
		}
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_false;
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();
		
		ReadToken();
		return *PreviousToken();
		
	case tt_ident:
		//Check symbol exists, and that its type is permitted in an exp (variable, function call)
		if(GetSymbol(CurrentToken()->data)!=NULL)
		{
			switch(GetSymbol(CurrentToken()->data)->type)
			{
			case st_function:
				if(!IsScopeValid(GetSymbol(CurrentToken()->data)))
				{
					//error, identifier not valid in this scope
					SetError(ec_par_identinvalidscope,CurrentToken()->line,CurrentToken()->col);
					return out;
				}
				//tok=CurrentToken();
				ParseFuncCall();
				if(IsError())	return out;
				return *CurrentToken();
				break;

			case st_var:
				if(!IsScopeValid(GetSymbol(CurrentToken()->data)))
				{
					//error, identifier not valid in this scope
					SetError(ec_par_identinvalidscope,CurrentToken()->line,CurrentToken()->col);
					return out;
				}
				pnbuff.tokens.push_back(index);
				pnbuff.type=pn_var;
				if(GetSymbolIt(CurrentToken()->data)->second.scope->first=="$global")
				{
					pnbuff.tag=CurrentToken()->data;
				}
				else
				{
					pnbuff.tag=GetSymbolIt(CurrentToken()->data)->second.scope->first+"."+CurrentToken()->data;
				}
				ptree.append_child(pindex,pnbuff);
				ClearPNBuff();
				ReadToken();
				return *PreviousToken();

			case st_object:
				if(IsScopeValid(GetSymbol(CurrentToken()->data)))
				{
					//error, identifier not valid in this scope
					SetError(ec_par_identinvalidscope,CurrentToken()->line,CurrentToken()->col);
					return out;
				}
				pnbuff.tokens.push_back(index);
				pnbuff.type=pn_object;
				ptree.append_child(pindex,pnbuff);
				ClearPNBuff();
				ReadToken();
				return *PreviousToken();

			default:
				SetError(ec_par_invalidident,CurrentToken()->line,CurrentToken()->col);
				return out;
			}
		}
		else
		{
			SetError(ec_par_undeclaredident,CurrentToken()->line,CurrentToken()->col);
			return out;
		}
		
		//ReadToken();
		return *PreviousToken();

	case tt_oparen:
		++parencount;
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_oparen;
		ptree.append_child(pindex,pnbuff);
		ClearPNBuff();
		ReadToken();
		out=ParseExp();
		if(IsError())
		{
			return out;
		}
		
		if(CurrentToken()->type!=tt_cparen)
		{
			//error no closing parenthesis 
			SetError(ec_par_nocparen,CurrentToken()->line,CurrentToken()->col);
			return out;
		}
		else
		{
			pnbuff.tokens.push_back(index);
			pnbuff.type=pn_cparen;
			ptree.append_child(pindex,pnbuff);
			ClearPNBuff();
			ReadToken();
			--parencount;
			return out;
		}
		break;

	case tt_cparen:
		//error?
		SetError(ec_par_invalidcparen,CurrentToken()->line,CurrentToken()->col);
		return out;
		break;

	default:
		//error
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return out;
		break;
	}
	

	return out;
}

//do a simple type to type check for compatability with each other.
//TODO add tt_assign as an operater
bool bcParser::CheckOperandTypes(bcToken operand1, bcToken operand2, bcTokenType op)
{
	switch(operand1.type)
	{
	
	case tt_intlit:
		return CheckOperandInt(operand2,op);
		break;

	case tt_strlit:
		return CheckOperandString(operand2,op);
		break;

	case tt_fltlit:
		return CheckOperandFloat(operand2,op);
		break;

	case tt_true:
	case tt_false:
		return CheckOperandBool(operand2,op);
		break;

	case tt_ident:
		switch(GetSymbol(operand1.data)->type)
		{
		case st_var:			
				if(GetSymbol(operand1.data)->datatype=="int")
				{
					return CheckOperandInt(operand2,op);	
				}
				if(GetSymbol(operand1.data)->datatype=="float")
				{
					return CheckOperandFloat(operand2,op);	
				}
				if(GetSymbol(operand1.data)->datatype=="string")
				{
					return CheckOperandString(operand2,op);	
				}
				if(GetSymbol(operand1.data)->datatype=="bool")
				{
					return CheckOperandBool(operand2,op);	
				}
				//non built in type

			break;

		case st_function:			
				if(GetSymbol(operand1.data)->datatype=="int")
				{
					return CheckOperandInt(operand2,op);	
				}
				if(GetSymbol(operand1.data)->datatype=="float")
				{
					return CheckOperandFloat(operand2,op);	
				}
				if(GetSymbol(operand1.data)->datatype=="string")
				{
					return CheckOperandString(operand2,op);	
				}
				if(GetSymbol(operand1.data)->datatype=="bool")
				{
					return CheckOperandBool(operand2,op);	
				}	
				//non built in type
				break;
		}
		return false;
		break;

	//Dynamic objects
	case st_object:
		return true;
		break;

	//Error
	default:
		//SetError(ec_par_invalidtoken,operand1.line,operand1.col);
		return false;
		
	}
}

bool bcParser::CheckOperandBool(bcToken operand2,bcTokenType op)
{
	switch(op)
	{
	case tt_plus:
	case tt_minus:
	case tt_div:
	case tt_mult:
	case tt_mod:
	case tt_pow:
		return false;
		break;

	case tt_assign:
		
		break;

	case tt_greater:
	case tt_less:
	case tt_greaterequal:
	case tt_lessequal:
		return false;
		break;

	case tt_equal:
	case tt_notequal:
	case tt_logand:
	case tt_logor:
	case tt_lognot:
		break;
		
	//only logical and relational ops, and assignment
	default:
		return false;
		break;
	}

	switch(operand2.type)
	{
	case tt_true:
	case tt_false:
		return true;
	
	case tt_intlit:
		return true;
		break;

	case tt_strlit:
		return false;
		break;

	case tt_fltlit:
		return true;
		break;

	case tt_ident:				
		//funcs and vars have the same 
		if(GetSymbol(operand2.data)->type==st_object)
		{
			return true;
		}
		else if(GetSymbol(operand2.data)->datatype=="int")
		{
			return true;
		}
		else if(GetSymbol(operand2.data)->datatype=="float")
		{
			return true;
		}
		else if(GetSymbol(operand2.data)->datatype=="string")
		{
			return false;
		}
		else if(GetSymbol(operand2.data)->datatype=="bool")
		{
			switch(op)
			{
			case tt_mult:
			case tt_div:
			case tt_plus:
			case tt_minus:	
			case tt_mod:
			case tt_pow:
				return false;
				break;

			default:
				return true;
			}
					
		}

			
		return false;
		break;

	//Error
	default:
		//SetError(ec_par_invalidtoken,operand1.line,operand1.col);
		return false;
		
	}

}

bool bcParser::CheckOperandInt(bcToken operand2,bcTokenType op)
{
	switch(op)
	{
	case tt_plus:
	case tt_minus:
	case tt_div:
	case tt_mult:
	case tt_mod:
	case tt_pow:
		
		break;

	case tt_assign:
		
		break;

	case tt_greater:
	case tt_less:
	case tt_greaterequal:
	case tt_lessequal:
		
		break;

	case tt_equal:
	case tt_notequal:
	case tt_logand:
	case tt_logor:
	case tt_lognot:

		break;
		
	//only logical and relational ops, and assignment
	default:
		return false;
		break;
	}


	switch(operand2.type)
	{
	case tt_true:
	case tt_false:
		switch(op)
		{
		case tt_mult:
		case tt_div:
		case tt_plus:
		case tt_minus:	
		case tt_mod:
		case tt_pow:
			return false;
			break;

		default:
			break;
		}
		break;
	
	case tt_intlit:
		return true;
		break;

	case tt_strlit:
		return false;
		break;

	case tt_fltlit:
		return true;
		break;

	case tt_ident:				
		//funcs and vars have the same 
		if(GetSymbol(operand2.data)->type==st_object)
		{
			return true;
		}
		else if(GetSymbol(operand2.data)->datatype=="int")
		{
			return true;
		}
		else if(GetSymbol(operand2.data)->datatype=="float")
		{
			return true;
		}
		else if(GetSymbol(operand2.data)->datatype=="string")
		{
			return false;
		}
		else if(GetSymbol(operand2.data)->datatype=="bool")
		{
			switch(op)
			{
			case tt_mult:
			case tt_div:
			case tt_plus:
			case tt_minus:	
			case tt_mod:
			case tt_pow:
				return false;
				break;

			default:
				return true;
			}
					
		}

			
		return false;
		break;

	//Error
	default:
		//SetError(ec_par_invalidtoken,operand1.line,operand1.col);
		return false;
		
	}


	return true;
}

bool bcParser::CheckOperandString(bcToken operand2,bcTokenType op)
{
	switch(op)
	{
	case tt_plus:
		break;

	case tt_minus:
	case tt_div:
	case tt_mult:
	case tt_mod:
	case tt_pow:
		return false;
		break;

	case tt_assign:		
		break;

	case tt_greater:
	case tt_less:
	case tt_greaterequal:
	case tt_lessequal:		
		break;

	case tt_equal:
	case tt_notequal:
	case tt_logand:
	case tt_logor:
	case tt_lognot:
		break;
		
	//only logical and relational ops, and assignment
	default:
		return false;
		break;
	}


	switch(operand2.type)
	{
	case tt_true:
	case tt_false:
		return false;
		break;
	
	case tt_intlit:
		if(op==tt_plus)
		{
			return true;
		}
		else
		{
			return false;
		}
		break;

	case tt_strlit:
		return true;
		break;

	case tt_fltlit:
		if(op==tt_plus)
		{
			return true;
		}
		else
		{
			return false;
		}
		break;

	case tt_ident:				
		//funcs and vars have the same 
		if(GetSymbol(operand2.data)->type==st_object)
		{
			return true;
		}
		else if(GetSymbol(operand2.data)->datatype=="int")
		{
			if(op==tt_plus)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if(GetSymbol(operand2.data)->datatype=="float")
		{			
			if(op==tt_plus)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if(GetSymbol(operand2.data)->datatype=="string")
		{
			return true;
		}
		else if(GetSymbol(operand2.data)->datatype=="bool")
		{
			return false;					
		}			
		return false;
		break;

	//Error
	default:
		//SetError(ec_par_invalidtoken,operand1.line,operand1.col);
		return false;
		
	}
	return true;
}

bool bcParser::CheckOperandFloat(bcToken operand2,bcTokenType op)
{
	switch(op)
	{
	case tt_plus:
	case tt_minus:
	case tt_div:
	case tt_mult:
	case tt_mod:
	case tt_pow:
		
		break;

	case tt_assign:		
		break;

	case tt_greater:
	case tt_less:
	case tt_greaterequal:
	case tt_lessequal:		
		break;

	case tt_equal:
	case tt_notequal:
	case tt_logand:
	case tt_logor:
	case tt_lognot:
		break;
		
	//only logical and relational ops, and assignment
	default:
		return false;
		break;
	}


	switch(operand2.type)
	{
	case tt_true:
	case tt_false:
		return false;
		break;
	
	case tt_intlit:
		return true;
		break;

	case tt_strlit:
		return false;
		break;

	case tt_fltlit:
		return true;
		break;

	case tt_ident:				
		//funcs and vars have the same 
		if(GetSymbol(operand2.data)->type==st_object)
		{
			return true;
		}
		else if(GetSymbol(operand2.data)->datatype=="int")
		{
			return true;
		}
		else if(GetSymbol(operand2.data)->datatype=="float")
		{			
			return true;
		}
		else if(GetSymbol(operand2.data)->datatype=="string")
		{
			return false;
		}
		else if(GetSymbol(operand2.data)->datatype=="bool")
		{
			return false;					
		}			
		return false;
		break;

	//Error
	default:
		//SetError(ec_par_invalidtoken,operand1.line,operand1.col);
		return false;
		
	}
	return true;
}

int bcParser::ParseDecInParam()
{
	tree<bcParseNode>::iterator oldpindex=pindex;
	bcSymbol symbuff;
	int identpos=0;
	int typepos=0;
	int typeisobject=0;
	//symtypebuff=//

	switch(CurrentToken()->type)
	{
	//requisite checks
	case tt_function:
		//error
		return 0;
		break;
	

	case tt_ident:
		if(GetSymbol(CurrentToken()->data)!=NULL)
		{
			switch(GetSymbol(CurrentToken()->data)->type)
			{
			case st_object:
			case st_type:
				//make note if type is object
				typeisobject=1;
				//pn_add parsenode dec to the tree
				pnbuff.type=pn_paramdeclaration;
				pindex=ptree.append_child(oldpindex,pnbuff);
				ClearPNBuff();
				//add the type to the pn_dec node
				pnbuff.tokens.push_back(index);
				pnbuff.type=pn_type;
				ptree.append_child(pindex,pnbuff);
				
				//Clear buffs, take note of this types token index
				ClearPNBuff();
				typepos=index;
				ReadToken();
				break;

			default:
				//can only be a type/object
				SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
				return 0;
				break;
			}
		}
		else
		{
			//undeclared symbol
			SetError(ec_par_undeclaredident, CurrentToken()->line, CurrentToken()->col);
			return 0;
		}
		break;

	//built in types
	case tt_int:
	case tt_float:
	case tt_string:
	case tt_bool:
	case tt_var:
		//pn_add parsenode dec to the tree
		pnbuff.type=pn_paramdeclaration;
		pindex=ptree.append_child(oldpindex,pnbuff);
		ClearPNBuff();
		//add the type to the pn_dec node
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_type;
		ptree.append_child(pindex,pnbuff);
		
		//Clear buffs, take note of this types token index
		ClearPNBuff();
		typepos=index;
		ReadToken();
		break;
	
	default:	
		//error, ParseDec shouldn't be called unless dec was found
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
		break;
		
	}
	
	//check for identifier 
	if(CurrentToken()->type!=tt_ident)
	{
		//error expected token: expected indentifier
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	else
	{
		//add ident to pn_dec node
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_ident;
		ptree.append_child(pindex,pnbuff);
		//clear buffs, and take note of idents position
		ClearPNBuff();
		identpos=index;
		ReadToken();
	}

	//add ident to symbol table
	symbuff.ident=in->at(identpos).data;
	symbuff.type=st_var;
	symbuff.datatype=in->at(typepos).data;
	//symbuff.scope=scope;
	//symbuff.stackindex=stackframebuff->size();	//this will be true after we add the symbuff
	//stackframebuff->push_back(AddSymbol( symbuff ));
	
	//add to the parameter buff, used to later add them with correct scope (this functions ident is currently unregistered
	//as we need the parameters to generate a functions internal ident correctly format being, scope.ident#type,type etc
	//though currently functions can only be declared in global they dont have a scope prefix.
	parambuff.push_back(symbuff);
	//add type to function signature buffer
	sigbuff.push_back(GetSymbolIt(in->at(typepos).data));
	
	//ReadToken();
	pindex=oldpindex;	//Reset our parse tree index back to where it was before this dec
	return 1;
		
}

string bcParser::GetTag(bcSymbol* sym)
{
	if(sym->scope->second.ident=="$global")
	{
		return sym->ident;
	}
	else
	{
		return sym->scope->second.ident+"."+sym->ident;
	}
	
}

int bcParser::ParseDec()
{
	tree<bcParseNode>::iterator oldpindex=pindex;
	tree<bcParseNode>::iterator identnode;
	bcToken lctokbuff;
	
	bcToken tokbuff;
	bcSymbol symbuff;
	int identpos=0;
	int typepos=0;
	int typeisobject=0;
	map<string,bcSymbol>::iterator oldscope=scope;


	switch(CurrentToken()->type)
	{
	//requisite checks
	case tt_function:
		return ParseDecFunc();				
		break;
	

	case tt_ident:
		if(GetSymbol(CurrentToken()->data)!=NULL)
		{
			switch(GetSymbol(CurrentToken()->data)->type)
			{
			case st_object:
			case st_type:
				//make note if type is object
				typeisobject=1;
				//pn_add parsenode dec to the tree
				pnbuff.type=pn_declaration;
				pindex=ptree.append_child(oldpindex,pnbuff);
				ClearPNBuff();
				//add the type to the pn_dec node
				pnbuff.tokens.push_back(index);
				pnbuff.type=pn_type;
				ptree.append_child(pindex,pnbuff);
				//Clear buffs, take note of this types token index
				ClearPNBuff();
				typepos=index;
				lctokbuff.type=CurrentToken()->type;
				ReadToken();
				break;

			default:
				//can only be a type/object
				SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
				return 0;
				break;
			}
		}
		else
		{
			//undeclared symbol
			SetError(ec_par_undeclaredident, CurrentToken()->line, CurrentToken()->col);
			return 0;
		}
		break;

	//built in types
	case tt_int:
	case tt_float:
	case tt_string:
	case tt_bool:
	case tt_var:
		//pn_add parsenode dec to the tree
		pnbuff.type=pn_declaration;
		pindex=ptree.append_child(oldpindex,pnbuff);
		ClearPNBuff();
		//add the type to the pn_dec node
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_type;
		ptree.append_child(pindex,pnbuff);
		//Clear buffs, take note of this types token index
		ClearPNBuff();
		typepos=index;
		lctokbuff.type=CurrentToken()->type;
		ReadToken();
		break;
	
	default:	
		//error, ParseDec shouldn't be called unless dec was found
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
		break;
		
	}
	
	//check for identifier 
	if(CurrentToken()->type!=tt_ident)
	{
		//error expected token: expected indentifier
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	else
	{
		//add ident to pn_dec node
		pnbuff.tokens.push_back(index);
		pnbuff.type=pn_ident;
		identnode=ptree.append_child(pindex,pnbuff);
		//clear buffs, and take note of idents position
		ClearPNBuff();
		identpos=index;
		lctokbuff.data=CurrentToken()->data;
		ReadToken();
	}

	switch(CurrentToken()->type)
	{
	//type ident = (expression) ;
	case tt_assign:
		ReadToken();
		pnbuff.type=pn_exp;
		pindex=ptree.append_child(pindex,pnbuff);
		ClearPNBuff();
		
		tokbuff=ParseExp();			
		if(IsError())	return 0;

		if(CurrentToken()->type==tt_scolon)
		{
			//add ident to symbol table			
			symbuff.ident=in->at(identpos).data;
			symbuff.type=st_var;
			symbuff.datatype=in->at(typepos).data;
			symbuff.scope=scope;
			symbuff.stackindex=stackframebuff->size()+1;
			identnode->tag=GetTag(&symbuff);
			stackframebuff->push_back(AddSymbol(symbuff));			

			//check we can initialize with this data type
			if(!CheckOperandTypes(in->at(identpos),tokbuff,tt_assign))
			{
				SetError(ec_par_invalidop,tokbuff.line,tokbuff.col);
				return 0;
			}
			
			ReadToken();
			pindex=oldpindex;	//Reset our parse tree index back to where it was before this dec
			return 1;
		}
		else
		{
			SetError(ec_par_noscolon,CurrentToken()->line,CurrentToken()->col);			
			return 0;
		}
		break;

	//type ident;       uninitialised var declared.
	case tt_scolon:
		//add ident to symbol table
		symbuff.ident=in->at(identpos).data;
		symbuff.type=st_var;
		symbuff.datatype=in->at(typepos).data;
		symbuff.scope=scope;
		symbuff.stackindex=stackframebuff->size()+1;				
		identnode->tag=GetTag(&symbuff);
		stackframebuff->push_back(AddSymbol(symbuff));

		ReadToken();
		pindex=oldpindex;	//Reset our parse tree index back to where it was before this dec
		return 1;
		break;

	default:
		//error invalid token
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
		break;
	}

	return 1;
}

int bcParser::ParseDecFunc_Type()
{
	//return type,		
	pnbuff.tokens.push_back(index);
	pnbuff.type=pn_type;
	ptree.append_child(pindex,pnbuff);
	ClearPNBuff();	
	ReadToken();						
	return 1;	
}

int bcParser::ParseDecFunc_Ident()
{
	//check if it already exists
	if(GetSymbol(CurrentToken()->data)!=NULL)
	{
		//variable
		switch(GetSymbol(CurrentToken()->data)->type)
		{
		case st_var:
		case st_object:
		case st_type:
		case st_function:
			SetError(ec_par_redeclaredident,CurrentToken()->line,CurrentToken()->col);
			return 0;
			break;		

		default:
			//Symbol with invalid type
			SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
			return 0;
			break;
		}
	}

	//symbol doesnt exist	
	//make note of iternal ident name
	pnbuff.tag=CurrentToken()->data;
	pnbuff.tokens.push_back(index);
	pnbuff.type=pn_ident;
	ptree.append_child(pindex,pnbuff);
	ClearPNBuff();
	ReadToken();

	return 1;
}


//Creates an entry in the symbol table, creates and populates a stackframe, changes scope to the current function
//and parses the inner block
int bcParser::ParseDecFunc()
{
	tree<bcParseNode>::iterator oldpindex=pindex;	
	bcToken tokbuff;
	
	int typepos=0;
	int identpos=0;
	bcSymbol symbuff;
	map<string,bcSymbol>::iterator oldscope=scope;
	vector<map<string,bcSymbol>::iterator> stackframe;
	vector<map<string,bcSymbol>::iterator>* oldframe=stackframebuff;
	stackframebuff=&stackframe;

	
	//= = =requisite checks= = =
	if(CurrentToken()->type==tt_function)
	{	
		//set parsenode type to declaration
		pnbuff.type=pn_funcdec;
		pindex=ptree.append_child(oldpindex,pnbuff);
		ClearPNBuff();
		typepos=index;
		ReadToken();
	}
	else
	{
		//error, ParseDec shouldn't be called unless dec was found
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}
	
	

	//= = =type identifier= = = 
	//
	typepos=index;	//note the type
	switch(CurrentToken()->type)
	{
	case tt_int:
	case tt_float:
	case tt_string:
	case tt_bool:
	case tt_object:
	case tt_var:		
		break;
	
	default:	
		//error expected token: expected indentifier
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
		break;	
	}	
	ParseDecFunc_Type();
	if(IsError())	return 0;

	//= = =Function identifier= = = 
	identpos=index;	//note the identifier
	if(CurrentToken()->type!=tt_ident)
	{
		//error expected token: expected indentifier
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}			
	ParseDecFunc_Ident();
	if(IsError())	return 0;

	//= = =Parameters List= = =
	ParseDecParamList();	
	if(IsError())	return 0;

	symbuff.ident=in->at(identpos).data;
	symbuff.type=st_function;
	symbuff.scope=scope;
	symbuff.datatype=in->at(typepos).data;
	string internident=SigToIdent(symbuff.ident,sigbuff);
	scope=AddSymbol(symbuff);
	openscopes.push(scope);	//add to scopes
	//add sigbuff to funcsigs, sigbuff was populated, if at all, in ParseDecParamList()->ParseDecInParam()	
	//paired with the interident (normally just the method name)
	funcsigs.insert( make_pair( internident,sigbuff ) );
	//adjust the symbol tables entry to include signature in iternal ident if any
	//scope->second.ident=internident;
	sigbuff.clear();

	//add the parameter symbols
	for(int t=0;t<parambuff.size();++t)
	{
		parambuff.at(t).scope=scope;
		parambuff.at(t).stackindex=stackframebuff->size();	//this will be true after we push_back the symbuff
		stackframebuff->push_back(AddSymbol( parambuff.at(t) ));
	}
	parambuff.clear();

	if(CurrentToken()->type==tt_scolon)
	{
		ReadToken();
	}
	else
	{
		ParseBlock();	
		if(IsError())	return 0;
	}
	
	
	//exit and restore previous scope etc
	pindex=oldpindex;
	--funccount;
	scope=oldscope;
	stackframes.insert( make_pair( internident,stackframe) );
	stackframebuff=oldframe;
	openscopes.pop();
	return 1;
}

//fills up callsigbuff, so remember to clear it!
int bcParser::ParseParamList()
{
	tree<bcParseNode>::iterator beforeexp;
	tree<bcParseNode>::iterator oldpindex=pindex;
	bcToken tokbuff;
	int params=0;	

	pnbuff.type=pn_paramlist;
	pindex=ptree.append_child(oldpindex,pnbuff);
	ClearPNBuff();
	bcToken* tok = CurrentToken();
	if(CurrentToken()->type==tt_oparen)
	{
		ReadToken();
	}
	else
	{
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}		
	
	//loop through (expression>tt_comma) until we find a cparen
	while(CurrentToken()->type!=tt_cparen)
	{
		//add expression node
		beforeexp=pindex;
		pnbuff.type=pn_exp;
		pindex=ptree.append_child(pindex,pnbuff);
		ClearPNBuff();
		tokbuff=ParseExp();
		pindex=beforeexp;	
		if(IsError()) return 0;

		//add to the callsigbuff
		callsigbuff.push_back(GetSymbolIt(tokbuff));
		
		switch(CurrentToken()->type)
		{
			case tt_comma:
			ReadToken();
			break;

			case tt_cparen:
				if(PreviousToken()->type==tt_comma)
				{
					SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
					return 0;
				}
				else
				{
					pindex=oldpindex;
					ReadToken();
					return 1;
				}
			break;
		}		
	}
	pindex=oldpindex;	
	ReadToken();// Let the caller parse the semicolon
	return 1;
}

int bcParser::ParseDecParamList()
{
	tree<bcParseNode>::iterator oldpindex=pindex;
	int params=0;	
	parambuff.clear();

	//parent node
	pnbuff.type=pn_decparamlist;
	pindex=ptree.append_child(oldpindex,pnbuff);
	ClearPNBuff();

	if(CurrentToken()->type==tt_oparen)
	{
		ReadToken();
	}
	else
	{
		SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
		return 0;
	}		
		
	while(CurrentToken()->type!=tt_cparen)
	{
		ParseDecInParam();
		if(IsError()) return 0;
		++params;		
		
		switch(CurrentToken()->type)
		{
			case tt_comma:
			ReadToken();
			break;

			case tt_cparen:
				if(PreviousToken()->type==tt_comma)
				{
					SetError(ec_par_invalidtoken,CurrentToken()->line,CurrentToken()->col);
					return 0;
				}
				else
				{
					pindex=oldpindex;
					ReadToken();
					return 1;
				}
			break;
		}
	}

	pindex=oldpindex;	
	ReadToken();
	return 1;
}

//UNUSED ATM-NO OVERLOADED METHODS SUPPORTED
//generates an iternal ident used as the key in symtab    i.e foo() = "foo"   but  foo(int,flt)="foo#int,float"
//ident=function name   types=vector of entries in the symtab
string bcParser::SigToIdent(string ident,vector<map<string,bcSymbol>::iterator> types)
{
	map<string,bcSymbol>::iterator it;
	string out="";
	
	if(types.size()==0)
	{
		return ident;
	}
	
	return ident; //comment me
	
	out=ident+"#";
	for(int t=0;t<types.size();++t)
	{
		it=types.at(t);
		out.append(it->second.ident);
		out.append(",");
	}
	out.erase(out.end()-1);
	return out;
}

bcSymbol* bcParser::GetSymbol(string ident)
{
	map<string,bcSymbol>::iterator it;
	stack<map<string,bcSymbol>::iterator> scopebuff;
	while(openscopes.size()!=0)
	{
		if(openscopes.top()->second.ident=="$global")
		{
			//search with /no/ prefix
			it=symtab.find(ident);		
		}
		else
		{
			//search with scope. prefix
			it=symtab.find(openscopes.top()->second.ident+"."+ident);		
		}
		if (it != symtab.end())	break;

		scopebuff.push(openscopes.top());
		openscopes.pop();
	}
	
	if(openscopes.size()==0)
	{
		//didnt find a match
		while(scopebuff.size()!=0)
		{
			openscopes.push(scopebuff.top());
			scopebuff.pop();
		}
		return NULL;
	}
	else
	{
		//found a matching symbol
		while(scopebuff.size()!=0)
		{
			openscopes.push(scopebuff.top());
			scopebuff.pop();
		}
		return &it->second;
	}	
}

map<string,bcSymbol>::iterator bcParser::GetSymbolIt(string ident)
{
	map<string,bcSymbol>::iterator it;
	stack<map<string,bcSymbol>::iterator> scopebuff;
	while(openscopes.size()!=0)
	{
		if(openscopes.top()->second.ident=="$global")
		{
			//search with no prefix
			it=symtab.find(ident);		
		}
		else
		{
			//search with scope. prefix
			it=symtab.find(openscopes.top()->second.ident+"."+ident);		

		}
		if(it != symtab.end())
		{
			break;
		}
		scopebuff.push(openscopes.top());
		openscopes.pop();
	}
	
	if(openscopes.size()==0)
	{
		//didnt find a match
		while(scopebuff.size()!=0)
		{
			openscopes.push(scopebuff.top());
			scopebuff.pop();
		}
		return symtab.end();
	}
	else
	{
		//found a matching symbol
		while(scopebuff.size()!=0)
		{
			openscopes.push(scopebuff.top());
			scopebuff.pop();
		}
		return it;
	}
}

map<string,bcSymbol>::iterator bcParser::GetSymbolIt(bcToken _in)
{
	switch(_in.type)
	{
	case tt_intlit:
		return symtab.find("int");

		case tt_strlit:
		return symtab.find("string");

		case tt_fltlit:
		return symtab.find("float");

		case tt_false:
		case tt_true:
		return symtab.find("bool");

		case tt_object:
		return symtab.find("object");

		case tt_ident:
			return GetSymbolIt(_in.data);
			
		default:
			//error
			return symtab.end();
	}
}

//standard add symbol
map<string,bcSymbol>::iterator bcParser::AddSymbol(bcSymbol symin)
{
	if(scope->first=="$global")
	{
		return symtab.insert( symtab.end(), make_pair( symin.ident, symin) );	
	}
	else
	{
		return symtab.insert( symtab.end(), make_pair( symin.scope->first+"."+symin.ident, symin) );	
	}
}

//specify first, used by overloaded funcs ala AddSymbol( symbuff, SigToIdent() )
map<string,bcSymbol>::iterator bcParser::AddSymbol(bcSymbol symin, string first)
{
	if(scope->second.ident=="$global")
	{
		return symtab.insert( symtab.end(), make_pair( first, symin) );	
	}
	else
	{
		return symtab.insert( symtab.end(), make_pair( first, symin) );	
	}
}

map<string,bcSymbol>::iterator bcParser::AddSymbol(string identin, bcSymbolType typein, string datatypein, map<string,bcSymbol>::iterator scopein)
{
	bcSymbol symbuff;
	symbuff.ident=identin;
	symbuff.type=typein;
	symbuff.datatype=datatypein;
	symbuff.scope=scopein;
	if(scope->second.ident=="$global")
	{
		return symtab.insert( symtab.end(), make_pair( symbuff.ident, symbuff) );	
	}
	else
	{
		return symtab.insert( symtab.end(), make_pair( symbuff.scope->second.ident+"."+symbuff.ident, symbuff) );	
	}
}

bool bcParser::IsError()
{
	return error;
}

bcErrorCode bcParser::GetError()
{
	return errorcode;
}

int bcParser::GetErrorLine()
{
	return error_line;
}

int bcParser::GetErrorCol()
{
	return error_col;
}

void bcParser::SetError(bcErrorCode in,int line, int col)
{
	error_line=line;
	error_col=col;
	error=true;
	errorcode = in;
}

void bcParser::ResetError()
{
	error=false;
	errorcode=ec_null;
}

bool bcParser::IsScopeValid(bcSymbol* _in)
{
	//global
	if(_in->scope->second.ident=="$global")
	{
		return true;
	}
	//same as current scope
	if(_in->scope->second.ident==scope->second.ident)
	{
		return true;
	}

	stack<map<string,bcSymbol>::iterator> scopebuff;	
	//check the top of openscopes with input, pop to scopebuff if false
	//if true, loop pushing top of scopebuff back openscopes, return true
	//if openscopes is empty and still no match, repeat scopebuff>openscopes return false
	while(openscopes.size()!=0)
	{
		if(openscopes.top()->second.ident==_in->scope->second.ident)
		{
			while(scopebuff.size()!=0)
			{
				openscopes.push(scopebuff.top());
				scopebuff.pop();
			}
			return true;
		}
		else
		{
			scopebuff.push(openscopes.top());
			openscopes.pop();
		}
	}
	
	//no match, reassemble openscopes, return false
	while(scopebuff.size()!=0)
	{
		openscopes.push(scopebuff.top());
		scopebuff.pop();
	}
	return false;
}