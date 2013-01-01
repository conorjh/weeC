#include "lexer.h"
#include "util.h"
#include <algorithm>

using namespace bc;
using namespace lexer;

bool lexer::IsDelim(string _in)
{
	//switch(DeriveType(_in))
	{

	}
	return false;
}

bcTokenType lexer::DeriveType(string _in)
{
	if(_in.size()==1)	return DeriveType(_in[0]);			
	if(_in=="++") 		return tt_incr;
	if(_in=="--") 		return tt_decr;
	if(_in=="==") 		return tt_equal;
	if(_in=="!=")		return tt_notequal;
	if(_in==">=") 		return tt_greaterequal;
	if(_in=="<=") 		return tt_lessequal;
	if(_in=="!") 		return tt_lognot;
	if(_in=="||")		return tt_logor;
	if(_in=="&&")		return tt_logand;	
	if(_in=="\"")		return tt_dquote;
	if(_in=="\\")		return tt_bslash;
	if(_in=="dec")		return tt_dec;
	if(_in=="var")		return tt_var;
	if(_in=="obj")		return tt_object;
	if(_in=="object")	return tt_object;
	if(_in=="int") 		return tt_int;
	if(_in=="string") 	return tt_string;
	if(_in=="str") 		return tt_string;
	if(_in=="flt") 		return tt_float;
	if(_in=="float") 	return tt_float;
	if(_in=="bool") 	return tt_bool;
	if(_in=="true") 	return tt_true;
	if(_in=="false") 	return tt_false;
	if(_in=="function")	return tt_function;
	if(_in=="func")		return tt_function;
	if(_in=="if")		return tt_if;
	if(_in=="else")		return tt_else;
	if(_in=="while")	return tt_while;
	if(_in=="break")	return tt_break;
	if(_in=="return")	return tt_return;
	if(_in=="continue")	return tt_continue;
	
	return tt_ident;	
}

bcTokenType lexer::DeriveType(char _c)
{
	switch(_c)
	{
	case '\n': 
		return tt_newline;	
	case '\t': 
		return tt_tab;
	case ' ': 
		return tt_ws;
	case '.': 
		return tt_period;
	case ',': 
		return tt_comma;
	case '+': 
		return tt_plus;
	case '-': 
		return tt_minus;
	case '/': 
		return tt_div;
	case '*': 
		return tt_mult;	
	case '%': 
		return tt_mod;
	case '^': 
		return tt_pow;
	case '\'': 
		return tt_squote;
	case '=': 
		return tt_assign;	
	case '|': 
		return tt_pipe;
	case '$': 
		return tt_dollar;
	case '&': 
		return tt_amper;
	case '>': 
		return tt_greater;
	case '<': 
		return tt_less;	
	case ';': 
		return tt_scolon;	
	case '(': 
		return tt_oparen;
	case ')': 
		return tt_cparen;
	case '_': 
		return tt_underscore;
	case '{': 
		return tt_obrace;
	case '}': 
		return tt_cbrace;
	case '[': 
		return tt_obracket;
	case ']': 
		return tt_cbracket;
	case '`': 
		return tt_tilde;
	default:
		return tt_ident;
	}
}

bcLexer::bcLexer()
{
	done=false;
	error=0;
	iserror=false;
	in=NULL;
	out=new vector<bcToken>();
	yindex=xindex=0;	
}

bcLexer::bcLexer(vector<string>* _in)
{
	bcLexer();
	Setup(_in);
}

bcLexer::bcLexer(string _in)
{
	bcLexer();
	Setup(_in);
}

int bcLexer::Setup(vector<string>* _in)
{
	if(!_in)	return 0;
	in=_in;
	yindex=xindex=0;
	done=false;
	return 1;
}

int bcLexer::Setup(string _in)
{
	in=new vector<string>;
	in->push_back(_in);
	yindex=xindex=0;
	done=false;
	return 1;
}

bcToken* bcLexer::GetToken()
{
	if(out->size()==0)	return NULL;
	return &out->at(out->size()-1);
}

int bcLexer::Lex()
{
	xindex=-1;
	while(!done)
	{
		NextToken();
	}
	return 1;
}

bcToken* bcLexer::NextToken()
{	
	//init buffers
	bcToken tokbuff;
	string databuff;

	//get the first char
	if(!IncIndex() || done)		return NULL;
	tokbuff.data=GetChar();
	tokbuff.type=DeriveType(tokbuff.data);

	//- - -token analyis state machine- - -
	switch(tokbuff.type)
	{
		//trim whitespace
		case tt_ws:
		case tt_tab:
		case tt_newline:			
			while(tokbuff.type==tt_ws || tokbuff.type==tt_tab || tokbuff.type==tt_newline)
			{
				if(!IncIndex())		return NULL;			
				tokbuff.type=DeriveType(GetChar());
			}
			return NextToken();
			break;

		//possible comment
		case tt_div:	
			if(PeekToken()->type==tt_div)
			{
				IncIndex();
				while(tokbuff.type!=tt_newline)
				{
					if(!IncIndex())		return NULL;			
					tokbuff.type=DeriveType(GetChar());				
				}
				return NextToken();
			}
			else
			{
				//just a div
				out->push_back(tokbuff);
				return GetToken();
			}
		break;

		//string literal
		case tt_dquote:		
			tokbuff.data="";
			tokbuff.type=tt_strlit;
			//inc past first dquote
			if(!IncIndex())		return NULL;
		
			while(DeriveType(GetChar())!=tt_dquote)
			{
				//collect the inner contents of the quote
				tokbuff.data+=GetChar();		
				if(!IncIndex())		return NULL;
			}
		
			out->push_back(tokbuff);
			return GetToken();
			break;
	
		//build token, check for delims
		default:		
			while((!IsDelim(GetChar())&&(!done)))
			{	
				//add char to data buffer until we hit a delim
				tokbuff.data+=GetChar();			
				IncIndex();
			}
			if(!tokbuff.data.size()<2)	DecIndex();

			//check for the first char being a delim		
			tokbuff.type=DeriveType(tokbuff.data);		
			out->push_back(tokbuff);			
			break;

	}	

	//- - -post token analysis- - -
	//possible float
	

	return GetToken();	
}

bcToken* bcLexer::PeekToken()
{
	int oldx=xindex;
	int oldy=yindex;
	bcToken* tok=NextToken();
	if(tok!=NULL)
	{
		xindex=oldx;
		yindex=oldy;
		done=false;
	}
	
	return tok;
}

string bcLexer::GetChar()
{
	return in->at(yindex).substr(xindex,1);
}

bool bcLexer::IncIndex()
{
	if(xindex+1 <= in->at(yindex).size()-1)
	{		
		xindex++;		
	}
	else
	{
		xindex=0,yindex++;
		if(yindex>in->size()-1)
		{
			xindex=yindex=0;
			done=true;
			return false;	//eof
		}		
	}
	return true;
}

bool bcLexer::DecIndex()
{
	if(xindex<1)
	{
		if(yindex>0)	--yindex;xindex=in->at(0).size()-1;
	}
	else
	{
		--xindex;
	}
	return true;
}