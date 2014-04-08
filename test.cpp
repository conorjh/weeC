#include "test.h"
#include "util.h"
#include <iostream>

using namespace bc;
using namespace bc::test;
using namespace bc::lex;
using namespace bc::parse;

int bc::test::test_lexer()
{
	bcLexer l;
	l.startup();
	bc::util::bcreadfile("scripts/lex.bcs",l.source);
	l.lex();
	printLexer(&l);
	l.shutdown();
	return 1;
}

int bc::test::test_parser()
{
	bcLexer l;
	l.startup();
	bc::util::bcreadfile("scripts/lex.bcs",l.source);

	bcParser p;
	p.startup();
	p.lexer=&l;
	p.parse();

	return 1;
}


std::string test::getTypeAsString(bcTokenType _t)
{
	switch(_t)
	{
		case tt_null: 
			return "null";  
		case tt_string:
			return "string keyw"; 
		case tt_int: 
			return "int keyw"; 
		case tt_float:
			return "float keyw"; 
		case tt_object:
			return "object keyw"; 
		case tt_var:
			return "var keyw"; 
		case tt_bool:
			return "bool keyw";
		case tt_scolon:
			return "semi colon (;)";
		case tt_colon:
			return "colon (:)";
		case tt_dcolon:
			return "double colon (::)";		
		case tt_comma:
			return "comma"; 
		case tt_period:
			return "period";
		case tt_squote:
			return "single quote";
		case tt_dquote:
			return "double quote";
		case tt_qmark:
			return "question mark";
		case tt_pipe:
			return "pipe";  
		case tt_minus:
			return "minus"; 
		case tt_plus: 
			return "plus"; 
		case tt_div:
			return "divide (/)"; 
		case tt_mult: 
			return "multiply (*)"; 
		case tt_mod: 
			return "modulus";
		case tt_pow:
			return "power"; 
		case tt_assign:
			return "assignment (=)";
		case tt_underscore:
			return "undrescore"; 
		case tt_tilde: 
			return "tilde (`)"; 
		case tt_oparen:
			return "open parenthesis (";  
		case tt_cparen:
			return "close parenthesis )"; 
		case tt_obracket: 
			return "open bracket [";  
		case tt_cbracket: 
			return "close bracket ]";  
		case tt_obrace:
			return "open brace {";  
		case tt_cbrace:
			return "close brace }";  
		case tt_bslash:
			return "backslash \\"; 
		case tt_percent: 
			return "percentage (%)";  
		case tt_newline:
			return "newline";  
		case tt_dollar:
			return "dollar"; 
		case tt_amper:
			return "ampersand &";  
		case tt_greater:
			return "greater than"; 
		case tt_less: 
			return "less than"; 
		case tt_equal:
			return "equal to"; 
		case tt_notequal: 
			return "not equal to";
		case tt_greaterequal: 
			return "equal or greater than";
		case tt_lessequal: 
			return "equal or less than";
		case tt_lognot:
			return "logical not"; 
		case tt_logor:
			return "logical or"; 
		case tt_logand: 
			return "logical and";
		case tt_plusassign:
			return "plus assign +=";
		case tt_minusassign:
			return "minus assign -=";
		case tt_incr: 
			return "increment ++";
		case tt_decr: 
			return "derement --";
		case tt_intlit: 
			return "int literal"; 
		case tt_strlit: 
			return "string literal"; 
		case tt_fltlit: 
			return "float literal";  
		case tt_ident: 
			return "identifier";  
		case tt_comment: 
			return "comment"; 
		case tt_dec:
			return "dec keyw";
		case tt_true:
			return "true keyw";
		case tt_false: 
			return "false keyw"; 
		case tt_function: 
			return "function keyw";  
		case tt_if: 
			return "if keyw";  
		case tt_else: 
			return "else keyw"; 
		case tt_while: 
			return "while keyw"; 
		case tt_break: 
			return "break keyw"; 
		case tt_return: 
			return "return keyw"; 
		case tt_continue: 
			return "continue keyw"; 
		case tt_varident:
			return "variable identifier";
		case tt_funcident: 
			return "function identifier"; 
		case tt_tab: 
			return "tab";
		case tt_ws: 
			return "whitespace";
		case tt_eof: 
			return "end of file";
	}
	return "null";
}

void test::printLexer(bc::lex::bcLexer* l)
{
	std::cout << "Lexer Output " << std::endl;
	for(int t=0;t<l->tokens.size();++t)
	{
		std::cout << "00" << t << " "<< l->tokens[t].y << ","<<l->tokens[t].x<< "	type:\t" << l->tokens[t].type << "\t" << getTypeAsString(l->tokens[t].type) << "\t\t\tdata: " <<  l->tokens[t].data << std::endl;
	}
	std::cout << std::endl;
}
