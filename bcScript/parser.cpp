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
using bc::parse::bcSymbolType;

bcParseNode::bcParseNode(bcSymbolType t)
{
	switch(t)
	{
	case st_null:
		type=pn_null;
		break;
	case st_var:
		type=pn_varident;
		break;
	case st_function:
		type=pn_funcident;
		break;
	case st_namespace:
		type=pn_namespaceident;
		break;
	}
}

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
	case tt_oparen:
		type=pn_oparen;	break;
	case tt_cparen:
		type=pn_cparen;	break;
	default:
		type=pn_null;
	}

}

void bcParser::startup()
{
	parenCount=0;
	currentFunc=NULL;
	currentScope=NULL;
	noDecVar=noDecFunc=noDecName=false;

	//tree
	ast.tree=new tree<bcParseNode>(bcParseNode(pn_head));
	pindex=ast.tree->begin();
	
	//symbol table
	ast.stackframes.push_back(std::vector<std::string>());	//global stackframe
	ast.functab=new std::unordered_map<std::string,bcFuncInfo>();
	ast.symtab=new std::unordered_map<std::string,bcSymbol>();
	//add global symbols
	bcSymbol sym;	sym.fullident = sym.ident = "$global";	sym.type = st_namespace; 
	addSymbol(sym.ident,&sym);currentScope = getSymbol("$global");
	//basic type symbol entries
	sym.datatype = sym.fullident = sym.ident = "int";		sym.type = st_type; 	addSymbol(sym.ident,&sym);
	sym.datatype = sym.fullident = sym.ident = "float";		sym.type = st_type; 	addSymbol(sym.ident,&sym);
	sym.datatype = sym.fullident = sym.ident = "string";	sym.type = st_type; 	addSymbol(sym.ident,&sym);
	sym.datatype = sym.fullident = sym.ident = "bool";		sym.type = st_type;		addSymbol(sym.ident,&sym);
}

void bcParser::clear()
{
	ast.symtab->clear();
	ast.tree->clear();
	lexer->clear();
}

unsigned int bcParser::getError()
{
	return ec_null;
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
	//add global so coder doesnt have to
	if(fullid.size() && fullid.substr(0,9)!="$global::" && (fullid!="$global" && fullid!="int" && fullid!="float" && fullid!="string" && fullid!="bool"))
		fullid="$global::"+fullid;
	if(ast.symtab->find(fullid)==ast.symtab->end())
		return NULL;
	return &ast.symtab->at(fullid);
}

//shortid
bcSymbol* bcParser::getSymbol(std::string shortid,bcSymbol* sc)
{
	if(ast.symtab->find(getFullIdent(this,shortid,sc))==ast.symtab->end())
		return NULL;
	return &ast.symtab->at(getFullIdent(this,shortid,sc));
}

int bcParser::parse()
{
	lexer->nextToken();
	while(!lexer->done)
		parseStatement(this);
	return getError();
}

void parse::parseStatement(bcParser* par)
{	
	bcSymbol sym;
	par->addNode( bcParseNode(pn_statement) );
	switch(par->lexer->getToken()->type)
	{
		case tt_eof:
			return;
			break;
		case tt_scolon:
			//empty statement
			parseSColon(par);
			break;	
		case tt_int:case tt_string:case tt_float:case tt_bool:case tt_object:
			if(!par->noDecVar)
				parseDecVar(par);
			break;
		case tt_namespace:
			if(!par->noDecName)
				parseDecNamespace(par);
			break;
		case tt_function:
			if(!par->noDecFunc)
				parseDecFunc(par);
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
			sym = resolveIdent(par,consumeIdent(par));
			switch(sym.type)
			{
			case st_var:
			case st_function:
				parseFExp(par,sym);
				parseSColon(par);
			default:
				true;//error
				return;
			}
		default:
			break;			
	}
	par->parent();
}

void parse::parseDecNamespace(bcParser* par)
{
	//namespace keyword
	if(par->lexer->getToken()->type!=tt_namespace)
		return;	//error
	par->lexer->nextToken();
	par->noDecName=par->noDecVar=par->noDecFunc=false;

	//ident
	bcSymbol* sym=addDecIdent(par,st_namespace);
	if(sym->type==st_null)
		return;	//already used;	
	bcSymbol* oldScope=par->currentScope;

	//block
	par->currentScope = par->getSymbol(sym->fullident);
	parseBlock(par);
	par->currentScope=oldScope;

}

