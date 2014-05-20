#include "test.h"
#include "util.h"
#include <iostream>

using namespace bc;
using namespace bc::test;
using namespace bc::lex;
using namespace bc::parse;
using namespace bc::vm;

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
	printParser(&p);
	std::cin >> std::string();
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


std::string test::getTypeAsString(bcParseNodeType _t)
{
	switch(_t)
	{
		case pn_null: 
			return "null";  
		case pn_string:
			return "string keyw"; 
		case pn_int: 
			return "int keyw"; 
		case pn_float:
			return "float keyw"; 
		case pn_object:
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
		case pn_varident: 
			return "var identifier";  
		case pn_funcident: 
			return "func identifier";  
		case pn_namespaceident: 
			return "namespace identifier";  
		case pn_comment: 
			return "comment"; 
		case pn_block:
			return "code blcase ock";
		case pn_paramlist:
			return "parameter list";
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
		case pn_funcdec:
			return "func dec";
		case pn_vardec:
			return "var dec";
		case pn_statement:
			return "statement";
		case pn_exp:
			return "expression";
		case pn_type:
			return "type";
		case pn_head:
			return "head";
	}

	return "null";
}

std::string test::getTypeAsString(bcSymbolType _t)
{
	switch(_t)
	{
	case st_var:
		return "st_var";
	case st_function:
		return "st_function";
	case st_namespace:
		return "st_namespace";
	case st_type:
		return "st_type";
	default:
		return "st_null";
	}
}
void test::printLexer(bc::lex::bcLexer* l)
{
	std::cout << "\n- - -\nbcLexer.tokens\n- - -" << std::endl;
	for(int t=0;t<l->tokens.size();++t)
	{
		std::cout << "00" << t << "	type:\t" << l->tokens[t].type << "\t" << getTypeAsString(l->tokens[t].type) << "\t\t\tdata: " <<  l->tokens[t].data << std::endl;
	}
	std::cout << std::endl;
}

std::string test::getTypeAsString(bcErrorCode t)
{
	switch(t)
	{
	case ec_p_undeclaredsymbol:
		return "undeclared symbol";
	case ec_p_invalidsymbol:
		return "invalid symbol";
	case ec_p_unexpectedtoken:
		return "unexpected token";
	case ec_p_badparams:
		return "incorrect parameters";
	case ec_p_nonintsubscript:
		return "non int array subscript";
	case ec_p_expmustbeconst:
		return "exp must be const";
	case ec_p_expmustbearray:
		return "exp must be array";
	default:
		return "";
	}
}

std::string test::getTypeAsString(vm::bcOpCode _t)
{
	switch(_t)
	{
		case oc_nop:	return "oc_nop";
		case oc_mov:	return "oc_mov";
		case oc_push:	return "oc_push";
		case oc_pop:	return "oc_pop";
		case oc_cmp:	return "oc_cmp";
		case oc_je:		return "oc_je";
		case oc_jne:	return "oc_jne";
		case oc_jg:		return "oc_jg";
		case oc_jl:		return "oc_jl";
		case oc_jge:	return "oc_jge";
		case oc_jle:	return "oc_jle";
		case oc_plus:	return "oc_plus";
		case oc_minus:	return "oc_minus";
		case oc_mult:	return "oc_mult";
		case oc_div:	return "oc_div";
		case oc_expo:	return "oc_expo";
		case oc_mod:	return "oc_mod";
		case oc_inc:	return "oc_inc";
		case oc_dec:	return "oc_dec";
		case oc_and:	return "oc_and";
		case oc_or:		return "oc_or";
		case oc_xor:	return "oc_xor";
		case oc_not:	return "oc_not";
		case oc_shfl:	return "oc_shfl";
		case oc_shfr:	return "oc_shfr";
		case oc_call:	return "oc_call";
		case oc_ret:	return "oc_return";
		case oc_callvm:	return "oc_callvm";
		case oc_pause:	return "oc_pause";
		case oc_halt:	return "oc_halt";
			default:	return "-";
	}
}

std::string test::getTypeAsString(vm::bcValType _t)
{
	switch(_t)
	{
		case vt_null:		return "vt_null";
		case vt_instr:		return "vt_instr";
		case vt_mem:		return "vt_mem";
		case vt_reg:		return "vt_reg";
		case vt_stack:		return "vt_stack";
		case vt_astack:		return "vt_astack";
		case vt_int:		return "vt_int";
		case vt_float:		return "vt_float";
		case vt_string:		return "vt_string";
		case vt_bool:		return "vt_bool";
		case vt_funccall:	return "vt_funccall";
		case vt_vmcall:		return "vt_vmcall";
		default:	return "-";
	}
}

void test::printParser(bc::parse::bcParser* p)
{
	tree<bcParseNode>::iterator it=p->ast.tree->begin();
	std::cout << "\n- - -\nbcParser.ast\n- - -" << std::endl;
	while(it!=p->ast.tree->end()) 
	{
		for(int t=0;t<p->ast.tree->depth(it);++t)
		{
			std::cout << " ";
		}
		std::cout << (it->type) <<"."<< getTypeAsString(it->type) << std::endl;
		++it;
    }

	std::cout <<"\n- - -\n Symbol Table\n- - -" << std::endl;
	std::string types,vars,names,funcs;
	for(auto it = p->ast.symtab->begin();it!=p->ast.symtab->end();++it)
	{
		switch(it->second.type)
		{
		case st_var:
			vars+=getTypeAsString(it->second.type) + " "+it->second.datatype+ "		  " +char(it->second.isConst+48)+" "+ it->second.fullident+" "+util::bcitos(it->second.offset)+"\n"; 
			break;
		case st_namespace:
			names+=getTypeAsString(it->second.type) + "	" + it->second.fullident + "\n"; 
			break;
		case st_function:
			funcs+=getTypeAsString(it->second.type) + "	" + it->second.fullident + "\n"; 
			break;
		case st_type:
			types+=getTypeAsString(it->second.type) + "		  " + it->second.fullident + "\n"; 
			break;
		default:
			std::cout << getTypeAsString(it->second.type) + "		  " + it->second.fullident + "\n";
		}
	}
	std::cout<<vars<<names<<funcs<<types;
	if(p->error)	
		std::cout << getTypeAsString(p->error) << std::endl << p->lexer->source->at(p->errorL) <<std::endl;
}


void test::printGen(bc::vm::bcByteCodeGen* bg)
{
	
	std::cout << "\n- - -\nbcByteCodeGen.istream\n- - -" << std::endl;
	int t=0;
	while(t<bg->istream->size()) 
	{
		std::cout << t << "	op: " << getTypeAsString(bg->istream->at(t).op);
		std::cout << "	arg1: " << getTypeAsString(bg->istream->at(t).arg1.type) << " = " << bg->istream->at(t).arg1.val;
		std::cout << "	arg2: " << getTypeAsString(bg->istream->at(t).arg2.type) << " = " << bg->istream->at(t).arg2.val << std::endl;
		++t;
    }
}