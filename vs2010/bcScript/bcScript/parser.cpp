#include "parser.h"

using namespace bc;
using namespace parser;

bcParseNodeType parser::DeriveType(bcToken in)
{	
	switch(in.type)
	{	
		case tt_intlit:		
			return pn_intlit;			
		case tt_period:
			return pn_period;
		case tt_comma:
			return pn_comma;
		case tt_plus:
			return pn_plus;
		case tt_minus:
			return pn_minus;
		case tt_div:
			return pn_div;
		case tt_mult:	
			return pn_mult;
		case tt_mod:
			return pn_mod;
		case tt_pow:
			return pn_pow;
		case tt_squote:
			return pn_squote;
		case tt_dquote:
			return pn_dquote;
		case tt_assign:	
			return pn_assign;
		case tt_pipe:
			return pn_pipe;
		case tt_dollar:
			return pn_dollar;
		case tt_amper:
			return pn_amper;
		case tt_lognot:
			return pn_lognot;
		case tt_greater:
			return pn_greater;
		case tt_less:	
			return pn_less;
		case tt_scolon:
			return pn_scolon;
		case tt_colon:
			return pn_colon;
		case tt_oparen:
			return pn_oparen;
		case tt_cparen:
			return pn_cparen;
		case tt_underscore:
			return pn_underscore;
		case tt_obrace:
			return pn_obrace;
		case tt_cbrace:
			return pn_cbrace;
		case tt_obracket:
			return pn_obracket;
		case tt_cbracket:
			return pn_cbracket;
		case tt_tilde:
			return pn_tilde;
		case tt_ident:
			return pn_ident;
	}
	return pn_null;
}

bcParser::bcParser()
{
	tindex=0;
	//out=new tree<bcParseNode>(bcParseNode(pn_head));
	//pindex=out->begin();
}

int bcParser::Parse()
{
	while(GetToken()->type!=tt_eof)
	{
		ParseStatement(this);		
	}
	return 1;
}

int bcParser::Setup(vector<bcToken>* _in)
{
	in=_in;
	ast.tree=new tree<bcParseNode>(bcParseNode(pn_head));
	pindex=ast.tree->begin();
	ast.symtab=new map<std::string,bcSymbol>();
	return 1;
}

bcAST* bcParser::Get()
{
	return &ast;
}

tree<bcParseNode>* bcParser::GetTree()
{
	return ast.tree;
}

std::map<std::string,bcSymbol>* bcParser::GetSymtab()
{
	return ast.symtab;
}

bcToken* bcParser::GetToken()
{
	if(in->size()>0 && tindex<=in->size()-1 && tindex>=0)	return &in->at(tindex);
	return NULL;
}

bcToken* bcParser::NextToken()
{
	if(tindex+1 >= in->size())	return NULL;
	++tindex;
	return GetToken();
}

bcToken* bcParser::NextToken(bcTokenType type)
{
	return GetToken();
}

bcToken* bcParser::PeekToken()
{
	if(tindex+1 >= in->size())	return NULL;
	return &in->at(tindex);
}

void bcParser::SetError(bcErrorCode ec,int l,int c)
{
	errorcode=ec;	
}

tree<bcParseNode>::iterator* bcParser::AddNode(bcParseNode pn)
{
	pindex=ast.tree->append_child(pindex,pn);
	return &pindex;
}


tree<bcParseNode>::iterator* bcParser::AddChild(bcParseNode pn)
{
	ast.tree->append_child(pindex,pn);
	return &pindex;
}

void bcParser::Parent()
{
	pindex=ast.tree->parent(pindex);
}

bcErrorCode bcParser::GetError()
{
	return errorcode;
}

bool bcParser::IsError()
{
	return (errorcode>0);	
}

void parser::ParseStatement(bcParser* par)
{	
	//create parent node	
	par->AddNode(bcParseNode(pn_statement,*par->GetToken()));	
	
	switch(par->GetToken()->type)
	{
		case tt_eof:
			break;

		case tt_scolon:
			//empty statement
			par->NextToken();			
			break;	

		case tt_var:
		case tt_int:
		case tt_string:
		case tt_float:
		case tt_bool:
		case tt_object:
		case tt_function:
		case tt_dec:
			ParseDec(par);
			break;

		default:
			par->SetError(ec_par_invalidtoken,par->GetToken()->line,par->GetToken()->col);
			break;			
	}
	
	//Return to parent node 
	par->Parent();
}

void parser::ParseDec(bcParser* par)
{	
	//init buffers
	

	//get our declaration type	(func/builtin type/user type)
	switch(par->GetToken()->type)
	{
	case tt_function:
		ParseDecFunc(par);
		break;

	case tt_ident:
		ParseIdent(par);
		break;

	case tt_var:
	case tt_int:
	case tt_string:
	case tt_float:
	case tt_bool:
	case tt_object:
		ParseDecVar(par);
		break;

	default:
		//not sure
		break;
	}
	
	//get identifier
	

	//(optional) initialize object
	
	//Return to parent node 
	//par->RTP();
}