void parse::parseDecVar(bcParser* par)
{
	bcSymbol symt,*symi;
	par->addNode(bcParseNode(pn_vardec));

	//1. variables type
	switch(par->lexer->getToken()->type)
	{
	//basic types
	case tt_bool:	case tt_int:	case tt_float:	case tt_string:	
		par->addChild(bcParseNode(pn_type,*par->lexer->getToken()));		
		symt.datatype = par->getSymbol(par->lexer->getToken()->data)->ident;
		par->lexer->nextToken();
		break;	
	//user type
	case tt_ident:
		symt = parseIdent(par);
		if(symt.type != st_type)
			return;	//error redefinition
		symt.datatype = symt.fullident;
		par->addChild(bcParseNode(pn_type,symt.fullident));
		break;
	default:
		//error
		break;
	}

	//2. variable identifier
	switch(par->lexer->getToken()->type)
	{
	case tt_ident:
		symi=addDecIdent(par,st_var);
		par->getSymbol(symi->fullident)->datatype=symt.datatype;
		if(symi)
			par->ast.stackframes[par->ast.stackframes.size()-1].push_back(symi->fullident);
		else
			return;	//errors redef
		par->addChild(bcParseNode(pn_ident,symi->fullident));
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
		parseFExp(par);
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
	bcSymbol sym;
	bcFuncInfo fi;

	//function keyword
	switch(par->lexer->getToken()->type)
	{
	case tt_function:			
		par->addNode(bcParseNode(pn_funcdec,*par->lexer->getToken()));
		par->lexer->nextToken();
		par->currentFunc=&fi;
		break;
	default:
		//error
		break;
	}
	
	//type and ident
	switch(par->lexer->getToken()->type)
	{
	//basic types
	case tt_bool:	case tt_int:	case tt_string:	case tt_float:
		par->addChild(bcParseNode(pn_type,*par->lexer->getToken()));
		fi.datatype=par->lexer->getToken()->data;
		par->lexer->nextToken();
		break;

	case tt_ident:
		sym = parseIdent(par);
		if(sym.type == st_type)
		{
			fi.datatype=sym.fullident;
			par->addChild(bcParseNode(pn_type,sym.fullident));
		}
		else
		{
			//error unknown symbol
			return;
		}
		break;

		default:			
			break;			//error
	}

	switch(par->lexer->getToken()->type)
	{
	case tt_ident:
		sym = parseIdent(par);
		if(sym.type == st_null)		
		{
			sym.type = st_function;
			sym.datatype = fi.datatype;
			fi.ident = sym.ident; fi.fullident=sym.fullident;
			par->addSymbol(sym.fullident,&sym);
			par->currentScope = par->getSymbol(sym.fullident);
			par->ast.stackframes.push_back(std::vector<std::string>());
			//par->addChild(bcParseNode(pn_funcdec_ident,sym.fullident));
		}
		else
		{
			//paramlist must be different or its a redefinition
		}
		break;
	default:
		//error
		break;
	}

	//3. scolon or paramlist
	switch(par->lexer->getToken()->type)
	{
	case tt_scolon:
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
		par->noDecFunc=true;
		parseBlock(par);
		par->noDecFunc=false;
		break;
	default:
		break;
	}
	
	 //change scopes back 
	par->currentScope = oldScope;
	par->currentFunc=NULL;
	//pop stack frame
	fi.stackframe=par->ast.stackframes[par->ast.stackframes.size()-1];
	par->ast.stackframes.erase(par->ast.stackframes.end()-1);
	par->ast.functab->insert( std::make_pair( sym.fullident,fi) );
	par->parent();
}

void parse::parseParamList(bcParser* par)
{
	//get our opening parenthesis (
	if(par->lexer->getToken()->type!=tt_oparen)	
		return;	//error!
	par->lexer->nextToken();
	par->addNode(bcParseNode(pn_paramlist));	
	
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
	//semi colon 
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

	//if were currently declaring a function, or (currentFunc!=NULL), make a note of the body
	par->addNode(bcParseNode(pn_block));
	if(par->currentFunc)
		par->currentFunc->body[par->currentFunc->fullident]=par->getNode();

	//inner contents
	while(par->lexer->getToken()->type!=tt_cbrace)
		parseStatement(par);

	//closing brace
	par->lexer->nextToken();
	par->parent();
}

void parse::parseIf(bcParser* par)
{	
	bool blocks=false;
	//opening if
	if(par->lexer->getToken()->type!=tt_if)
		return;	//error
	par->lexer->nextToken();
	par->addNode(bcParseNode(pn_if));
	
	//conditional expression
	parseFExp(par);
	
	//true block
	if(par->lexer->getToken()->type==tt_obrace)
		blocks=true;
	if(blocks)
		parseBlock(par);
	else
		parseStatement(par);
	
	//optional elseif/else
	while(par->lexer->getToken()->type==tt_else || par->lexer->getToken()->type==tt_elseif)
	{
		par->addNode(bcParseNode(par->lexer->getToken()->type));
		par->lexer->nextToken();
		if(blocks)
			parseBlock(par);
		else
			parseStatement(par);
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
	//break keyword
	if(par->lexer->getToken()->type!=tt_break)
		return;	//error
	par->lexer->nextToken();
	par->addChild(bcParseNode(pn_break));
	
	//semi colon
	parseSColon(par);
}

void parse::parseContinue(bcParser* par)
{	
	//continue keyword
	if(par->lexer->getToken()->type!=tt_continue)
		return;	//error
	par->lexer->nextToken();
	par->addChild(bcParseNode(pn_continue));
	
	//semi colon
	parseSColon(par);
}

//nextToken til end of ident, return shortident
std::string parse::consumeIdent(bcParser* par)
{
	std::string id;
	bool exit=false;
	while(!exit)
		switch(par->lexer->getToken()->type)
		{
		case tt_ident:
		case tt_period:
		case tt_dcolon:
		case tt_colon:
			id+=par->lexer->getToken()->data;
			par->lexer->nextToken();
			break;
		default:
			exit=true;
		}
	return id;
}

bcParseNode parse::parseIdent(bcParser* par)
{
	std::string sident;		
	//get symbol, if any, consume ident in lexer
	bcSymbol sym = resolveIdent(par,sident = consumeIdent(par));

	bcParseNode pn(sym.type);
	pn.tokens.push_back(bcToken(tt_ident,sym.fullident));

	return pn;
}

//check shortident is a valid symbol in current scope
//shortid must be valid in currentScope
//null symbol returned if ident cant be found in any scope
//cleanup the return object
bcSymbol parse::resolveIdent(bcParser* par,std::string shortid)
{
	std::string buff,fullid;
	int ind;
	//return a null type symbol if the shortident doesnt resolve at any scope
	bcSymbol sym;	sym.type=st_null;	sym.ident=shortid;	
	sym.fullident=getFullIdent(par,shortid,par->currentScope);
	
	//deal with explicit identifiers (with scope/member prefix)
	if(isIdentExplicit(par,shortid))
	{
		if(par->getSymbol(shortid))
			return *par->getSymbol(shortid);
		return sym;
	}
	else
	{
		fullid=getFullIdent(par,shortid,par->currentScope);
		ind=fullid.size()-1;
	}

	//check all parent scopes as well, backwards to $global
	bcSymbol sc;
	while(ind>0)
		switch(fullid[ind])
		{
		//two steps pack if we get two colons
		case ':':
			if(ind>0 && fullid[ind-1]==':')
				--ind;	
		case '.':	//member
			--ind;
			sc = *par->getSymbol(fullid.substr(0,ind+1));	//get scope
			if(par->getSymbol(getFullIdent(par,shortid,&sc)))
				return *par->getSymbol(getFullIdent(par,shortid,&sc));
			break;
			default:
				--ind;
		}
	return sym;
}

//make a string from a given shortident and scope
std::string parse::getFullIdent(bcParser* par,std::string sident,bcSymbol* scope)
{
	if(!isIdentExplicit(par,sident))
		switch(scope->type)
		{
		case st_namespace:
				return scope->fullident+"::"+sident;
		case st_function:
			return scope->fullident+":"+sident;
		default:
			return "";
		}
	else
		return sident;//actually explicait
}

bool parse::isIdentExplicit(bcParser* par,std::string id)
{
	std::string shortid;
	bcSymbol sc;
	int ind=id.size()-1;
	while(ind>0)
		switch(id[ind])
		{
		//two steps pack if we get two colons
		case ':':
		case '.':	//member
			return true;
		default:
		--ind;
		}
	return false;
}

std::string parse::getShortIdent(std::string fullident)
{
	int ind=fullident.size()-1;
	while(ind>0)
	{
		if(fullident[ind] == ':' || fullident[ind] == '.')
			return fullident.substr(ind+1,fullident.size()-ind);
		ind--;
	}
	return fullident;
}

void parse::parseFuncCall(bcParser* par,bcSymbol sym)
{
	//ident must be a function symbol
	if(sym.type!=st_function)
		return;	//error
	else if(sym.type==st_null)
		return;	//undeclared ident
	par->addNode(bcParseNode(pn_funccall))->node->data.tokens.push_back(bcToken(sym.fullident));

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

void parse::parseAssignment(bcParser* par,bcSymbol sym)
{
	//ident already parsed
	par->addNode(bcParseNode(pn_assignment));
	//expression
	parseFExp(par,sym);
	//semi colon
	parseSColon(par);
	par->parent();
}

//full expression
void parse::parseFExp(bcParser* par)
{	
	par->addNode(bcParseNode(pn_exp));
	parseExp(par);
	par->parent();
}

//for when weve already consumed the identifier with parseIdent()
void parse::parseFExp(bcParser* par,bcSymbol sym)
{	
	//get the node of the previously parsed ident, and move it into this expression
	tree<bcParseNode>::iterator id=--*par->getNode();
	par->addNode(bcParseNode(pn_exp));
	par->addChild(*id);
	par->ast.tree->erase(id);

	bcToken op;
	while(par->lexer->getToken())
	{
		op=*par->lexer->getToken();
		switch(op.type)
		{		
		case tt_less:	case tt_greater:	case tt_lessequal:			case tt_greaterequal:
		case tt_equal:	case tt_notequal:	case tt_logand:		case tt_logor:		case tt_assign:
			par->addChild(bcParseNode(op.type))->node->data.tokens.push_back(op);
			par->lexer->nextToken();
			break;
		default:
			return;
		}
		parseSubExp(par);
	}
	par->parent();
}


lex::bcToken parse::parseExp(bcParser* par)
{
	bcToken oper1,oper2,op;
	oper1=parseSubExp(par);
	while(par->lexer->getToken())
	{
		op=*par->lexer->getToken();
		switch(op.type)
		{		
		case tt_less:	case tt_greater:	case tt_lessequal:			case tt_greaterequal:
		case tt_equal:	case tt_notequal:	case tt_logand:		case tt_logor:		case tt_assign:
			par->addChild(bcParseNode(op.type))->node->data.tokens.push_back(op);;
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
		switch(op.type)
		{		
		case tt_plus:	case tt_minus: 
			par->addChild(bcParseNode(op.type))->node->data.tokens.push_back(op);
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
		switch(op.type)
		{		
		case tt_div:	case tt_mult:	case tt_pow:	case tt_mod:	
			par->addChild(bcParseNode(op.type))->node->data.tokens.push_back(op);
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
		//	(
		par->parenCount++;
		par->addChild(bcParseNode(tt_oparen))->node->data.tokens.push_back(oper1);	
		par->lexer->nextToken();

		//parse inner expression
		parseExp(par);							
		if(par->lexer->getToken()->type!=tt_cparen)
			return NULL;	//error
		
		//	)
		par->parenCount--;
		par->addChild(bcParseNode(tt_cparen))->node->data.tokens.push_back(oper1);
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
		case st_namespace:
			//must be var/function
			return NULL;
			break;
		default:
			//error unknown identifier
			return NULL;
			break;
		}
		break;
	case tt_intlit:
	case tt_strlit:	
	case tt_fltlit:
	case tt_true:
	case tt_false:
		par->addChild(bcParseNode(oper1.type))->node->data.tokens.push_back(oper1);
		par->lexer->nextToken();
		break;
	default:
		return NULL;	//error
	}	
	return oper1;
}

//adds an ident if it doesnt exist locally
//parse, checks ident already exists in any scope, checks locally too if it does
//if it doesnt exist, its ok to make the declaration and adds it to symbol table etc
bcSymbol* parse::addDecIdent(bcParser* par,bcSymbolType ty)
{	
	bcSymbol sym = resolveIdent(par,consumeIdent(par));
	if(sym.type != st_null)
		//check only this scope, in case we previously found another valid identifier
		if(	par->getSymbol(getFullIdent(par,sym.ident,par->currentScope))
		&&par->getSymbol(getFullIdent(par,sym.ident,par->currentScope))->type!=st_null)
			return NULL;	//redefinition
		else 
			sym.fullident=getFullIdent(par,sym.ident,par->currentScope);	
	//no ident of that name in current scope, declare variable
	sym.type=ty;
	par->addSymbol(&sym);
	return par->getSymbol(sym.fullident);
}