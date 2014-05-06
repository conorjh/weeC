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
using bc::parse::bcExpression;
using bc::bcErrorCode;

bcSymbol::bcSymbol()
{
	ident=fullident=datatype="";
	type=st_null;
	isConst=isArray=false;
}
bcParseNode::bcParseNode()
{

}
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
	case st_type:
		//type=pn_typeident;
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
	error=ec_null;
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

void bcParser::setError(bcErrorCode ec,int x,int y,std::string s)
{
	error=ec;errorL=y;
	errorC=x;errorS=s;
}

void bcParser::setError(bcErrorCode ec,std::string s)
{
	error=ec;
	errorL=lexer->getToken()->y;
	errorC=lexer->getToken()->x;
	errorS=s;
}

unsigned int bcParser::getError()
{
	return error;
}

tree<bcParseNode>::iterator* bcParser::getNode()
{
	return &pindex;
}

tree<bcParseNode>::iterator* bcParser::prevNode()
{
	return &--pindex;
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
	while(!lexer->done && !error)
		parseStatement(this);
	return getError();
}

void parse::parseStatement(bcParser* par)
{	
	bcExpression ex;
	bcParseNode pni;
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
		case tt_const:	case tt_int:case tt_string:case tt_float:case tt_bool:case tt_object:
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
			pni = parseIdent(par);
			if(par->getSymbol(pni.tokens.at(0).data))
				sym = *par->getSymbol(pni.tokens.at(0).data);
			switch(sym.type)
			{
			case st_function:
				parseFuncCall(par,pni);
				break;
			case st_var:
				ex=parseFExp(par,pni);
				parseSColon(par);
				break;
			case st_null:
			default:
				//unknown identifier
				return par->setError(ec_p_undeclaredsymbol,pni.tokens.at(0).y,pni.tokens.at(0).x,sym.fullident);
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
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
	par->lexer->nextToken();
	par->noDecName=par->noDecVar=par->noDecFunc=false;
	par->addNode(bcParseNode(pn_namespacedec));

	//ident
	bcSymbol* sym=addIdentDec(par,st_namespace);
	if(sym->type==st_null)
		return;	//already used;	handled in addIdentDec()
	bcSymbol* oldScope=par->currentScope;

	//block
	par->currentScope = par->getSymbol(sym->fullident);
	parseBlock(par);
	par->currentScope=oldScope;

}

void parse::parseDecVar(bcParser* par)
{
	bool setToConst=false;
	bcParseNode pni;
	bcSymbol symt,*symi;
	par->addNode(bcParseNode(pn_vardec));

	//check for const
	if(par->lexer->getToken()->type==tt_const)
	{
		setToConst=true;
		par->lexer->nextToken();
	}

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
		pni = parseIdent(par);
		symt = *par->getSymbol(pni.tokens.at(0).data);

		if(symt.type != st_type)
			return	par->setError(ec_p_invalidsymbol,pni.tokens.at(0).data);			
		
		symt.datatype = symt.fullident;
		par->addChild(bcParseNode(pn_type,symt.fullident));
		break;
	default:
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
	}

	//2. variable identifier
	switch(par->lexer->getToken()->type)
	{
	case tt_ident:
		symi=addIdentDec(par,st_var);
		par->getSymbol(symi->fullident)->datatype=symt.datatype;
		par->getSymbol(symi->fullident)->isArray=symt.isArray;
		par->getSymbol(symi->fullident)->isConst=setToConst;
		if(symi)
			par->ast.stackframes[par->ast.stackframes.size()-1].push_back(symi->fullident);
		else
			return;	//errors redef
		par->addChild(bcParseNode(pn_ident,symi->fullident));
		break;
	default:
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
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
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
	}
	par->parent();
}

