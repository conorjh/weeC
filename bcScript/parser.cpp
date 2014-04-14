#include "parser.h"

//using namespace bc::parse;
using namespace bc;
using namespace bc::lex;
using bc::lex::bcToken;
using bc::lex::bcTokenType;
using bc::parse::bcParser;
using bc::parse::bcParseNode;
using bc::parse::bcParseNodeType;
using bc::parse::bcSymbol;

bcParseNode::bcParseNode(bcTokenType t)
{
	switch(t)
	{
	case tt_greater: 
		type=pn_greater;	break;
	case tt_less: 
		type=pn_less;	break;
	case tt_equal: 
		type=pn_equal;	break;
	case tt_notequal: 
		type=pn_notequal;	break;
	case tt_greaterequal: 
		type=pn_greaterequal;	break;
	case tt_lessequal:
		type=pn_lessequal;	break;
	case tt_lognot: 
		type=pn_lognot;	break;
	case tt_logor: 
		type=pn_logor;	break;
	case tt_logand: 
		type=pn_logand;	break;
	case tt_incr: 
		type=pn_incr;	break;
	case tt_decr: 
		type=pn_decr;	break;
	case tt_plusassign: 
		type=pn_plusassign;	break;
	case tt_minusassign: 
		type=pn_minusassign;	break;
	case tt_multassign: 
		type=pn_multassign;	break;
	case tt_divassign:
		type=pn_divassign;	break;
	case tt_intlit: 
		type=pn_intlit;	break;
	case tt_strlit: 
		type=pn_strlit;	break;
	case tt_fltlit: 
		type=pn_fltlit;	break;
	case tt_ident:
		type=pn_ident;	break;
	case tt_minus:
		type=pn_minus;	break;
	case tt_plus:			
		type=pn_plus;	break;
	case tt_div:
		type=pn_div;	break;
	case tt_mult: 
		type=pn_mult;	break;
	case tt_mod:
		type=pn_mod;	break;
	case tt_pow:
		type=pn_pow;	break;
	case tt_assign:
		type=pn_assign;	break;
	default:
		type=pn_null;
	}

}

void bcParser::startup()
{
	lexer=NULL;
	parenCount=0;

	//tree
	ast.tree=new tree<bcParseNode>(bcParseNode(pn_head));
	pindex=ast.tree->begin();
	
	//symbol table
	ast.symtab=new std::unordered_map<std::string,bcSymbol>();
	bcSymbol sym;	sym.fullident = sym.ident = "$global";	sym.type = st_namespace; 
	addSymbol(sym.ident,&sym);currentScope = getSymbol("$global");
}


void bcParser::clear()
{
	ast.symtab->clear();
	ast.tree->clear();
	lexer->clear();
}



tree<bcParseNode>::iterator* bcParser::getNode()
{
	return &pindex;
}

tree<bcParseNode>::iterator* bcParser::addNode(bcParseNode pn)
{
	pindex=ast.tree->append_child(pindex,pn);
	return &pindex;
}

tree<bcParseNode>::iterator* bcParser::addChild(bcParseNode pn)
{
	ast.tree->append_child(pindex,pn);
	return &pindex;
}

void bcParser::parent()
{
	pindex=ast.tree->parent(pindex);
}

bool bcParser::addSymbol(bcSymbol* sym)
{
	return addSymbol(sym->fullident,sym);
}

bool bcParser::addSymbol(std::string id,bcSymbol* sym)
{
	if(ast.symtab->find(id)!=ast.symtab->end())
		return false;	//already exists
	ast.symtab->insert( std::make_pair( id,*sym ));
	return true;
}

bcSymbol* bcParser::getSymbol(std::string fullid)
{
	if(ast.symtab->find(fullid)==ast.symtab->end())
		return NULL;
	return &ast.symtab->at(fullid);
}

//shortid
bcSymbol* bcParser::getSymbol(std::string id,bcSymbol* sc)
{
	if(ast.symtab->find(getFullIdent(id,sc))==ast.symtab->end())
		return NULL;
	return &ast.symtab->at(getFullIdent(id,sc));
}

int bcParser::parse()
{
	lexer->nextToken();
	while(!lexer->done)
		parseStatement(this);
	return 1;
}

