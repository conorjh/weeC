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

void bcParser::startup()
{
	lexer=NULL;
	ast.tree=new tree<bcParseNode>(bcParseNode(pn_head));
	pindex=ast.tree->begin();
	ast.symtab=new std::unordered_map<std::string,bcSymbol>();
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

bcSymbol* bcParser::getSymbol(std::string id)
{
	if(ast.symtab->find(id)==ast.symtab->end())
		return NULL;
	return &ast.symtab->at(id);
}

int bcParser::parse()
{
	lexer->nextToken();
	while(true)
		parseStatement(this);
	return 1;
}

void parse::parseStatement(bcParser* par)
{	
	//create parent node	
	par->addNode(bcParseNode(pn_statement,*par->lexer->getToken()));	
	
	switch(par->lexer->getToken()->type)
	{
		case tt_eof:
			break;

		case tt_scolon:
			//empty statement
			par->lexer->nextToken();			
			break;	

		case tt_var:case tt_int:case tt_string:case tt_float:
		case tt_bool:case tt_object:case tt_function:case tt_dec:
			parseDec(par);
			break;

		default:
			//par->SetError(ec_par_invalidtoken,par->GetToken()->line,par->GetToken()->col);
			break;			
	}
	//Return to parent node 
	par->parent();
}

void parse::parseDec(bcParser* par)
{	
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
//		parseDecVar(par);
		break;

	case tt_ident:
//		parseIdent(par);
		break;
	}
}

void parse::parseDecVar(bcParser* par)
{
	bcToken* ty=NULL;
	bcToken* id=NULL;
	par->addNode(bcParseNode(pn_vardec));

	//1. get type
	switch(par->lexer->getToken()->type)
	{
	case tt_int:case tt_float:
	case tt_string:case tt_bool:
		ty=par->lexer->getToken();
		par->lexer->nextToken();
		break;

	default:
		//error
		break;
	}

	//2. get ident
	switch(par->lexer->getToken()->type)
	{
	case tt_ident:
		id=par->lexer->getToken();
		parseIdent(par);
		break;

	default:
		//error
		break;
	}

	//3. semi colon, or assignment (optional)
	switch(par->lexer->getToken()->type)
	{
	case tt_scolon:
		par->lexer->nextToken();
		break;

	case tt_assign:
		par->lexer->nextToken();
		//parseFExp(par);
		//parseSColon(par);
		break;

	default:
		//error
		//par->SetError(ec_par_invalidtoken,par->lexer->getToken()->line,par->lexer->getToken()->col);
		break;
	}

	//
}

void parse::parseDecFunc(bcParser* par)
{
	//1. get our declaration type	(func/builtin type/user type)
	//	 and create our parent node
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
	
	//2. get type/identifier
	switch(par->lexer->getToken()->type)
	{
	case tt_ident:
		if(resolveIdent(par,par->lexer->getToken()->data).type != st_null)
		{

		}
		else
		{
			//fall down to bool,int,string,float etc
		}
		break;

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

	//3. (optional) parameter list
	switch(par->lexer->getToken()->type)
	{
	case tt_scolon:
		//forward dec
		//not supported atm
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
	par->parent();

}

void parse::parseDecFunc_Type(bcParser* par)
{
	switch(par->lexer->getToken()->type)
	{
	case tt_int:
	case tt_string:
	case tt_float:
	case tt_bool:
		par->addChild(bcParseNode(pn_funcdec_type,*par->lexer->getToken()));
		par->lexer->nextToken();
		break;

	case tt_ident:

		break;

	default:
		//error
		break;
	}
}

void parse::parseDecFunc_Ident(bcParser* par)
{
	switch(par->lexer->getToken()->type)
	{
	case tt_ident:
		parseIdent(par);
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
}

bcSymbol* parse::parseIdent(bcParser* par)
{
	//create node
	par->addChild(bcParseNode(pn_ident));	
	
	//1.check our first ident exists in the symbol table
	//if(!par->SymbolExists( par->lexer->getToken()->data ))	
	//	return NULL;
	std::string id;
	//namespace
	while(par->getSymbol(par->lexer->getToken()->data,par->currentScope) 
		&& par->getSymbol(par->lexer->getToken()->data,par->currentScope)->type==st_namespace)
	{
		id+=par->lexer->getToken()->data;
		if(par->lexer->nextToken()->type!=tt_dcolon)
			return NULL;	//error
		id+=par->lexer->getToken()->data;
		par->lexer->nextToken();					
	}

	while(par->getSymbol(par->lexer->getToken()->data,par->currentScope) 
		&& par->getSymbol(par->lexer->getToken()->data,par->currentScope)->type==st_var)
	{
		id+=par->lexer->getToken()->data;
		if(par->lexer->nextToken()->type!=tt_period)
			return NULL;	//error
		id+=par->lexer->getToken()->data;
		par->lexer->nextToken();
	}
	
	//3.(optional) parse members
	return NULL;
}

void parse::parseBlock(bcParser* par)
{

}

bcSymbol parse::resolveIdent(bcParser* par,std::string id)
{
	bcSymbol out;
	

	return out;
}