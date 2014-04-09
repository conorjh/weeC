#include "lexer.h"

using namespace bc;
using bc::lex::bcTokenType;
using bc::lex::bcToken;
using bc::lex::bcLexer;
using std::string;
using std::vector;

void bcLexer::startup()
{
	x=-1;y=0;
	done=false;
	source = new std::vector<std::string>();
}

void bcLexer::shutdown()
{
	x=-1;y=0;
	delete source;
}

bcToken* bcLexer::getToken()
{
	if(!tokens.size())
		return NULL;
	return &tokens[tokens.size()-1];
}

int bcLexer::lex()
{
	while(nextToken())
	{
		true;
	}
	return true;
}

bcToken* bcLexer::nextToken()
{
	bcToken tok;
	if(!inc())
		return NULL;	//some kind of error
	tok.x=x;	tok.y=y;
	tok.data=getChar();
	tok.type=getTokenType(&tok.data);
	
	//Lexer State Machine
	switch(tok.type)
	{
		//drop whitespace
		case tt_ws:
		case tt_tab:
		case tt_newline:	
			//chomp up reocurring whitespace
			while(tok.type==tt_ws || tok.type==tt_tab || tok.type==tt_newline)
			{
				if(!inc())		
					return NULL;			
				tok.type=getTokenType(&getChar());
			}
			dec();
			return nextToken();

			
		//possible comment	DROP
		case tt_div:	
			if(getTokenType(&peekChar())==tt_div)
			{
				inc();				
				while(getTokenType(&getChar())!=tt_newline)
				{
					if(!inc())		break;			
					tok.type=getTokenType(&getChar());				
				}
				dec();
				return nextToken();
			}
			else if(getTokenType(&peekChar())==tt_mult)
			{
				//multiline comment /* */
				inc();					
				while(inc())
				{		
					tok.type=getTokenType(&getChar());	
					if(tok.type==tt_mult)
						if(getTokenType(&peekChar())==tt_div)
						{
							inc();
							return nextToken();
						}					
				}				
				//eof before end of comment error
				return NULL;
			}
			else tokens.push_back(tok);
			return getToken();
			
		case tt_intlit:	//int/float lit		
			inc();				
			while(getTokenType(&getChar())==tt_intlit)
			{						
				tok.data+=getChar();
				if(!inc())		
					break;				
			}					
			//check for float
			if(getTokenType(&getChar())==tt_period)
			{
				//must be float now
				tok.data+=".";
				inc();				
				while(getTokenType(&getChar())==tt_intlit)
				{						
					tok.data+=getChar();
					if(!inc())		break;				
				}			
				tok.type=tt_fltlit;
				if(tok.data.substr(tok.data.size()-1,1)==".")	
					return NULL;//error, no rvalue
			}					
			dec();
			tokens.push_back(tok);
			return getToken();
		
		case tt_dquote:	//string literal
			tok.data="";
			tok.type=tt_strlit;
			if(!inc())		return NULL;	//inc past first dquote		
			while(getTokenType(&getChar())!=tt_dquote)
			{
				//collect the inner contents of the quote
				tok.data+=getChar();		
				if(!inc())		return NULL;
			}
			tokens.push_back(tok);
			return getToken();
		
		case tt_greater:// >=			
		case tt_less:// <=
		case tt_lognot:	// !=
		case tt_assign:// ==			
		case tt_plus:// ++ or +=	
		case tt_minus:// -- or -=			
		case tt_amper:// &&			
		case tt_pipe:// ||
		case tt_colon:// ::
			//dual step tokens		
			if(!((getTokenType(&(getChar()+peekChar()))!=tt_greaterequal) && (getTokenType(&(getChar()+peekChar()))!=tt_lessequal) &&
				(getTokenType(&(getChar()+peekChar()))!=tt_notequal) && (getTokenType(&(getChar()+peekChar()))!=tt_equal) && 
				(getTokenType(&(getChar()+peekChar()))!=tt_incr) && (getTokenType(&(getChar()+peekChar()))!=tt_plusassign) &&
				(getTokenType(&(getChar()+peekChar()))!=tt_decr) && (getTokenType(&(getChar()+peekChar()))!=tt_minusassign) &&
				(getTokenType(&(getChar()+peekChar()))!=tt_logand) &&(getTokenType(&(getChar()+peekChar()))!=tt_logor) &&	
				(getTokenType(&(getChar()+peekChar()))!=tt_dcolon)))	
				{
					inc();
					tok.data+=getChar();
					tok.type=getTokenType(&tok.data);
				}			
			tokens.push_back(tok);
			return getToken();
			
		
		default:	//otherwise, build token, check for delims	
			if(!isDelim(&tok.data))
				while((!isDelim(&peekChar())&&(!done)))
				{	
					//add char to data buffer until we hit a delim
					inc();	tok.data+=getChar();															
				}		
			tok.type=getTokenType(&tok.data);		
			tokens.push_back(tok);	
			return getToken();
	}
}

