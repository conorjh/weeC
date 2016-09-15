#include "test.h"
#include "util.h"
#include <iostream>

using namespace std;
using namespace wc;
using namespace wc::test;
using namespace wc::lex;
using namespace wc::parse;
using namespace wc::util;

namespace wc {
namespace test {

int wc::test::test_lexer()
{
	wcLexer l;
	readFile("scripts/lex.wcs", l.data.source);
	lex::lexInput(&l);
	printLexer(&l);

	return 1;
}

int wc::test::test_parser()
{
	wcLexer l;
	util::readFile("scripts/lex.wcs", l.data.source);

	wcParser p;
	p.startup();
	p.lexer = &l;
	p.parse();
	printParser(&p);
	cin >> string();
	return 1;
}


string wc::test::getTypeAsString(wcTokenType _t)
{
	switch (_t)
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
	case tt_expo:
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
	case tt_chr:
		return "chr keyw";
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


string test::getTypeAsString(wcParseNodeType _t)
{
	switch (_t)
	{
	case pn_null:
		return "null";
	case pn_str:
		return "string keyw";
	case pn_int:
		return "int keyw";
	case pn_flt:
		return "float keyw";
	case pn_obj:
		return "object keyw";
	case pn_var:
		return "var keyw";
	case pn_bool:
		return "bool keyw";
	case pn_scolon:
		return "semi colon (;)";
	case pn_colon:
		return "colon (:)";

	case pn_comma:
		return "comma";
	case pn_period:
		return "period";
	case pn_squote:
		return "single quote";
	case pn_dquote:
		return "double quote";
	case pn_qmark:
		return "question mark";
	case pn_pipe:
		return "pipe";
	case pn_minus:
		return "minus";
	case pn_plus:
		return "plus";
	case pn_div:
		return "divide (/)";
	case pn_mult:
		return "multiply (*)";
	case pn_mod:
		return "modulus";
	case pn_expo:
		return "power";
	case pn_assign:
		return "assignment (=)";
	case pn_underscore:
		return "undrescore";
	case pn_tilde:
		return "tilde (`)";
	case pn_oparen:
		return "open parenthesis (";
	case pn_cparen:
		return "close parenthesis )";
	case pn_obracket:
		return "open bracket [";
	case pn_cbracket:
		return "close bracket ]";
	case pn_obrace:
		return "open brace {";
	case pn_cbrace:
		return "close brace }";
	case pn_bslash:
		return "backslash \\";
	case pn_percent:
		return "percentage (%)";
	case pn_newline:
		return "newline";
	case pn_dollar:
		return "dollar";
	case pn_amper:
		return "ampersand &";
	case pn_greater:
		return "greater than";
	case pn_less:
		return "less than";
	case pn_equal:
		return "equal to";
	case pn_notequal:
		return "not equal to";
	case pn_greaterequal:
		return "equal or greater than";
	case pn_lessequal:
		return "equal or less than";
	case pn_lognot:
		return "logical not";
	case pn_logor:
		return "logical or";
	case pn_logand:
		return "logical and";
	case pn_plusassign:
		return "plus assign +=";
	case pn_minusassign:
		return "minus assign -=";
	case pn_incr:
		return "increment ++";
	case pn_decr:
		return "derement --";
	case pn_intlit:
		return "int literal";
	case pn_strlit:
		return "string literal";
	case pn_fltlit:
		return "float literal";
	case pn_ident:
		return "identifier";
	case pn_comment:
		return "comment";
	case pn_dec:
		return "dec keyw";
	case pn_true:
		return "true keyw";
	case pn_false:
		return "false keyw";
	case pn_function:
		return "function keyw";
	case pn_if:
		return "if keyw";
	case pn_else:
		return "else keyw";
	case pn_while:
		return "while keyw";
	case pn_break:
		return "break keyw";
	case pn_return:
		return "return keyw";
	case pn_continue:
		return "continue keyw";
	}
	return "null";
}


void test::printLexer(wc::lex::wcLexer* l)
{
	cout << "Lexer Output " << endl;
	for (int t = 0; t < l->data.tokens.size(); ++t)
	{
		cout << "00" << t << " " << l->data.tokens[t].y << "," << l->data.tokens[t].x << "	type:\t" << l->data.tokens[t].type << "\t" << getTypeAsString(l->data.tokens[t].type) << "\t\t\tdata: " << l->data.tokens[t].data << endl;
	}
	cout << endl;
}


void test::printParser(wc::parse::wcParser* p)
{
	tree<wcParseNode>::iterator it = p->ast.tree.begin();
	cout << "Parser" << endl;
	while (it != p->ast.tree.end())
	{
		for (int t = 0; t < p->ast.tree.depth(it); ++t)
		{
			cout << " ";
		}
		cout << (it->type) << "." << getTypeAsString(it->type) << endl;
		++it;
	}


}

}	//wc::test
}	//wc