void parse::parseStatement(bcParser* par)
{	
	//create parent node	
	par->addNode(bcParseNode(pn_statement,*par->lexer->getToken()));	
	bcSymbol sym;

	switch(par->lexer->getToken()->type)
	{
		case tt_eof:
			break;

		case tt_scolon:
			//empty statement
			parseSColon(par);
			break;	

		case tt_var:case tt_int:case tt_string:case tt_float:
		case tt_bool:case tt_object:case tt_function:case tt_dec:
			parseDec(par);
			break;
			
		case tt_if:
			parseIf(par);
			break;

		case tt_while:
			parseWhile(par);
			break;

		case tt_continue:
			parseContinue(par);
			break;

		case tt_break:
			parseBreak(par);
			break;

		case tt_return:
			parseReturn(par);
			break;

		case tt_ident:
			sym = parseIdent(par);
			switch(sym.type)
			{
			case st_var:
				parseAssignment(par);
				break;
			case st_function:
				parseFuncCall(par);
				break;
			break;
			default:
				true;//error
				return;
			}
		default:
			break;			
	}

	//Return to parent node 
	par->parent();
}

void parse::parseDec(bcParser* par)
{	
	bcSymbol sym;
	//get our declaration type	(func/builtin type/user type)
	switch(par->lexer->getToken()->type)
	{
	case tt_function:
		parseDecFunc(par);
		break;

	case tt_int:
	case tt_float:
	case tt_string:
	case tt_bool:
		parseDecVar(par);
		break;

	case tt_ident:sym = parseIdent(par);
			switch(sym.type)
			{
			case st_type:
				parseDecVar(par);				
			break;
			default:
				return;
			}
		break;
	}
}

void parse::parseDecVar(bcParser* par)
{
	bcSymbol sym;
	par->addNode(bcParseNode(pn_vardec));

	//1. type
	switch(par->lexer->getToken()->type)
	{
	case tt_int:
	case tt_float:
	case tt_string:
	case tt_bool:
		par->addChild(bcParseNode(pn_type,*par->lexer->getToken()));
		par->lexer->nextToken();
		break;
	
	case tt_ident:
		sym = parseIdent(par);
		if(sym.type != st_type)
			return;	//error unknown symbol
		par->addChild(bcParseNode(pn_type,sym.fullident));
		break;

	default:
		//error
		break;
	}

	//2. get ident
	switch(par->lexer->getToken()->type)
	{
	case tt_ident:
		sym = parseIdent(par);
		if(sym.type != st_null)
			return;	//error
		sym.type=st_var;
		par->addSymbol(&sym);
		par->addChild(bcParseNode(pn_ident,sym.fullident));
		break;

	default:
		//error
		break;
	}

	//3. semi colon, or assignment (optional)
	switch(par->lexer->getToken()->type)
	{
	case tt_scolon:
		parseSColon(par);
		break;

	case tt_assign:
		par->lexer->nextToken();
		//parseFExp(par);
		parseSColon(par);
		break;

	default:
		//error
		break;
	}
	par->parent();
}

void parse::parseDecFunc(bcParser* par)
{
	bcSymbol* oldScope = par->currentScope;
	//function keyword
	switch(par->lexer->getToken()->type)
	{
	case tt_function:			
		par->addNode(bcParseNode(pn_funcdec,*par->lexer->getToken()));
		par->lexer->nextToken();
		break;

	default:
		//error
		break;
	}
	
	//type and ident
	switch(par->lexer->getToken()->type)
	{
	case tt_ident:
	case tt_bool:
	case tt_int:
	case tt_string:
	case tt_float:
		parseDecFunc_Type(par);
		parseDecFunc_Ident(par);
		break;

	default:
		//error
		break;
	}

	//3. scolon or paramlist
	switch(par->lexer->getToken()->type)
	{
	case tt_scolon:
		//forward dec
		//not supported atm
		par->lexer->nextToken();
		break;

	case tt_oparen:
		//parameter list
		parseParamList(par);
		break;

	default:
		//error
		break;
	}

	//4. (optional) function body
	switch(par->lexer->getToken()->type)
	{
	case tt_obrace:
		parseBlock(par);
		break;

	default:
		break;
	}

	//Return to parent node (statement most times);
	par->currentScope = oldScope;
	par->parent();

}

void parse::parseDecFunc_Type(bcParser* par)
{
	bcSymbol sym;
	switch(par->lexer->getToken()->type)
	{
	case tt_ident:
		sym = parseIdent(par);
		if(sym.type == st_type)
		{
			par->addChild(bcParseNode(pn_type,sym.fullident));
		}
		else
		{
			//error unknown symbol
			return;
		}
		break;

	case tt_int:
	case tt_string:
	case tt_float:
	case tt_bool:
		par->addChild(bcParseNode(pn_type,*par->lexer->getToken()));
		par->lexer->nextToken();
		break;

	default:
		//error
		break;
	}
}

