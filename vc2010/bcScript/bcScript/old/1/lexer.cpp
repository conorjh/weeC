#include "lexer.h"
#include "util.h"
#include <algorithm>
//#include <cctype>

bcLexer::bcLexer()
{
	error = false;
	errorcode = ec_null;
	xindex=0;
	yindex=0;

	delims.push_back(tt_ws);
	delims.push_back(tt_newline);
	delims.push_back(tt_tab);
	delims.push_back(tt_period);
	delims.push_back(tt_comma);
	delims.push_back(tt_plus);
	delims.push_back(tt_minus);
	delims.push_back(tt_div);
	delims.push_back(tt_mult);
	delims.push_back(tt_pow);
	delims.push_back(tt_mod);
	delims.push_back(tt_squote);
	delims.push_back(tt_assign);
	
	delims.push_back(tt_pipe);
	delims.push_back(tt_dollar);
	delims.push_back(tt_amper);
	delims.push_back(tt_greater);
	delims.push_back(tt_less);
	delims.push_back(tt_lognot);
	delims.push_back(tt_dquote);
	delims.push_back(tt_bslash);
	delims.push_back(tt_scolon);
	delims.push_back(tt_percent);
	delims.push_back(tt_oparen);
	delims.push_back(tt_cparen);
	delims.push_back(tt_obrace);
	delims.push_back(tt_cbrace);
	delims.push_back(tt_obracket);
	delims.push_back(tt_cbracket);
	
	delims.push_back(tt_tilde);
	
	tokens.insert( make_pair("\n" , tt_newline));	
	tokens.insert( make_pair("\t" , tt_tab));
	tokens.insert( make_pair(" " , tt_ws));
	tokens.insert( make_pair("." , tt_period));
	tokens.insert( make_pair("," , tt_comma));
	tokens.insert( make_pair("+" , tt_plus));
	tokens.insert( make_pair("-" , tt_minus));
	tokens.insert( make_pair("/" , tt_div));
	tokens.insert( make_pair("*" , tt_mult));
	tokens.insert( make_pair("%" , tt_mod));
	tokens.insert( make_pair("^" , tt_pow));
	tokens.insert( make_pair("'" , tt_squote));
	tokens.insert( make_pair("=" , tt_assign));
	
	tokens.insert( make_pair("|" , tt_pipe));
	tokens.insert( make_pair("$" , tt_dollar));
	tokens.insert( make_pair("&" , tt_amper));
	tokens.insert( make_pair(">" , tt_greater));
	tokens.insert( make_pair("<" , tt_less));

	tokens.insert( make_pair("++" , tt_incr));
	tokens.insert( make_pair("--" , tt_decr));
	tokens.insert( make_pair("==" , tt_equal));
	tokens.insert( make_pair("!=" , tt_notequal));
	tokens.insert( make_pair(">=" , tt_greaterequal));
	tokens.insert( make_pair("<=" , tt_lessequal));
	tokens.insert( make_pair("!" , tt_lognot));
	tokens.insert( make_pair("||" , tt_logor));
	tokens.insert( make_pair("&&" , tt_logand));
	
	tokens.insert( make_pair("\"" , tt_dquote));
	tokens.insert( make_pair("\\" , tt_bslash));
	tokens.insert( make_pair(";" , tt_scolon));	
	tokens.insert( make_pair("%" , tt_percent));
	tokens.insert( make_pair("(" , tt_oparen));
	tokens.insert( make_pair(")" , tt_cparen));
	tokens.insert( make_pair("_" , tt_underscore));
	tokens.insert( make_pair("{" , tt_obrace));
	tokens.insert( make_pair("}" , tt_cbrace));
	tokens.insert( make_pair("[" , tt_obracket));
	tokens.insert( make_pair("]" , tt_cbracket));
	tokens.insert( make_pair("`" , tt_tilde));
	
	tokens.insert( make_pair("dec" , tt_dec));
	tokens.insert( make_pair("var" , tt_var));
	tokens.insert( make_pair("obj" , tt_object));
	tokens.insert( make_pair("object" , tt_object));
	tokens.insert( make_pair("int" , tt_int));
	tokens.insert( make_pair("string" , tt_string));
	tokens.insert( make_pair("str" , tt_string));
	tokens.insert( make_pair("flt" , tt_float));
	tokens.insert( make_pair("float" , tt_float));
	tokens.insert( make_pair("bool" , tt_bool));
	tokens.insert( make_pair("true", tt_true));
	tokens.insert( make_pair("false", tt_false));
	tokens.insert( make_pair("function",tt_function));
	tokens.insert( make_pair("func",tt_function));
	tokens.insert( make_pair("if",tt_if));
	tokens.insert( make_pair("else",tt_else));
	tokens.insert( make_pair("while",tt_while));
	tokens.insert( make_pair("break",tt_break));
	tokens.insert( make_pair("return",tt_return));
	tokens.insert( make_pair("continue",tt_continue));	
}

