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

	//tree
	ast.tree=new tree<bcParseNode>(bcParseNode(pn_head));
	pindex=ast.tree->begin();
	
	//symbol table
	ast.symtab=new std::unordered_map<std::string,bcSymbol>();
	bcSymbol sym;	sym.fullident = sym.ident = "$global";	sym.type = st_namespace;
	addSymbol(sym.ident,&sym);currentScope = getSymbol("$global");
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
	while(true)
		parseStatement(this);
	return 1;
}

void parse::parseStatement(bcParser* par)
{	
	//create parent node	
	par->addNode(bcParseNode(pn_statement,*par->lexer->getToken()));	
	bcSymbol* sym;

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

		case tt_ident:
			sym = &parseIdent(par);
			switch(sym->type)
			{
			case st_var:
			case st_function:
				parseAssignment(par);
			break;
			default:
				//error
				return;
			}
		default:
			//par->SetError(ec_par_invalidtoken,par->GetToken()->line,par->GetToken()->col);
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

	case tt_ident:
		sym = parseIdent(par);
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
			par->addChild(bcParseNode(pn_ident,sym.fullident));
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
}

void parse::parseSColon(bcParser* par)
{
	if(par->lexer->getToken()->type!=tt_scolon)
		return;	//error
	par->lexer->nextToken();
}


bcSymbol parse::parseIdent(bcParser* par)
{
	//create node
	par->addNode(bcParseNode(pn_ident));	
	
	std::string id;
	while(par->lexer->getToken()->type == tt_ident)
	{
		id+=par->lexer->getToken()->data;
		par->lexer->nextToken();
	}

	if(par->getSymbol(id,par->currentScope))
		return *par->getSymbol(id,par->currentScope);
	
	bcSymbol sym;	sym.type=st_null;	sym.ident=id;	sym.fullident=getFullIdent(id,par->currentScope);	
	par->getNode()->node->data.tokens.push_back(bcToken(tt_ident,id)); 

	par->parent();
	return sym;
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

bcSymbol parse::resolveIdent(bcParser* par,std::string id)
{
	bcSymbol out;
	out.type = st_null;
	
	return out;
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

//full expression
void parse::parseFExp(bcParser* par)
{	
	parseExp(par);
}

lex::bcToken parse::parseExp(bcParser* par)
{
	
	return bcToken();
}

lex::bcToken parse::parseSubExp(bcParser* par)
{
	bcToken out;
	return out;
}

lex::bcToken parse::parseTerm(bcParser* par)
{
	bcToken out;	

	return out;
}

bcToken parse::ParseFactor()
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