void parse::parseDecFunc_Ident(bcParser* par)
{
	bcSymbol sym;
	switch(par->lexer->getToken()->type)
	{
	case tt_ident:
		sym = parseIdent(par);
		if(sym.type == st_null)		
		{
			sym.type = st_function;
			par->addSymbol(sym.fullident,&sym);
			par->currentScope = par->getSymbol(sym.fullident);
			par->addChild(bcParseNode(pn_funcdec_ident,sym.fullident));
		}
		else
		{
			//error redefinition
		}
		break;

	default:
		//error
		break;
	}
}

void parse::parseParamList(bcParser* par)
{
	//add paramlist node	
	par->addNode(bcParseNode(pn_paramlist));	

	//get our opening parenthesis (
	if(par->lexer->getToken()->type!=tt_oparen)	
		return;	//error!
	par->lexer->nextToken();
	
	//loop thru all the parameters in the func call
	while(par->lexer->getToken()->type!=tt_cparen)
	{
		if(par->lexer->getToken()->type!=tt_comma)
		{			
			parseIdent(par);			
		}
		else
		{
			//error
		}
		//consume comma 
		par->lexer->nextToken();
	}
	
	//consume cparen
	par->lexer->nextToken();
	par->parent();
}

void parse::parseSColon(bcParser* par)
{
	if(par->lexer->getToken()->type!=tt_scolon)
		return;	//error
	par->lexer->nextToken();
}

void parse::parseBlock(bcParser* par)
{
	//opening brace
	if(par->lexer->getToken()->type!=tt_obrace)
		return;	//error
	par->lexer->nextToken();
	par->addNode(bcParseNode(pn_block));
	
	//inner contents
	while(par->lexer->getToken()->type!=tt_cbrace)
		parseStatement(par);

	//closing brace
	par->lexer->nextToken();
	par->parent();
}

void parse::parseIf(bcParser* par)
{	
	//opening if
	if(par->lexer->getToken()->type!=tt_if)
		return;	//error
	par->lexer->nextToken();
	par->addNode(bcParseNode(pn_if));

	//if expression
	parseFExp(par);

	//true block
	parseBlock(par);
	
	//optional elseif/else
	while(par->lexer->getToken()->type==tt_else || par->lexer->getToken()->type==tt_elseif)
	{
		par->addNode(bcParseNode(par->lexer->getToken()->type));
		par->lexer->nextToken();
		parseBlock(par);
	}
}

void parse::parseReturn(bcParser* par)
{	
	//return keyword
	if(par->lexer->getToken()->type!=tt_return)
		return;	//error
	par->lexer->nextToken();
	par->addChild(bcParseNode(pn_return));
	//return expression
	parseFExp(par);
	//semi colon
	parseSColon(par);
}

void parse::parseBreak(bcParser* par)
{	
	if(par->lexer->getToken()->type!=tt_break)
		return;	//error
	par->lexer->nextToken();
	par->addChild(bcParseNode(pn_break));
	parseSColon(par);
}

void parse::parseContinue(bcParser* par)
{	
	if(par->lexer->getToken()->type!=tt_continue)
		return;	//error
	par->lexer->nextToken();
	par->addChild(bcParseNode(pn_continue));
	parseSColon(par);
}

//nextToken til end of ident, return shortident
std::string parse::consumeIdent(bcParser* par)
{
	std::string id;
	bool exit=false;
	while(!exit)
	{
		switch(par->lexer->getToken()->type)
		{
		case tt_ident:
			id+=par->lexer->getToken()->data;
			par->lexer->nextToken();
			break;
		case tt_period:
			id+=par->lexer->getToken()->data;
			par->lexer->nextToken();
			break;
		default:
			exit=true;
		}
		
	}
	return id;
}
bcSymbol parse::parseIdent(bcParser* par)
{
	std::string ident;
	//create node
	par->addNode(bcParseNode(pn_ident));		
	//get symbol, if any, consume ident in lexer
	bcSymbol sym = resolveIdent(par,ident = consumeIdent(par));
	//add fullident name to node
	par->getNode()->node->data.tokens.push_back(bcToken(tt_ident,sym.fullident)); 
	//back to parent
	par->parent();

	return sym;
}

//check ident is a valid symbol in current scope
bcSymbol parse::resolveIdent(bcParser* par,std::string id)
{
	if(par->getSymbol(id,par->currentScope))
		return *par->getSymbol(id,par->currentScope);
	//return a null type symbol if the ident doesnt resolve at currentScope
	bcSymbol sym;	sym.type=st_null;	sym.ident=id;	sym.fullident=getFullIdent(id,par->currentScope);	
	return sym;
}