void parser::ParseDecFunc(bcParser* par)
{
	//1. get our declaration type	(func/builtin type/user type)
	//	 and create our parent node
	switch(par->GetToken()->type)
	{
	case tt_function:			
		par->AddNode(bcParseNode(pn_funcdec,*par->GetToken()));
		par->NextToken();
		break;

	default:
		//error
		break;
	}
	
	//2. get type/identifier
	switch(par->GetToken()->type)
	{
	//ident (namespace, var, method)
	case tt_ident:		
		ParseIdent(par);		
		break;

	//basic type
	case tt_bool:
	case tt_int:
	case tt_string:
	case tt_float:
		ParseDecFunc_Type(par);
		ParseDecFunc_Ident(par);
		break;

	//object type
	case tt_object:

		break;

	

	default:
		//error
		break;
	}

	//3. (optional) parameter list
	switch(par->GetToken()->type)
	{
	case tt_scolon:
		//forward dec
		//not supported atm
		break;

	case tt_oparen:
		//parameter list
		ParseParamList(par);
		break;

	default:
		//error
		break;
	}

	//4. (optional) function body
	switch(par->GetToken()->type)
	{
	case tt_obrace:
		ParseBlock(par);
		break;

	default:

		break;
	}

	//Return to parent node (statement most times);
	par->Parent();

}

void parser::ParseDecFunc_Type(bcParser* par)
{
	switch(par->GetToken()->type)
	{
	case tt_int:
	case tt_string:
	case tt_float:
	case tt_bool:
		par->AddChild(bcParseNode(pn_funcdec_type,*par->GetToken()));
		par->NextToken();
		break;

	case tt_ident:

		break;

	default:
		//error
		break;
	}
}

void parser::ParseDecFunc_Ident(bcParser* par)
{
	switch(par->GetToken()->type)
	{
	case tt_ident:
		ParseIdent(par);
		break;

	default:
		//error
		break;
	}
}

void parser::ParseDecVar(bcParser* par)
{
	//get type
	switch(par->GetToken()->type)
	{
	case tt_int:
	case tt_string:
	case tt_float:
	case tt_bool:
		ParseDecVar_Type(par);
		break;

	case tt_ident:

		break;

	default:
		//error
		break;
	}

	//get ident
	switch(par->GetToken()->type)
	{
	case tt_ident:
		ParseDecVar_Ident(par);
		break;

	default:
		//error
		break;
	}

	//check for assignment
	switch(par->GetToken()->type)
	{
	case tt_scolon:
		//consume
		par->NextToken();
		break;

	case tt_assign:
		par->NextToken();
		ParseFExp(par);
		break;

	default:
		//error
		break;
	}

}

void parser::ParseDecVar_Type(bcParser* par)
{
	switch(par->GetToken()->type)
	{
	case tt_int:
	case tt_string:
	case tt_float:
	case tt_bool:
		par->AddChild(bcParseNode(pn_vardec_type,*par->GetToken()));
		par->NextToken();
		break;

	default:
		//error
		break;
	}
}

void parser::ParseDecVar_Ident(bcParser* par)
{
	
	switch(par->GetToken()->type)
	{
	case tt_ident:
		ParseIdent(par);
		break;

	default:
		//error
		break;
	}
}

void parser::ParseParamList(bcParser* par)
{
	//add paramlist node	
	par->AddNode(bcParseNode(pn_paramlist));	

	//get our opening parenthesis (
	if(par->GetToken()->type!=tt_oparen)	return;//error!
	par->NextToken();
	
	//loop thru all the parameters in the func call
	while(par->GetToken()->type!=tt_cparen)
	{
		if(par->GetToken()->type!=tt_comma)
		{			
			ParseIdent(par);			
		}
		else
		{
			//error

		}
		//consume comma 
		par->NextToken();
		
	}
	
	//consume cparen
	par->NextToken();	
}

void parser::ParseIdent(bcParser* par)
{
	//create node
	par->AddChild(bcParseNode(pn_ident));	
	
	//1.check our first ident exists in the symbol table
	if(par->GetSymtab()->find( par->GetToken()->data ) == par->GetSymtab()->end())
	{
		//no such identifier
		
	}
	par->NextToken();

	//2.(optional) parse namespace (::) or parse member (.)
	while(par->GetToken()->type==tt_dcolon || par->GetToken()->type==tt_period)
	{
		switch(par->GetToken()->type)
		{
		case tt_dcolon:

			break;

		case tt_period:

			break;

		default:

			break;
		}
	}

	//3.(optional) parse members .


}

void parser::ParseBlock(bcParser* par)
{
	//check for opening brace
	if(par->GetToken()->type!=tt_obrace)
	{
		//error yo
		par->SetError(ec_par_invalidtoken,par->GetToken()->line,par->GetToken()->col);
		return;
	}
	//consume it
	par->NextToken();
	par->AddNode(bcParseNode(pn_block));
	
	//parse inner statements
	while(par->GetToken()->type!=tt_cbrace)
	{
		ParseStatement(par);
	}

	//check for and consume closing brace
	if(par->GetToken()->type!=tt_cbrace)
	{
		//error yo, no closing brace
		par->SetError(ec_par_nocbrace,par->GetToken()->line,par->GetToken()->col);
		return;
	}
	par->NextToken();
}

void parser::ParseFExp(bcParser* par)
{

}


/*
bool parser::CheckOperandTypes(bcToken operand1, bcToken operand2, bcTokenType op)
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

bool parser::CheckOperandBool(bcToken operand2,bcTokenType op)
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

bool parser::CheckOperandInt(bcToken operand2,bcTokenType op)
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

bool parser::CheckOperandString(bcToken operand2,bcTokenType op)
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

bool parser::CheckOperandFloat(bcToken operand2,bcTokenType op)
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
}*/