void parse::parseDecFunc(bcParser* par)
{
	bcParseNode pni;
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
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
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
		pni = parseIdent(par);
		sym = *par->getSymbol(pni.tokens.at(0).data);
		if(sym.type == st_type)
		{
			fi.datatype=sym.fullident;
			par->addChild(bcParseNode(pn_type,sym.fullident));
		}
		else
		{
			//error unknown symbol
			return par->setError(ec_p_undeclaredsymbol,par->lexer->getToken()->data);
		}
		break;

		default:			
			break;			//error
	}

	switch(par->lexer->getToken()->type)
	{
	case tt_ident:
		sym = resolveIdent(par,consumeIdent(par));
		if(sym.type == st_null)		
		{
			sym.type = st_function;
			sym.datatype = fi.datatype;
			fi.ident = sym.ident; fi.fullident=sym.fullident;
			par->addSymbol(sym.fullident,&sym);
			par->currentScope = par->getSymbol(sym.fullident);
			par->ast.stackframes.push_back(std::vector<std::string>());
			par->addChild(bcParseNode(pn_funcident,sym.fullident));
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
		parseParamListDec(par);
		break;
	default:
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
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

void parse::parseParamListCall(bcParser* par,bcSymbol* id)
{
	bcParamList cpl;
	bcParseNode pni;
	bcSymbol sym;
	bcExpression ex;

	//get our opening parenthesis (
	if(par->lexer->getToken()->type!=tt_oparen)	
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
	par->lexer->nextToken();
	par->addNode(bcParseNode(pn_paramlist));	
	
	//loop thru all the parameters in the func call
	while(par->lexer->getToken()->type!=tt_cparen)
	{
		//broke here
		if(par->lexer->getToken()->type!=tt_comma)
		{			
			//parse the expression
			ex=parseFExp(par);
			cpl.params.push_back(ex.datatype);
			par->addNode(pni);			
		}
		else
		{
			//consume comma 
			par->lexer->nextToken();	
		}
	}
	//check this signature exists within the given idents symbol table signatures
	if(!checkForOverload(par,&cpl,id))
		return par->setError(ec_p_badparams,par->lexer->getToken()->data);

	//consume cparen
	par->lexer->nextToken();
	par->parent();
}

void parse::parseParamListDec(bcParser* par)
{
	bcParseNode pni;
	bcParamList pl;
	bcSymbol symt,symi;

	//get our opening parenthesis (
	if(par->lexer->getToken()->type!=tt_oparen)	
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
	par->lexer->nextToken();
	par->addNode(bcParseNode(pn_decparamlist));	
	
	//loop thru all the parameters in the func call
	while(par->lexer->getToken()->type!=tt_cparen)
		if(par->lexer->getToken()->type==tt_comma)
		{
			par->lexer->nextToken();
		}
		else
		{
			//get stick the string we get into symbol ta
			switch(par->lexer->getToken()->type)
			{			
			case tt_bool:	case tt_int:	case tt_string:	case tt_float:
				symt=*par->getSymbol(par->lexer->getToken()->data);
				pl.params.push_back(symt.fullident);
				par->addChild(bcParseNode(pn_type,*par->lexer->getToken()));
				par->lexer->nextToken();
				break;
			case tt_ident:
				pni = parseIdent(par);
				symt = *par->getSymbol(pni.tokens.at(0).data);
				if(symt.type != st_type)
					return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);	//must be a type
				pl.params.push_back(symt.fullident);
				par->addChild(bcParseNode(pn_type,symt.fullident));
			}

			//ident
			symi=resolveIdent(par,consumeIdent(par));
			if(symi.type!=st_null)
				return par->setError(ec_p_redefinition,symi.ident);
			symi.type=st_var;
			symi.datatype=symt.datatype;
			par->addSymbol(symi.fullident,&symi);
			par->addChild(bcParseNode(symi.type));
		}	
	
	//consume cparen, add to current bcFuncInfo
	par->currentFunc->sigs.insert(std::make_pair(getMethodStringSignature(&pl),pl));
	par->lexer->nextToken();
	par->parent();
}


void parse::parseSColon(bcParser* par)
{
	//semi colon 
	if(par->lexer->getToken()->type!=tt_scolon)
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
	par->lexer->nextToken();
}

void parse::parseBlock(bcParser* par)
{
	//opening brace
	if(par->lexer->getToken()->type!=tt_obrace)
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
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
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
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
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
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
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
	par->lexer->nextToken();
	par->addChild(bcParseNode(pn_break));
	
	//semi colon
	parseSColon(par);
}

void parse::parseContinue(bcParser* par)
{	
	//continue keyword
	if(par->lexer->getToken()->type!=tt_continue)
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
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

//get symbol, if any, consume ident in lexer
bcParseNode parse::parseIdent(bcParser* par)
{
	std::string sident;		
	bcSymbol sym = resolveIdent(par,sident = consumeIdent(par));
	bcParseNode pn(sym.type);
	pn.tokens.push_back(bcToken(tt_ident,sym.fullident));

	//parse array index or func call
	if(sym.isArray)
		parseArrayIndex(par,&pn);

	return pn;
}

void parse::parseArrayIndex(bcParser* par,bcParseNode* pn)
{
	bcExpression ex;
	switch(par->lexer->getToken()->type)
	{
	case tt_obracket:
		par->lexer->nextToken();
		
		/*
		ex = parseFExp(par);
		if(ex.datatype != "int")
			//error, array ref must be int
			return;
		*/
		//just check for int now
		pn->tokens.push_back(*par->lexer->getToken());

		par->lexer->nextToken();

		//consume closing bracket
		par->lexer->nextToken();
	default:
		//error
		break;
	}
}

//check shortident is a valid symbol in current scope
//shortid must be valid in currentScope
//null symbol returned if ident cant be found in any scope
//cleanup the return object
bcSymbol parse::resolveIdent(bcParser* par,std::string shortid)
{
	//return a null type symbol if the shortident doesnt resolve at any scope
	bcSymbol sym;	sym.type=st_null;	sym.ident=shortid;	
	sym.fullident=getFullIdent(par,shortid,par->currentScope);
	std::string buff,fullid;
	int ind;
	
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
	
	return sident;//actually explicait
}

//check whether a given string contains any scope or members ie (main:arg1 or std::string)
bool parse::isIdentExplicit(bcParser* par,std::string id)
{
	std::string shortid;
	bcSymbol sc;
	int ind=id.size()-1;

	while(ind>0)
		switch(id[ind])
		{
		//two steps back if we get two colons
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

void parse::parseFuncCall(bcParser* par,bcParseNode pn)
{
	//ident must be a declared function symbol
	bcSymbol sym = *par->getSymbol(pn.tokens.at(0).data);
	
	if(sym.type!=st_function)
		return par->setError(ec_p_invalidsymbol,par->lexer->getToken()->data);
	else if(sym.type==st_null)
		return par->setError(ec_p_undeclaredsymbol,par->lexer->getToken()->data);
	par->addNode(bcParseNode(pn_funccall))->node->data.tokens.push_back(bcToken(sym.fullident));

	//params
	parseParamListCall(par,par->getSymbol(sym.fullident));
	
	//semi colon
	parseSColon(par);
	par->parent();
}

void parse::parseWhile(bcParser* par)
{
	//while keyword
	if(par->lexer->getToken()->type!=tt_while)
		return par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
	par->lexer->nextToken();
	par->addChild(bcParseNode(pn_while));
	
	//condition
	parseFExp(par);
	
	//block to execute
	parseBlock(par);
}

void parse::parseAssignment(bcParser* par,bcParseNode pn)
{
	//ident already parsed
	bcSymbol sym = *par->getSymbol(pn.tokens.at(0).data);
	par->addNode(bcParseNode(pn_assignment));
	//expression
	parseFExp(par,pn);
	//semi colon
	parseSColon(par);
	par->parent();
}

//full expression - only this method makes the pn_exp parse node
bcExpression parse::parseFExp(bcParser* par)
{	
	tree<bcParseNode>::iterator* exnode;
	exnode=par->addNode(bcParseNode(pn_exp));
	bcExpression ex = parseExp(par);
	ex.node=exnode;
	par->parent();
	return ex;
}

//for when weve already consumed the identifier with parseIdent()
bcExpression parse::parseFExp(bcParser* par,bcParseNode id)
{	
	bcExpression ex;ex.isConst=true;
	ex.node=par->addNode(bcParseNode(pn_exp));
	//taken from parseFactor
	bcSymbol sym = *par->getSymbol(id.tokens.at(0).data);
	switch(sym.type)
	{	
	case st_var:case st_function:
		ex.datatype=sym.datatype;
		break;
	default:
		par->setError(ec_p_invalidsymbol,par->lexer->getToken()->data);
		return ex;
	}
	par->addChild(id);

	bcToken op;
	while(par->lexer->getToken())
	{
		op=*par->lexer->getToken();
		switch(op.type)
		{		
		case tt_less:	case tt_greater:	case tt_lessequal:	case tt_greaterequal:
		case tt_equal:	case tt_notequal:	case tt_logand:		case tt_logor:		case tt_assign:
			par->addChild(bcParseNode(op.type))->node->data.tokens.push_back(op);
			par->lexer->nextToken();
			break;
		default:	
			par->parent();
			return ex;
		}
		parseSubExp(par,&ex);
	}
	par->parent();
	return ex;
}


bcExpression parse::parseExp(bcParser* par)
{
	bcExpression ex;ex.isConst=true;
	bcToken oper1,oper2,op;
	oper1=parseSubExp(par,&ex);
	ex.datatype = getDatatype(par,oper1);
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
			//function call or one sided expression (ie not a boolean or assignment)
			return ex;
		}
		oper2 = parseSubExp(par,&ex);
		//check this operation is valid for the types we have
		//if(!checkOperandTypes(par,oper1,op,oper2))
		{
			//error
		}
	}
	return ex;
}

lex::bcToken parse::parseSubExp(bcParser* par,bcExpression* ex)
{
	bcToken oper1,oper2,op;
	bcParseNode pn;
	oper1=parseTerm(par,ex);
	while(par->lexer->getToken())
	{
		op=*par->lexer->getToken();
		switch(op.type)
		{		
		case tt_plus:	case tt_minus:
			pn=bcParseNode(op.type);
			pn.tokens.push_back(op);
			par->addChild(pn);
			par->lexer->nextToken();
			break;
		default:
			return oper1;
		}
		oper2 = parseTerm(par,ex);
	}
	return oper1;
}

lex::bcToken parse::parseTerm(bcParser* par,bcExpression* ex)
{
	bcToken oper1,oper2,op;
	bcParseNode pn;
	oper1=parseFactor(par,ex);
	while(par->lexer->getToken())
	{
		op=*par->lexer->getToken();
		switch(op.type)
		{		
		case tt_div:	case tt_mult:	case tt_pow:	case tt_mod:
			pn=bcParseNode(op.type);
			pn.tokens.push_back(op);
			par->addChild(pn);
			par->lexer->nextToken();
			break;
		default:
			return oper1;
		}
		oper2 = parseFactor(par,ex);
	}
	return oper1;
}

bcToken parse::parseFactor(bcParser* par,bcExpression* ex)
{
	bcExpression ex2;
	bcParseNode pn,pni;
	bcSymbol sym;sym.type=st_null;
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
		pn=bcParseNode(oper1.type);
		pn.tokens.push_back(oper1);
		par->addChild(pn);
		par->lexer->nextToken();

		//parse inner expression, marking a change to non-constness if needed (isConst true by default)
		if(parseExp(par).isConst==false && ex->isConst)
			ex->isConst=false;
		if(par->lexer->getToken()->type!=tt_cparen)
		{
			par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
			return oper1;	//error
		}
		
		//	)
		par->parenCount--;
		pn=bcParseNode(pn_cparen);
		pn.tokens.push_back(*par->lexer->getToken());
		par->addChild(pn);
		par->lexer->nextToken();
		break;

	case tt_cparen:
		//par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
		return oper1;

	case tt_ident:
		pni = parseIdent(par);
		if(par->getSymbol(pni.tokens.at(0).data))
			sym = *par->getSymbol(pni.tokens.at(0).data);
		switch(sym.type)
		{
		case st_function:
			parseFuncCall(par,pni);		//parse as func call, passing the free floating bcParseNode we made
			if(!sym.isConst)	ex->isConst=false;
			break;
		case st_var:
			if(!sym.isConst)	ex->isConst=false;
			if(sym.isArray)		parseArrayIndex(par,&pni);	//the [] subscript
			par->addChild(pni);
			break;
		case st_namespace:
			par->setError(ec_p_invalidsymbol,par->lexer->getToken()->data);
			return oper1;
		default:
			par->setError(ec_p_undeclaredsymbol,par->lexer->getToken()->data);
			return oper1;
			break;
		}
		break;

	case tt_intlit:
	case tt_strlit:	
	case tt_fltlit:
	case tt_true:
	case tt_false:
		//ex.isConst = true by default
		pn=bcParseNode(oper1.type);
		pn.tokens.push_back(oper1);
		par->addChild(pn);
		par->lexer->nextToken();
		break;

	default:
		//par->setError(ec_p_unexpectedtoken,par->lexer->getToken()->data);
		return oper1;	//error
	}	
	return oper1;
}

//assumes a valid set of bcSymbol pointaz
std::string parse::getMethodStringSignature(bcParamList* pl)
{
	std::string out;
	for(int t=0;t<pl->params.size();++t)
		out += pl->params[t] + ",";
	return out;
}

//checks whether a given ParamList has been declared for the given symbol
bool parse::checkForOverload(bcParser* par,bcParamList* pl,bcSymbol* id)
{
	//get function info for the given ident
	bcFuncInfo fi = par->ast.functab->at(id->fullident);
	//check the given paramlist is in there somewhere
	return (fi.sigs.find(getMethodStringSignature(pl))!=fi.sigs.end());
}

//adds an ident if it doesnt exist locally
//parse, checks ident already exists in any scope, checks locally too if it does
//if it doesnt exist, its ok to make the declaration and adds it to symbol table etc
//returns null if ident already exists
bcSymbol* parse::addIdentDec(bcParser* par,bcSymbolType ty)
{	
	bcSymbol sym = resolveIdent(par,consumeIdent(par));
	if(sym.type != st_null)
		//check only this scope, in case we previously found another valid identifier
		if(	par->getSymbol(getFullIdent(par,sym.ident,par->currentScope))
		&&par->getSymbol(getFullIdent(par,sym.ident,par->currentScope))->type!=st_null)
		{
			par->setError(ec_p_redefinition,sym.ident);
			return NULL;	//redefinition
		}
		else
		{
			sym.fullident=getFullIdent(par,sym.ident,par->currentScope);	
		}
	//no ident of that name in current scope, declare variable
	sym.type=ty;
	par->addSymbol(&sym);
	return par->getSymbol(sym.fullident);
}

bool parse::checkOperandTypes(bcParser* par,bcToken oper1, bcToken op, bcToken oper2)
{
	bcSymbol sym;
	bcToken opers[2];
	std::string oper_dt[2];
	opers[0]=oper1;opers[1]=oper2;

	//get datatype of token as a string
	for(int t=0;t<2;++t)
		if(opers[t].type==tt_ident)
		{
			sym=*par->getSymbol(opers[t].data);
			switch(sym.type)
			{
			case st_var:	case st_function:
				oper_dt[t]=sym.datatype;
				break;
			default:
				//error
				return false;
			}
		}
		else
		{
			switch(opers[t].type)
			{
			case tt_intlit:	case tt_strlit:	case tt_fltlit:
				oper_dt[t]=par->getSymbol(opers[t].data)->datatype;	break;
			case tt_true:	case tt_false:
				oper_dt[t]=par->getSymbol("bool")->datatype;	break;
			}
		}

	//banging massive table
	switch(op.type)
	{
		//float/int
		case tt_div:	case tt_mult:	case tt_pow:	
		case tt_mod:	case tt_minus: 	case tt_plus:
			if(oper_dt[0]=="string"||oper_dt[0]=="bool"	
			||oper_dt[1]=="string"||oper_dt[1]=="bool")
				return false;
		//string/bool
		case tt_less:	case tt_greater:	case tt_lessequal:	case tt_greaterequal:
		case tt_equal:	case tt_notequal:	case tt_logand:		case tt_logor:		case tt_assign:
			break;
	}

	return false;
}

std::string parse::getDatatype(bcParser* par,bcToken tk)
{
	switch(tk.type)
	{
	case tt_intlit:
		return par->getSymbol("int")->datatype;
	case tt_strlit:
		return par->getSymbol("string")->datatype;
	case tt_fltlit:
		return par->getSymbol("float")->datatype;
	case tt_true:
	case tt_false:
		return par->getSymbol("bool")->datatype;
	case tt_ident:
		if(par->getSymbol(tk.data))
			return par->getSymbol(tk.data)->datatype;
		return "";
	default:
		return "";
	}
	return "";
}

int parse::getPrecedence(lex::bcToken tokin)
{
	switch(tokin.type)
	{
	case tt_assign:
		return 15;

	case tt_logor:
		return 14;

	case tt_logand:
		return 13;

	case tt_equal:
	case tt_notequal:
		return 12;

	case tt_greater:
	case tt_less:
	case tt_lessequal:
	case tt_greaterequal:
		return 10;

	case tt_mult:
	case tt_div:
		return 8;

	case tt_plus:
	case tt_minus:
		return 6;

	case tt_lognot:
		return 4;

	case tt_cparen:
		return -1;

	default:
		return 0;
	}
}

int parse::isOperator(lex::bcToken tokin)
{
	switch(tokin.type)
	{
	case tt_assign:
	case tt_logor:	case tt_logand:	case tt_equal:	case tt_notequal:
	case tt_greater:case tt_less:	case tt_lessequal:	case tt_greaterequal:
	case tt_mult:	case tt_div:	case tt_plus:	case tt_minus:	case tt_lognot:
		return true;
	}
	return false;
}