std::string parse::getFullIdent(std::string ident,bcSymbol* scope)
{
	switch(scope->type)
	{
	case st_namespace:
		return scope->fullident+"::"+ident;
	case st_function:
		return scope->fullident+":"+ident;
	default:
		return "";
	}
}

void parse::parseFuncCall(bcParser* par)
{
	//func name
	if(par->lexer->getToken()->type!=tt_ident)
		return;	//error
	bcSymbol sym = parseIdent(par);
	if(sym.type!=st_function)
		return;	//error
	else if(sym.type==st_null)
		return;	//undeclared ident
	par->addNode(bcParseNode(pn_funccall));
	par->getNode()->node->data.tokens.push_back(bcToken(sym.fullident));

	//params
	parseParamList(par);

	//semi colon
	parseSColon(par);

	par->parent();
}

void parse::parseWhile(bcParser* par)
{
	//while keyword
	if(par->lexer->getToken()->type!=tt_while)
		return;	//error
	par->lexer->nextToken();
	
	par->addChild(bcParseNode(pn_while));
	
	//condition
	parseFExp(par);

	//block to execute
	parseBlock(par);
}

void parse::parseAssignment(bcParser* par)
{
	par->addNode(bcParseNode(pn_assignment));
	par->lexer->nextToken();	//consume "assign
	parseFExp(par);
	parseSColon(par);
	par->parent();
}

//full expression
void parse::parseFExp(bcParser* par)
{	
	par->addNode(bcParseNode(pn_exp));
	parseExp(par);
	par->parent();
	//par->lexer->nextToken();
}

lex::bcToken parse::parseExp(bcParser* par)
{
	bcToken oper1,oper2,op;
	oper1=parseSubExp(par);

	while(par->lexer->getToken())
	{
		op=*par->lexer->getToken();
		par->addChild(bcParseNode(op.type));
		switch(op.type)
		{		
		case tt_less:	case tt_greater:	case tt_lessequal:			case tt_greaterequal:
		case tt_equal:	case tt_notequal:	case tt_logand:		case tt_logor:		case tt_assign:
			par->addChild(bcParseNode(op.type));
			par->lexer->nextToken();
			break;
		default:
			return oper1;
		}
		oper2 = parseSubExp(par);
	}
	return oper1;
}

lex::bcToken parse::parseSubExp(bcParser* par)
{
	bcToken oper1,oper2,op;
	oper1=parseTerm(par);

	while(par->lexer->getToken())
	{
		op=*par->lexer->getToken();
		par->addChild(bcParseNode(op.type));
		switch(op.type)
		{		
		case tt_plus:	case tt_minus: case tt_assign:
			par->addChild(bcParseNode(op.type));
			par->lexer->nextToken();
			break;
		default:
			return oper1;
		}
		oper2 = parseTerm(par);
	}
	return oper1;
}

lex::bcToken parse::parseTerm(bcParser* par)
{
	bcToken oper1,oper2,op;
	oper1=parseFactor(par);

	while(par->lexer->getToken())
	{
		op=*par->lexer->getToken();
		par->addChild(bcParseNode(op.type));
		switch(op.type)
		{		
		case tt_div:	case tt_mult:	case tt_pow:	case tt_mod:
			par->addChild(bcParseNode(op.type));
			par->lexer->nextToken();
			break;
		default:
			return oper1;
		}
		oper2 = parseFactor(par);
	}
	return oper1;
}

bcToken parse::parseFactor(bcParser* par)
{
	bcSymbol sym;
	bcToken oper1,oper2,op;
	bool negate;
	oper1=*par->lexer->getToken();

	//check for negation
	if(oper1.type==tt_minus)
	{
		negate=true;
		oper1 = *par->lexer->nextToken();
	}

	switch(oper1.type)
	{		
	case tt_oparen:
		par->parenCount++;
		par->addChild(bcParseNode(tt_oparen));
		par->lexer->nextToken();

		parseExp(par);
		
		if(par->lexer->getToken()->type!=tt_cparen)
			return NULL;	//error
		par->parenCount--;
		par->addChild(bcParseNode(tt_cparen));
		par->lexer->nextToken();
		break;

	case tt_cparen:
		//error
		return NULL;

	case tt_ident:
		sym = parseIdent(par);
		switch(sym.type)
		{
		case st_var:
		case st_function:
			
			break;
		default:
			//error
			return NULL;
			break;
		}
		break;

	case tt_intlit:
	case tt_strlit:	
	case tt_fltlit:	
	case tt_mod:
	case tt_true:
	case tt_false:
		par->addChild(bcParseNode(oper1.type));
		par->lexer->nextToken();
		break;
	default:
		return NULL;	//error
	}	
	return oper1;
}