bool bcLexer::inc()
{	
	if(x+1 <= source->at(y).size()-1)
	{		
		x++;		
	}
	else
	{
		x=0,y++;
		if(y>source->size()-1)
		{
			x=y=-1;
			return false;	//eof
		}		
	}
 	return true;
}

bool bcLexer::dec()
{
	if(x==0)
		if(y>0)
		{
			--y;
			x=source->at(y).size()-1;
		}
		else
		{
			return false;
		}
	else
		--x;
	return true;
}
string bcLexer::getChar()
{
	return source->at(y).substr(x,1);
}

string bcLexer::peekChar()
{
	int _y,_x;
	if(inc())
	{		
		_x=x;
		_y=y;
		dec();
		return source->at(_y).substr(_x,1);
	}
	else
	{
		return "";
	}	
}

bool lex::isDelim(string* s)
{
	switch(getTokenType(s))
	{
	case tt_ws:		
	case tt_newline:
	case tt_tab: 
	case tt_period:
	case tt_comma: 
	case tt_plus: 
	case tt_minus:
	case tt_div:
	case tt_mult:
	case tt_pow: 
	case tt_mod: 
	case tt_squote:
	case tt_assign:
	case tt_pipe: 
	case tt_dollar:
	case tt_amper: 
	case tt_greater:
	case tt_less: 
	case tt_lognot:
	case tt_dquote:
	case tt_bslash:
	case tt_scolon:
	case tt_percent:
	case tt_oparen: 
	case tt_cparen: 
	case tt_obrace: 
	case tt_cbrace: 
	case tt_obracket:
	case tt_cbracket:
	case tt_tilde: 
	case tt_colon: 
		return true;
	default:
		return false;
	}
}

bcTokenType lex::getTokenType(string* s)
{
	if(s->size()==1)				return getTokenType(s->at(0));
	if(*s=="++") 					return tt_incr;
	if(*s=="--") 					return tt_decr;
	if(*s=="+=") 					return tt_plusassign;
	if(*s=="-=") 					return tt_minusassign;
	if(*s=="*=") 					return tt_multassign;
	if(*s=="/=") 					return tt_divassign;
	if(*s=="==") 					return tt_equal;
	if(*s=="!=")					return tt_notequal;
	if(*s==">=") 					return tt_greaterequal;
	if(*s=="<=") 					return tt_lessequal;
	if(*s=="!") 					return tt_lognot;
	if(*s=="||")					return tt_logor;
	if(*s=="&&")					return tt_logand;	
	if(*s=="::")					return tt_dcolon;	
	if(*s=="\"")					return tt_dquote;
	if(*s=="\\")					return tt_bslash;
	if(*s=="dec")					return tt_dec;
	if(*s=="var")					return tt_var;
	if(*s=="obj"||*s=="object")		return tt_object;
	if(*s=="int") 					return tt_int;
	if(*s=="string"||*s=="str") 	return tt_string;
	if(*s=="flt"||*s=="float") 		return tt_float;
	if(*s=="bool")					return tt_bool;
	if(*s=="true") 					return tt_true;
	if(*s=="false") 				return tt_false;
	if(*s=="function")				return tt_function;
	if(*s=="func")					return tt_function;
	if(*s=="if")					return tt_if;
	if(*s=="else")					return tt_else;
	if(*s=="while")					return tt_while;
	if(*s=="break")					return tt_break;
	if(*s=="return")				return tt_return;
	if(*s=="continue")				return tt_continue;	
	return tt_ident;	
}

bcTokenType lex::getTokenType(char c)
{
	switch(c)
	{
	case '0':	
	case '1':		
	case '2':		
	case '3':		
	case '4':		
	case '5':		
	case '6':
	case '7':		
	case '8':		
	case '9':
		return tt_intlit;		
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
	case '"': 
		return tt_dquote;
	case '=': 
		return tt_assign;	
	case '|': 
		return tt_pipe;
	case '$': 
		return tt_dollar;
	case '&': 
		return tt_amper;
	case '!':
		return tt_lognot;
	case '>': 
		return tt_greater;
	case '<': 
		return tt_less;	
	case ';': 
		return tt_scolon;
	case ':':
		return tt_colon;
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