bcLexer::bcLexer(vector<string>* _in)
{
	bcLexer();	
	Lex(_in);
}

int bcLexer::Lex(vector<string>* _in)
{
	data.clear();
	in=_in;
	
	string strbuff="";
	int digitsafterperiod=0;
	int linebuff=1;
	int colbuff=1;
	xindex=0;
	yindex=0;
	bool eof=false;
	bcToken tokbuff;
	bcTokenType typebuff=GetTokenType(in->at(xindex).at(yindex));

	while(!eof)
	{
		switch(typebuff)
		{
		
			case tt_newline:
			case tt_tab:
			case tt_ws:
				//drop ws				
				eof=Inc();
				if(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));				
				}				
				break;

			


			case tt_intlit:
				linebuff=xindex+1;
				colbuff=yindex+1;

				strbuff+=in->at(xindex).at(yindex);
				eof=Inc();
				
				//loop until we find a non number
				while(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));
					if(typebuff==tt_intlit)
					{
						strbuff+=in->at(xindex).at(yindex);
					}
					else
					{
						break;
					}					
					eof=Inc();
				}
				
				//check if we found a .
				if(typebuff==tt_period)
				{
					strbuff+=".";
					eof=Inc();
					while(!eof)
					{	
						typebuff=GetTokenType(in->at(xindex).at(yindex));
						if(typebuff==tt_intlit)
						{
							strbuff+=in->at(xindex).at(yindex);
							++digitsafterperiod;
						}
						else
						{
							if(digitsafterperiod==0)
							{
								//not a float  (intlit,period,non-intlit)
								//push first intlit
								tokbuff.data=strbuff;	
								tokbuff.type=tt_intlit;
								tokbuff.line=linebuff;
								tokbuff.col=colbuff;
								data.push_back(tokbuff);
								strbuff="";
								//push last token .
								tokbuff.data=".";	
								tokbuff.type=tt_period;
								tokbuff.line=xindex+1;	//taking a guess here, Inc() could have moved us to the next line
								tokbuff.col=yindex-2;
								data.push_back(tokbuff);
								
								//let default handle this token
								break;
							}
							else
							{
								//intlit period intlit
								tokbuff.data=strbuff;	
								tokbuff.type=tt_fltlit;
								tokbuff.line=linebuff;
								tokbuff.col=colbuff;
								data.push_back(tokbuff);
								strbuff="";
							}
							break;
						}					
						eof=Inc();
					}
				}
				else
				{
					//no period, push intlit, let default: handle this token
					tokbuff.data=strbuff;	
					tokbuff.type=tt_intlit;
					tokbuff.line=linebuff;
					tokbuff.col=colbuff;
					data.push_back(tokbuff);
					strbuff="";
					break;
				}				
				break;

			//check for two char tokens
			case tt_greater:
				tokbuff.data=">";	
				tokbuff.type=tt_greater;
				tokbuff.line=xindex+1;
				tokbuff.col=yindex+1;
				
				eof=Inc();
				if(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));
					if(typebuff==tt_assign)
					{
						//greater or equal to (>=)
						tokbuff.type=tt_greaterequal;
						tokbuff.data=">=";
						data.push_back(tokbuff);
						eof=Inc();
						if(!eof)	typebuff=GetTokenType(in->at(xindex).at(yindex));
					}
					else
					{
						data.push_back(tokbuff);
					}
				}
				break;

			case tt_less:
				tokbuff.data="<";	
				tokbuff.type=tt_less;
				tokbuff.line=xindex+1;
				tokbuff.col=yindex+1;
				
				eof=Inc();
				if(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));
					if(typebuff==tt_assign)
					{
						//less or equal to (<=)
						tokbuff.type=tt_lessequal;
						tokbuff.data="<=";
						data.push_back(tokbuff);
						eof=Inc();
						if(!eof)	typebuff=GetTokenType(in->at(xindex).at(yindex));
					}
					else
					{
						data.push_back(tokbuff);
					}
				}
				break;

			case tt_lognot:
				tokbuff.data="!";	
				tokbuff.type=tt_lognot;
				tokbuff.line=xindex+1;
				tokbuff.col=yindex+1;
				
				eof=Inc();
				if(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));
					if(typebuff==tt_assign)
					{
						//not equal to (!=)
						tokbuff.type=tt_notequal;
						tokbuff.data="!=";
						data.push_back(tokbuff);
						eof=Inc();
						if(!eof)	typebuff=GetTokenType(in->at(xindex).at(yindex));
					}
					else
					{
						data.push_back(tokbuff);
					}
				}
				break;

			case tt_amper:
				tokbuff.data="&";	
				tokbuff.type=tt_amper;
				tokbuff.line=xindex+1;
				tokbuff.col=yindex+1;
				
				eof=Inc();
				if(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));
					if(typebuff==tt_amper)
					{
						//&&
						tokbuff.type=tt_logand;
						tokbuff.data="&&";
						data.push_back(tokbuff);
						eof=Inc();
						if(!eof)	typebuff=GetTokenType(in->at(xindex).at(yindex));
					}
					else
					{
						data.push_back(tokbuff);
					}
				}
				break;

			case tt_pipe:
				tokbuff.data="|";	
				tokbuff.type=tt_pipe;
				tokbuff.line=xindex+1;
				tokbuff.col=yindex+1;
				
				eof=Inc();
				if(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));
					if(typebuff==tt_pipe)
					{
						//||
						tokbuff.type=tt_logor;
						tokbuff.data="||";
						data.push_back(tokbuff);
						eof=Inc();
						if(!eof)	typebuff=GetTokenType(in->at(xindex).at(yindex));
						break;
					}
				}
				data.push_back(tokbuff);
				break;

			case tt_assign:
				tokbuff.data="=";	
				tokbuff.type=tt_assign;
				tokbuff.line=xindex+1;
				tokbuff.col=yindex+1;
				
				eof=Inc();
				if(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));
					if(typebuff==tt_assign)
					{
						//==
						tokbuff.type=tt_equal;
						tokbuff.data="==";
						data.push_back(tokbuff);
						eof=Inc();
						if(!eof)	typebuff=GetTokenType(in->at(xindex).at(yindex));
						break;
					}
				}
				data.push_back(tokbuff);
				break;

			case tt_plus:
				tokbuff.data="+";	
				tokbuff.type=tt_plus;
				tokbuff.line=xindex+1;
				tokbuff.col=yindex+1;
				
				eof=Inc();
				if(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));
					if(typebuff==tt_plus)
					{
						//++
						tokbuff.type=tt_incr;
						tokbuff.data="++";
						data.push_back(tokbuff);
						eof=Inc();
						if(!eof)	typebuff=GetTokenType(in->at(xindex).at(yindex));
						break;
					}
				}
				data.push_back(tokbuff);
				break;

			case tt_minus:
				tokbuff.data="-";	
				tokbuff.type=tt_minus;
				tokbuff.line=xindex+1;
				tokbuff.col=yindex+1;
				
				eof=Inc();
				if(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));
					if(typebuff==tt_minus)
					{
						//--
						tokbuff.type=tt_decr;
						tokbuff.data="--";
						data.push_back(tokbuff);
						eof=Inc();
						if(!eof)	typebuff=GetTokenType(in->at(xindex).at(yindex));
						break;
					}
				}
				data.push_back(tokbuff);
				break;

			case tt_div:				
				eof=Inc();				
				if(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));
					//if this next char is /, were in a comment
					if(typebuff==tt_div)
					{
						//in a comment, readahead til newline
						eof=Inc();						
						while(!eof)
						{
							typebuff=GetTokenType(in->at(xindex).at(yindex));
							if(typebuff==tt_newline)
							{
								eof=Inc();
								if(!eof)
								{
									typebuff=GetTokenType(in->at(xindex).at(yindex));
								}
								break;
							}
							else
							{
								eof=Inc();
							}
						}
					}
					else
					{
						//add the tt_div,let the main loop handle this current token
						tokbuff.data="/";	
						tokbuff.type=tt_div;
						tokbuff.line=xindex+1;
						tokbuff.col=yindex+1;
						data.push_back(tokbuff);
						break;
					}
					
				}
				else
				{
					break;
				}
		
				break;

			case tt_dquote:
				linebuff=xindex+1;
				colbuff=yindex+1;
				eof=Inc();
				if(!eof)
				{
					typebuff=GetTokenType(in->at(xindex).at(yindex));
				}
				while(!eof)
				{
					if(typebuff!=tt_dquote)
					{
						strbuff+=in->at(xindex).at(yindex);
						eof=Inc();
						if(!eof)
						{
							typebuff=GetTokenType(in->at(xindex).at(yindex));
						}
					}
					else
					{
						tokbuff.data=strbuff;	
						tokbuff.type=tt_strlit;
						tokbuff.line=linebuff;
						tokbuff.col=colbuff;
						data.push_back(tokbuff);
						strbuff="";
						eof=Inc();
						if(!eof)
						{
							typebuff=GetTokenType(in->at(xindex).at(yindex));
						}
						break;
					}
				}
				break;

			default:
				linebuff=xindex+1;
				colbuff=yindex+1;
				while(!eof)
				{
					if(IsDelim(in->at(xindex).at(yindex)))
					{
						//push buffer
						if(strbuff.size()>0)
						{
							tokbuff.data=bctolower(strbuff);	
							tokbuff.type=GetTokenType(tokbuff.data);
							tokbuff.line=linebuff;
							tokbuff.col=colbuff;
							//convert aliases
							if(tokbuff.data=="str") tokbuff.data="string";
							if(tokbuff.data=="obj") tokbuff.data="object";
							if(tokbuff.data=="flt") tokbuff.data="float";							
							data.push_back(tokbuff);
							strbuff="";
						}
						//push this single char delim
						tokbuff.data=in->at(xindex).at(yindex);
						tokbuff.type=typebuff;
						tokbuff.line=xindex+1;
						tokbuff.col=yindex+1;

						//dont push transitional tokens (multi char tokens)
						if(	tokbuff.type==tt_intlit ||
							tokbuff.type==tt_dquote	||
							tokbuff.type==tt_ws		||
							tokbuff.type==tt_newline||
							tokbuff.type==tt_tab	||	
							tokbuff.type==tt_plus	||
							tokbuff.type==tt_minus	||
							tokbuff.type==tt_assign	||
							tokbuff.type==tt_amper	||
							tokbuff.type==tt_pipe	||
							tokbuff.type==tt_less	||
							tokbuff.type==tt_greater||
							tokbuff.type==tt_lognot	
						)
						{

						}
						else
						{
							data.push_back(tokbuff);
							eof=Inc();
							if(!eof)
							{
								typebuff=GetTokenType(in->at(xindex).at(yindex));
							}
						}
						break;
					}
					else
					{
						//add to stringbuffer
						strbuff+=in->at(xindex).at(yindex);
						eof=Inc();
						if(!eof)
						{
							typebuff=GetTokenType(in->at(xindex).at(yindex));
						}
					}
				}
				break;			
		}
	}
	return 1;
}

bool bcLexer::Inc()
{
	//inc pointer
	if(yindex+1 >= in->at(xindex).size())
	{
		if(xindex+1 >= in->size())
		{
			//eof
			return true;
		}
		else
		{
			yindex=0;
			++xindex;
			return false;
		}
	}
	else
	{
		++yindex;
		return false;
	}
}

bcTokenType bcLexer::GetTokenType(char in)
{
	map<string,bcTokenType>::iterator it;
	string buff="";
	buff.append(1,in);
		
	//single char token
	it=tokens.find(buff);
	
	if(it == tokens.end())
	{
		if(isdigit(in))
		{
			//number
			return tt_intlit;
		}

		//unknown, maybe part of a valid token though
		return tt_ident;
	}
	
	return tokens[buff];
}

bcTokenType bcLexer::GetTokenType(string in)
{
	map<string,bcTokenType>::iterator it;

	//check for empty input
	if(bcLexer_CheckEmptyInput==true && in.size()==0)
	{
		return tt_unk; 
	}

	//single char
	if(in.size()==1)
	{
		return GetTokenType(in[0]);
	}
			
	//first char is numeric, cant be an ident, must be a number
	if(isdigit(in.at(0)))
	{
		return tt_intlit;
	}
		
	//search against tokens
	it=tokens.find(in);
		
	if(it == tokens.end())
	{
		//unkown, treat as identifier
		return tt_ident;
	}
	else
	{
		return tokens[in];
	}
}

bool bcLexer::IsDelim(char in)
{
	vector<bcTokenType>::iterator it;
	it = std::find(delims.begin(),delims.end(),GetTokenType(in));
	
	if(it==delims.end())
	{
		return false;
	}		
	return true;
}

bool bcLexer::IsDelim(string in)
{
	if(in.size()==1)
	{
		return IsDelim(in[0]);
	}	

	vector<bcTokenType>::iterator it;
	it = std::find(delims.begin(),delims.end(),GetTokenType(in));
	if(it==delims.end())
	{	
		return false;
	}	
	return true;
}

