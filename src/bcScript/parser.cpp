#include "parser.h"
#include "util.h"

//using namespace bc::parse;
using namespace std;
using namespace bc;
using namespace bc::lex;
using namespace bc::util;
using namespace bc::parse;

bcExpression::bcExpression()
{
	this->dataType = "";
	this->isArray = false;
	this->isAssign = false;
	this->isBool = false;
	this->isConst = false;
	this->isFunc = false;
	//this->node=null;
}

bcSymbol::bcSymbol()
{
	ident = fullIdent = dataType = "";
	type = st_null;
	isStatic = isConst = isArray = false;
}

bcParseNode::bcParseNode()
{

}

bcParseNode::bcParseNode(bcSymbolType t)
{
	switch (t)
	{
	case st_null:
		type = pn_null;
		break;
	case st_var:
		type = pn_varident;
		break;
	case st_function:
		type = pn_funcident;
		break;
	case st_namespace:
		type = pn_namespaceident;
		break;
	case st_type:
		//type=pn_typeident;
		break;
	}
}

bcParseNode::bcParseNode(bcTokenType t)
{
	switch (t)
	{
	case tt_greater:
		type = pn_greater;	break;
	case tt_less:
		type = pn_less;	break;
	case tt_equal:
		type = pn_equal;	break;
	case tt_notequal:
		type = pn_notequal;	break;
	case tt_greaterequal:
		type = pn_greaterequal;	break;
	case tt_lessequal:
		type = pn_lessequal;	break;
	case tt_lognot:
		type = pn_lognot;	break;
	case tt_logor:
		type = pn_logor;	break;
	case tt_logand:
		type = pn_logand;	break;
	case tt_incr:
		type = pn_incr;	break;
	case tt_decr:
		type = pn_decr;	break;
	case tt_plusassign:
		type = pn_plusassign;	break;
	case tt_minusassign:
		type = pn_minusassign;	break;
	case tt_multassign:
		type = pn_multassign;	break;
	case tt_divassign:
		type = pn_divassign;	break;
	case tt_intlit:
		type = pn_intlit;	break;
	case tt_strlit:
		type = pn_strlit;	break;
	case tt_fltlit:
		type = pn_fltlit;	break;
	case tt_ident:
		type = pn_ident;	break;
	case tt_minus:
		type = pn_minus;	break;
	case tt_plus:
		type = pn_plus;	break;
	case tt_div:
		type = pn_div;	break;
	case tt_mult:
		type = pn_mult;	break;
	case tt_mod:
		type = pn_mod;	break;
	case tt_expo:
		type = pn_expo;	break;
	case tt_assign:
		type = pn_assign;	break;
	case tt_oparen:
		type = pn_oparen;	break;
	case tt_cparen:
		type = pn_cparen;	break;
	default:
		type = pn_null;
	}

}

void bcParser::startup()
{
	sOffset = parenCount = 0;
	error = ec_null;
	currentParamList = nullptr;
	currentFunc = nullptr;
	currentScope = nullptr;
	noDecVar = noDecFunc = noDecName = false;

	//tree
	ast.tree = new tree<bcParseNode>(bcParseNode(pn_head));
	pindex = ast.tree->begin();

	//symbol table
	ast.stackFrames.push_back(std::vector<string>());	//global stackframe
	ast.funcTab = new std::unordered_map<string, bcFuncInfo>();
	ast.symTab = new std::unordered_map<string, bcSymbol>();

	//add global symbols
	bcSymbol sym;	sym.fullIdent = sym.ident = "$global";	sym.type = st_namespace;
	addSymbol(sym.ident, &sym); currentScope = getSymbol("$global");
	//basic type symbol entries

	sym.dataType = sym.fullIdent = sym.ident = "int";		sym.type = st_type; 	addSymbol(sym.ident, &sym);
	sym.dataType = sym.fullIdent = sym.ident = "float";		sym.type = st_type; 	addSymbol(sym.ident, &sym);
	sym.dataType = sym.fullIdent = sym.ident = "string";	sym.type = st_type; 	addSymbol(sym.ident, &sym);
	sym.dataType = sym.fullIdent = sym.ident = "bool";		sym.type = st_type;		addSymbol(sym.ident, &sym);
}

void bcParser::shutdown()
{

}

void bcParser::clear()
{
	ast.symTab->clear();
	ast.tree->clear();
	ast.stackFrames.clear();
	ast.constTab.clear();
	lexer->clear();
}

void bcParser::setError(bcErrorCode ec, int x, int y, string s)
{
	error = ec; errorL = y;
	errorC = x; errorS = s;
}

void bcParser::setError(bcErrorCode ec, string s)
{
	error = ec;
	errorL = lexer->getToken()->y;
	errorC = lexer->getToken()->x;
	errorS = s;
}

unsigned int bcParser::getError()
{
	return error;
}

tree<bcParseNode>::iterator bcParser::getNode()
{
	return pindex;
}

tree<bcParseNode>::iterator bcParser::prevNode()
{
	return --pindex;
}

tree<bcParseNode>::iterator bcParser::addNode(bcParseNode pn)
{
	pindex = ast.tree->append_child(pindex, pn);
	return pindex;
}

tree<bcParseNode>::iterator bcParser::addChild(bcParseNode pn)
{
	ast.tree->append_child(pindex, pn);
	return ast.tree->child(pindex, ast.tree->number_of_children(pindex) - 1);
}

void bcParser::parent()
{
	pindex = ast.tree->parent(pindex);
}

bool bcParser::addSymbol(bcSymbol* sym)
{
	return addSymbol(sym->fullIdent, sym);
}

bool bcParser::addSymbol(string id, bcSymbol* sym)
{
	if (ast.symTab->find(id) != ast.symTab->end())
		return false;	//already exists
	ast.symTab->insert(std::make_pair(id, *sym));
	return true;
}

bcSymbol* bcParser::getSymbol(string fullid)
{
	//add global so coder doesnt have to
	if (fullid.size() && fullid.substr(0, 9) != "$global::" && (fullid != "$global" && fullid != "int" && fullid != "float" && fullid != "string" && fullid != "bool"))
		fullid = "$global::" + fullid;
	if (ast.symTab->find(fullid) == ast.symTab->end())
		return nullptr;
	return &ast.symTab->at(fullid);
}

//shortid
bcSymbol* bcParser::getSymbol(string shortid, bcSymbol* sc)
{
	if (ast.symTab->find(getFullIdent(this, shortid, sc)) == ast.symTab->end())
		return nullptr;
	return &ast.symTab->at(getFullIdent(this, shortid, sc));
}

int bcParser::parse()
{
	//start the lexer off
	lexer->nextToken();

	//parse til we cant parse no mo
	while (!lexer->done && !error)
		parseStatement(this);

	//return any errors
	return getError();
}

void parse::parseStatement(bcParser* p_par)
{
	bcExpression ex;
	bcParseNode pni;
	bcSymbol sym;

	p_par->addNode(bcParseNode(pn_statement));

	switch (p_par->lexer->getToken()->type)
	{
	case tt_eof:
		return;
		break;

	case tt_scolon:
		//empty statement
		parseSColon(p_par);
		break;

		//treat literals as the start of an expression
	case tt_strlit:	case tt_intlit:	case tt_fltlit:
	case tt_true:	case tt_false:
		parseFExp(p_par);
		break;

	case tt_const:	case tt_int:case tt_string:case tt_float:case tt_bool:case tt_object:
		if (!p_par->noDecVar)
			parseDecVar(p_par);
		break;
	case tt_namespace:
		if (!p_par->noDecName)
			parseDecNamespace(p_par);
		break;
	case tt_function:
		if (!p_par->noDecFunc)
			parseDecFunc(p_par);
		break;
	case tt_if:
		parseIf(p_par);
		break;
	case tt_while:
		parseWhile(p_par);
		break;
	case tt_continue:
		parseContinue(p_par);
		break;
	case tt_break:
		parseBreak(p_par);
		break;
	case tt_return:
		parseReturn(p_par);
		break;
	case tt_ident:
		pni = parseIdent(p_par);
		if (p_par->getSymbol(pni.tokens.at(0).data))
			sym = *p_par->getSymbol(pni.tokens.at(0).data);
		switch (sym.type)
		{
		case st_function:
			parseFuncCall(p_par, pni);
			break;
		case st_var:
			if (sym.isArray)
				parseArrayIndex(p_par, &pni);
			ex = parseFExp(p_par, pni);
			parseSColon(p_par);
			break;

		case st_null:
		default:
			//unknown identifier
			return p_par->setError(ec_p_undeclaredsymbol, pni.tokens.at(0).y, pni.tokens.at(0).x, sym.fullIdent);
		}
	default:
		break;
	}
	p_par->parent();
}

void parse::parseDecNamespace(bcParser* p_par)
{
	//namespace keyword
	if (p_par->lexer->getToken()->type != tt_namespace)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->noDecName = p_par->noDecVar = p_par->noDecFunc = false;
	p_par->addNode(bcParseNode(pn_namespacedec));

	//ident
	bcSymbol* sym = addIdentDec(p_par, st_namespace);
	if (sym->type == st_null)
		return;	//already used;	handled in addIdentDec()
	bcSymbol* oldScope = p_par->currentScope;

	//block
	p_par->currentScope = p_par->getSymbol(sym->fullIdent);
	parseBlock(p_par);
	p_par->currentScope = oldScope;

}

void parse::parseDecVar(bcParser* p_par)
{
	int ix, iy;
	bool setToConst = false;
	bcParseNode pnt, pni, pnv;
	bcSymbol symt, *symi;
	bcExpression ex;
	p_par->addNode(bcParseNode(pn_vardec));

	//0. check for const
	if (p_par->lexer->getToken()->type == tt_const)
	{
		setToConst = true;
		p_par->lexer->nextToken();
	}

	//1. variable's type	(int, string etc)
	pnt.tokens.push_back(*p_par->lexer->getToken());
	switch (p_par->lexer->getToken()->type)
	{
		//basic types
	case tt_bool:	case tt_int:	case tt_float:	case tt_string:
		p_par->addChild(bcParseNode(pn_type, *p_par->lexer->getToken()));
		symt.dataType = p_par->getSymbol(p_par->lexer->getToken()->data)->ident;
		p_par->lexer->nextToken();
		break;

		//user type
	case tt_ident:
		//parse and save the ident
		pni = parseIdent(p_par);
		symt = *p_par->getSymbol(pni.tokens.at(0).data);
		//if it ent a type, error!
		if (symt.type != st_type)
			return	p_par->setError(ec_p_invalidsymbol, pni.tokens.at(0).data);
		//add the node
		symt.dataType = symt.fullIdent;
		p_par->addChild(pnt = bcParseNode(pn_type, symt.fullIdent));
		break;
	default:
		//no cigar
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	}

	//2. variable identifier	
	switch (p_par->lexer->getToken()->type)
	{
	case tt_ident:
		ix = p_par->lexer->getToken()->x;
		iy = p_par->lexer->getToken()->y;	//for error reporting porpoises
		//add symbol
		symi = addIdentDec(p_par, st_var);
		p_par->getSymbol(symi->fullIdent)->dataType = symt.dataType;
		p_par->getSymbol(symi->fullIdent)->isConst = setToConst;
		if (!(p_par->getSymbol(symi->fullIdent)->isArray = symt.isArray))
			p_par->getSymbol(symi->fullIdent)->size = 1;
		//if(p_par->currentScope->type==st_namespace)
		//p_par->currentScope->offset += getTypeSize(symi);
		if (symi)
			p_par->ast.stackFrames[p_par->ast.stackFrames.size() - 1].push_back(symi->fullIdent);
		else
			return;	//errors redef
		p_par->addChild(bcParseNode(pn_ident, symi->fullIdent));
		break;
	default:
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	}

	//3. (optional) array subscripts
	if (p_par->lexer->getToken()->type == tt_obracket)
	{
		//consume opening bracket
		p_par->lexer->nextToken();

		//adjust symbol table entry
		p_par->getSymbol(symi->fullIdent)->isArray = true;
		//parse expression; type must convert to int, and be const
		ex = parseFExp(p_par);
		if (ex.dataType != "int")
			return p_par->setError(ec_p_nonintsubscript, ix, iy, "Array subscript must be of type \"int\"");
		else if (!ex.isConst)
			return p_par->setError(ec_p_expmustbeconst, ix, iy, ex.rpn);
		//evaluate as a const expression, compile time expressions 
		//		p_par->ast.constTab.at(symi->fullIdent) = evalConstExp(p_par,ex);

		//consume closing bracket
		p_par->lexer->nextToken();
	}

	//now we know if its an array, make sure to adjust the stack offset 
	//with the size of this variable
	p_par->getSymbol(symi->fullIdent)->offset = p_par->sOffset;
	p_par->sOffset += getTypeSize(pnt.tokens.at(0)) * p_par->getSymbol(symi->fullIdent)->size;

	//3. semi colon, or assignment (optional)
	switch (p_par->lexer->getToken()->type)
	{
	case tt_scolon:
		parseSColon(p_par);
		break;
	case tt_assign:
		p_par->lexer->rewind();	//parse as an expression
		ex = parseFExp(p_par);
		if (symi->isArray && !ex.isArray)
			return p_par->setError(ec_p_expmustbearray, ix, iy, ex.rpn);
		parseSColon(p_par);
		break;
	default:
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	}
	p_par->parent();
}

void parse::parseDecFunc(bcParser* p_par)
{
	bcParseNode pni;
	int oldOffset = p_par->sOffset;
	p_par->sOffset = 0;
	bcSymbol* oldScope = p_par->currentScope;
	bcSymbol sym;
	bcFuncInfo fi;

	//function keyword
	switch (p_par->lexer->getToken()->type)
	{
	case tt_function:
		p_par->addNode(bcParseNode(pn_funcdec, *p_par->lexer->getToken()));
		p_par->lexer->nextToken();
		p_par->currentFunc = &fi;
		break;
	default:
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	}

	//type and ident
	switch (p_par->lexer->getToken()->type)
	{
		//basic types
	case tt_bool:	case tt_int:	case tt_string:	case tt_float:
		p_par->addChild(bcParseNode(pn_type, *p_par->lexer->getToken()));
		fi.dataType = p_par->lexer->getToken()->data;
		p_par->lexer->nextToken();
		break;

		//user type
	case tt_ident:
		//check its a valid identifier
		pni = parseIdent(p_par);
		if (p_par->getSymbol(pni.tokens.at(0).data) == nullptr)
		{
			//error unknown symbol
			return p_par->setError(ec_p_undeclaredsymbol, p_par->lexer->getToken()->data);
		}

		//and check the identifier relates to a user defined type
		sym = *p_par->getSymbol(pni.tokens.at(0).data);
		if (sym.type == st_type)
		{
			fi.dataType = sym.fullIdent;
			p_par->addChild(bcParseNode(pn_type, sym.fullIdent));
		}
		else
		{
			//unknow error mandem, expected an type here
			return p_par->setError(ec_p_undeclaredsymbol, p_par->lexer->getToken()->data);
		}
		break;

	default:
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
		break;			//error
	}

	//function ident
	switch (p_par->lexer->getToken()->type)
	{
	case tt_ident:
		sym = resolveIdent(p_par, consumeIdent(p_par));
		if (sym.type == st_null)
		{
			//fill in symbol information
			sym.type = st_function;
			sym.dataType = fi.dataType;
			fi.ident = sym.ident; fi.fullIdent = sym.fullIdent;
			p_par->addSymbol(sym.fullIdent, &sym);
			//set our current scope to this function, using method signature
			p_par->currentScope = p_par->getSymbol(sym.fullIdent);
			//new stackframe
			p_par->ast.stackFrames.push_back(std::vector<string>());
			p_par->addChild(bcParseNode(pn_funcident, sym.fullIdent));
		}
		else if (sym.type == st_function)
		{
			//paramlist must be different or its a redefinition, make available the current paramlist
			p_par->currentFunc = &p_par->ast.funcTab->at(sym.fullIdent);
			p_par->currentScope = p_par->getSymbol(sym.fullIdent);
			p_par->currentFunc->isOverloaded = true;
			//new stackframe
			p_par->ast.stackFrames.push_back(std::vector<string>());
			p_par->addChild(bcParseNode(pn_funcident, sym.fullIdent));
		}
		else
		{
			//redef - used elsewhere, not as a function name
		}
		break;
	default:
		//error
		return p_par->setError(ec_p_undeclaredsymbol, p_par->lexer->getToken()->data);
		break;
	}

	//3. paramlist
	switch (p_par->lexer->getToken()->type)
	{
	case tt_oparen:
		//parameter list
		parseParamListDec(p_par);
		break;
	default:
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	}

	//4. (optional) function body
	switch (p_par->lexer->getToken()->type)
	{
	case tt_obrace:
		p_par->noDecFunc = true;
		//fi.body.insert(
		parseBlock(p_par);
		p_par->sOffset = oldOffset;
		p_par->noDecFunc = false;
		break;
	default:
		break;
	}
	//pop stack frame, adding it to the currentFunc info, using the current method signature
	fi.stackFrames.insert(std::make_pair(getMethodStringSignature(p_par->currentParamList), p_par->ast.stackFrames[p_par->ast.stackFrames.size() - 1]));
	p_par->ast.stackFrames.erase(p_par->ast.stackFrames.end() - 1);
	p_par->ast.funcTab->insert(std::make_pair(sym.fullIdent, fi));
	//change scopes back 
	p_par->currentScope = oldScope;
	p_par->currentFunc = nullptr;
	p_par->currentParamList = nullptr;
	p_par->sOffset = oldOffset;
	p_par->parent();
}

void parse::parseParamListCall(bcParser* p_par, bcSymbol* id)
{
	bcParamList cpl;
	bcParseNode pni;
	bcSymbol sym;
	bcExpression ex;

	//get our opening parenthesis (
	if (p_par->lexer->getToken()->type != tt_oparen)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addNode(bcParseNode(pn_paramlist));

	//loop thru all the parameters in the func call, all parse as expressions
	while (p_par->lexer->getToken()->type != tt_cparen)
	{
		if (p_par->lexer->getToken()->type != tt_comma)
		{
			//parse the expression
			ex = parseFExp(p_par);
			cpl.params.push_back(ex.dataType);	//only bovv'd about its leftmost dataType (type symbols fullIdent)
			p_par->addNode(pni);
		}
		else
		{
			//consume comma 
			p_par->lexer->nextToken();
		}
	}

	//check this signature exists within the given idents symbol table signatures
	if (!checkForOverload(p_par, &cpl, id))
		return p_par->setError(ec_p_badparams, p_par->lexer->getToken()->data);

	//consume cparen
	p_par->lexer->nextToken();
	p_par->parent();
}

//this code shud be much the same as parseDecVar
void parse::parseParamListDec(bcParser* p_par)
{
	bcParseNode pni, pnt;
	bcParamList pl = *new bcParamList();
	bcSymbol symt, symi;

	//get our opening parenthesis (
	if (p_par->lexer->getToken()->type != tt_oparen)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	tree<bcParseNode>::iterator plnode = p_par->addNode(bcParseNode(pn_decparamlist));

	//loop thru all the parameters in the func call
	while (p_par->lexer->getToken()->type != tt_cparen)
		if (p_par->lexer->getToken()->type == tt_comma)
		{
		p_par->lexer->nextToken();
		}
		else
		{
			//get stick the string we get into symbol table
			switch (p_par->lexer->getToken()->type)
			{
			case tt_bool:	case tt_int:	case tt_string:	case tt_float:
				symt = *p_par->getSymbol(p_par->lexer->getToken()->data);
				pl.params.push_back(symt.fullIdent);
				p_par->addChild(pnt = bcParseNode(pn_type, *p_par->lexer->getToken()));
				p_par->lexer->nextToken();
				break;
			case tt_ident:
				pni = parseIdent(p_par);
				symt = *p_par->getSymbol(pni.tokens.at(0).data);
				if (symt.type != st_type)
					return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);	//must be a type
				pl.params.push_back(symt.fullIdent);
				p_par->addChild(pnt = bcParseNode(pn_type, symt.fullIdent));
			}

			//ident
			symi = resolveIdent(p_par, consumeIdent(p_par));
			if (symi.type != st_null)
				return p_par->setError(ec_p_redefinition, symi.ident);
			symi.type = st_var;
			symi.dataType = symt.dataType;
			//add parameters symbol
			p_par->addSymbol(symi.fullIdent, &symi);
			p_par->getSymbol(symi.fullIdent)->isConst = false;
			if (!(p_par->getSymbol(symi.fullIdent)->isArray = symt.isArray))
				p_par->getSymbol(symi.fullIdent)->size = 1;
			//make sure to adjust the stack offset with the size of this variable
			p_par->getSymbol(symi.fullIdent)->offset = p_par->sOffset;
			p_par->sOffset += getTypeSize(pnt.tokens.at(0))*p_par->getSymbol(symi.fullIdent)->size;

			p_par->addChild(bcParseNode(symi.type));
		}

	//consume cparen, add to current bcFuncInfo
	p_par->currentFunc->sigs.insert(std::make_pair(getMethodStringSignature(&pl), pl));
	p_par->currentParamList = &p_par->currentFunc->sigs[getMethodStringSignature(&pl)];
	plnode.node->data.tokens.push_back(bcToken(getMethodStringSignature(&pl)));
	p_par->lexer->nextToken();
	p_par->parent();
}


void parse::parseSColon(bcParser* p_par)
{
	//semi colon 
	if (p_par->lexer->getToken()->type != tt_scolon)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
}

void parse::parseBlock(bcParser* p_par)
{
	//opening brace
	if (p_par->lexer->getToken()->type != tt_obrace)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();

	//if were currently declaring a function, or (currentFunc!=nullptr), make a note of the body
	p_par->addNode(bcParseNode(pn_block));
	if (p_par->currentFunc)
		p_par->currentFunc->body[getMethodStringSignature(p_par->currentParamList)] = p_par->getNode();

	//inner contents
	while (!p_par->error && p_par->lexer->getToken()->type != tt_cbrace)
		parseStatement(p_par);
	if (p_par->error)	return;

	//closing brace
	p_par->lexer->nextToken();
	p_par->parent();
}

void parse::parseIf(bcParser* p_par)
{
	bool blocks = false;
	//opening if
	if (p_par->lexer->getToken()->type != tt_if)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addNode(bcParseNode(pn_if));

	//conditional expression
	parseFExp(p_par);

	//true block
	p_par->addNode(bcParseNode(pn_if_trueblock));
	if (p_par->lexer->getToken()->type == tt_obrace)
		blocks = true;
	if (blocks)
		parseBlock(p_par);
	else
		parseStatement(p_par);
	p_par->parent();

	//optional elseif/else
	while (p_par->lexer->getToken()->type == tt_else || p_par->lexer->getToken()->type == tt_elseif)
	{
		p_par->addNode(bcParseNode(pn_if_elseblock));
		p_par->lexer->nextToken();
		if (blocks)
			parseBlock(p_par);
		else
			parseStatement(p_par);
		p_par->parent();
	}
	p_par->parent();
}

void parse::parseReturn(bcParser* p_par)
{
	//return keyword
	if (p_par->lexer->getToken()->type != tt_return)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addChild(bcParseNode(pn_return));

	//return expression
	parseFExp(p_par);

	//semi colon
	parseSColon(p_par);
}

void parse::parseBreak(bcParser* p_par)
{
	//break keyword
	if (p_par->lexer->getToken()->type != tt_break)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addChild(bcParseNode(pn_break));

	//semi colon
	parseSColon(p_par);
}

void parse::parseContinue(bcParser* p_par)
{
	//continue keyword
	if (p_par->lexer->getToken()->type != tt_continue)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addChild(bcParseNode(pn_continue));

	//semi colon
	parseSColon(p_par);
}

//nextToken til end of ident, return shortident
string parse::consumeIdent(bcParser* p_par)
{
	string id;
	bool exit = false;
	while (!exit)
		switch (p_par->lexer->getToken()->type)
	{
		case tt_ident:
		case tt_period:
		case tt_dcolon:
		case tt_colon:
			id += p_par->lexer->getToken()->data;
			p_par->lexer->nextToken();
			break;
		default:
			exit = true;
	}
	return id;
}

//get symbol, if any, consume ident in lexer
bcParseNode parse::parseIdent(bcParser* p_par)
{
	string p_sIdent;
	bcSymbol sym = resolveIdent(p_par, p_sIdent = consumeIdent(p_par));

	bcParseNode pn(sym.type);
	pn.tokens.push_back(bcToken(tt_ident, sym.fullIdent));
	pn.tokens.push_back(bcToken(tt_intlit, bcitos(sym.offset)));

	//parse array index or func call
	if (sym.isArray)
		parseArrayIndex(p_par, &pn);

	return pn;
}

void parse::parseArrayIndex(bcParser* p_par, bcParseNode* pn)
{
	bcExpression ex;
	int ix, iy;
	switch (p_par->lexer->getToken()->type)
	{
	case tt_obracket:
		ix = p_par->lexer->getToken()->x;
		iy = p_par->lexer->getToken()->y;
		p_par->lexer->nextToken();

		//exoect to parse an expression, check for error, array ref must be const int
		ex = parseFExp(p_par);
		if (!ex.isConst)
		{
			p_par->setError(ec_p_expmustbeconst, ix, iy, ex.rpn);
			return;
		}
		else if (ex.dataType != "int")
		{
			p_par->setError(ec_p_nonintsubscript, ix, iy, ex.rpn);
			return;
		}

		//just check for int now
		//pn->tokens.push_back(*p_par->lexer->getToken());

		//p_par->lexer->nextToken();

		//consume closing bracket
		p_par->lexer->nextToken();
	default:
		//error
		break;
	}
}

//check shortident is a valid symbol in current scope
//shortid must be valid in currentScope
//null symbol returned if ident cant be found in any scope
//cleanup the return object
bcSymbol parse::resolveIdent(bcParser* p_par, string shortid)
{
	//return a null type symbol if the shortident doesnt resolve at any scope
	bcSymbol sym;	sym.type = st_null;	sym.ident = shortid;
	sym.fullIdent = getFullIdent(p_par, shortid, p_par->currentScope);
	string buff, fullid;
	int ind;

	//deal with explicit identifiers (with scope/member prefix)
	if (isIdentExplicit(p_par, shortid))
	{
		if (p_par->getSymbol(shortid))
			return *p_par->getSymbol(shortid);
		return sym;
	}
	else
	{
		fullid = getFullIdent(p_par, shortid, p_par->currentScope);
		ind = fullid.size() - 1;
	}

	//check all parent scopes as well, backwards to $global
	bcSymbol sc;
	while (ind > 0)
		switch (fullid[ind])
	{
		//two steps pack if we get two colons
		case ':':
			if (ind > 0 && fullid[ind - 1] == ':')
				--ind;
		case '.':	//member
			--ind;
			sc = *p_par->getSymbol(fullid.substr(0, ind + 1));	//get scope
			if (p_par->getSymbol(getFullIdent(p_par, shortid, &sc)))
				return *p_par->getSymbol(getFullIdent(p_par, shortid, &sc));
			break;
		default:
			--ind;
	}
	return sym;
}

//make a string from a given shortident and scope
string parse::getFullIdent(bcParser* p_par, string p_sIdent, bcSymbol* p_scope)
{
	if (!isIdentExplicit(p_par, p_sIdent))
		switch (p_scope->type)
	{
		case st_namespace:
			return p_scope->fullIdent + "::" + p_sIdent;
		case st_function:
			return p_scope->fullIdent + ":" + p_sIdent;
		default:
			return "";
	}

	return p_sIdent;//actually explicait
}

//check whether a given string contains any scope or members ie (main:arg1 or string)
bool parse::isIdentExplicit(bcParser* p_par, string id)
{
	string shortid;
	bcSymbol sc;
	int ind = id.size() - 1;

	while (ind > 0)
		switch (id[ind])
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

//creates a short ident (normally the variable/function name - "foo") from a full ident ("$global::main::foo")
string bc::parse::getShortIdent(string p_fid)
{
	int i = p_fid.size() - 1;
	while (i > 0)
	{
		if (p_fid[i] == ':' || p_fid[i] == '.')
			return p_fid.substr(i + 1, p_fid.size() - i);
		i--;
	}
	return p_fid;
}

void parse::parseFuncCall(bcParser* p_par, bcParseNode pn)
{
	//ident must be a declared function symbol
	bcSymbol sym = *p_par->getSymbol(pn.tokens.at(0).data);

	if (sym.type != st_function)
		//probably the variable name was already used elswhere
		return p_par->setError(ec_p_invalidsymbol, p_par->lexer->getToken()->data);
	else if (sym.type == st_null)
		//unknown identifier
		return p_par->setError(ec_p_undeclaredsymbol, p_par->lexer->getToken()->data);
	p_par->addNode(bcParseNode(pn_funccall)).node->data.tokens.push_back(bcToken(sym.fullIdent));

	//params
	parseParamListCall(p_par, p_par->getSymbol(sym.fullIdent));

	//semi colon
	parseSColon(p_par);
	p_par->parent();
}

void parse::parseWhile(bcParser* p_par)
{
	//while keyword
	if (p_par->lexer->getToken()->type != tt_while)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addChild(bcParseNode(pn_while));

	//condition
	parseFExp(p_par);

	//block to execute
	parseBlock(p_par);
}

void parse::parseAssignment(bcParser* p_par, bcParseNode pn)
{
	//ident already parsed
	bcSymbol sym = *p_par->getSymbol(pn.tokens.at(0).data);
	p_par->addNode(bcParseNode(pn_assignment));
	p_par->lexer->rewind();
	//expression
	parseFExp(p_par, pn);
	//semi colon
	parseSColon(p_par);
	p_par->parent();
}

//full expression - only this method makes the pn_exp parse node
bcExpression parse::parseFExp(bcParser* p_par)
{
	tree<bcParseNode>::iterator exnode;
	exnode = p_par->addNode(bcParseNode(pn_exp));
	bcExpression ex = parseExp(p_par);
	ex.node = exnode;
	p_par->parent();
	return ex;
}

//for when weve already consumed the identifier with parseIdent(), copy code from parseSubExp
bcExpression parse::parseFExp(bcParser* p_par, bcParseNode id)
{
	bcExpression ex; ex.isConst = true;
	bcParseNode pn; bcToken op;
	ex.node = p_par->addNode(bcParseNode(pn_exp));

	//taken from parseFactor
	bcSymbol sym = *p_par->getSymbol(id.tokens.at(0).data);
	switch (sym.type)
	{
	case st_var:case st_function:
		ex.dataType = sym.dataType;
		break;
	default:
		p_par->setError(ec_p_invalidsymbol, p_par->lexer->getToken()->data);
		return ex;
	}
	p_par->addChild(id);

	while (p_par->lexer->getToken())
	{
		op = *p_par->lexer->getToken();
		switch (op.type)
		{
		case tt_less:	case tt_greater:	case tt_lessequal:	case tt_greaterequal:
		case tt_equal:	case tt_notequal:	case tt_logand:		case tt_logor:		case tt_assign:
			pn = bcParseNode(op.type);
			pn.tokens.push_back(op);
			pn.tokens.push_back(bcitos(p_par->getSymbol(id.tokens.at(0).data)->offset));	//stack index of oper1
			pn.tokens.at(pn.tokens.size()-1).type = tt_lvalue;	//denote that its an l-value
			p_par->addChild(pn);
			p_par->lexer->nextToken();
			break;
		default:
			p_par->parent();
			return ex;
		}
		parseSubExp(p_par, &ex);
	}
	p_par->parent();
	return ex;
}


bcExpression parse::parseExp(bcParser* p_par)
{
	bcExpression ex; ex.isConst = true;
	bcToken oper1, oper2, op, tok;
	bcParseNode pn;
	oper1 = parseSubExp(p_par, &ex);
	ex.dataType = getDatatype(p_par, oper1);
	while (p_par->lexer->getToken())
	{
		op = *p_par->lexer->getToken();
		switch (op.type)
		{
		case tt_less:	case tt_greater:	case tt_lessequal:			case tt_greaterequal:
		case tt_equal:	case tt_notequal:	case tt_logand:		case tt_logor:		case tt_assign:
			pn = bcParseNode(op.type);
			pn.tokens.push_back(op);
			//use last idents token string in getSymbol to push the stack offset of lastIdent onto this parseNode. used in ByteCode generation
			tok = bcitos(p_par->getSymbol(p_par->lastIdent.node->data.tokens.at(0).data)->offset);
			if (op.type == tt_assign)
				p_par->lastIdent.node->data.tokens.at(1).type = tt_lvalue;
			pn.tokens.push_back(tok);
			p_par->addChild(pn);
			p_par->lexer->nextToken();
			break;
		default:
			//function call or one sided expression (ie not a boolean or assignment)
			return ex;
		}
		oper2 = parseSubExp(p_par, &ex);
	}
	return ex;
}

lex::bcToken parse::parseSubExp(bcParser* p_par, bcExpression* ex)
{
	bcToken oper1, oper2, op;
	bcParseNode pn;
	oper1 = parseTerm(p_par, ex);
	while (p_par->lexer->getToken())
	{
		op = *p_par->lexer->getToken();
		switch (op.type)
		{
		case tt_plus:	case tt_minus:
			pn = bcParseNode(op.type);
			pn.tokens.push_back(op);
			p_par->addChild(pn);
			p_par->lexer->nextToken();
			break;
		default:
			return oper1;
		}
		oper2 = parseTerm(p_par, ex);
	}
	return oper1;
}

lex::bcToken parse::parseTerm(bcParser* p_par, bcExpression* ex)
{
	bcToken oper1, oper2, op;
	bcParseNode pn;
	oper1 = parseFactor(p_par, ex);
	while (p_par->lexer->getToken())
	{
		op = *p_par->lexer->getToken();
		switch (op.type)
		{
		case tt_div:	case tt_mult: case tt_expo:	case tt_mod:
			pn = bcParseNode(op.type);
			pn.tokens.push_back(op);
			p_par->addChild(pn);
			p_par->lexer->nextToken();
			break;
		default:
			return oper1;
		}
		oper2 = parseFactor(p_par, ex);
	}
	return oper1;
}

bcToken parse::parseFactor(bcParser* p_par, bcExpression* ex)
{
	bcExpression ex2;
	bcParseNode pn, pni;
	bcSymbol sym; sym.type = st_null;
	bcToken oper1, oper2, op;
	bool negate;

	oper1 = *p_par->lexer->getToken();
	//check for negation
	if (oper1.type == tt_minus)
	{
		negate = true;
		oper1 = *p_par->lexer->nextToken();
	}
	switch (oper1.type)
	{
	case tt_oparen:
		//	(
		p_par->parenCount++;
		pn = bcParseNode(oper1.type);
		pn.tokens.push_back(oper1);
		p_par->addChild(pn);
		p_par->lexer->nextToken();

		//parse inner expression, marking a change to non-constness if needed (isConst true by default)
		if (parseExp(p_par).isConst == false && ex->isConst)
			ex->isConst = false;
		if (p_par->lexer->getToken()->type != tt_cparen)
		{
			p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
			return oper1;	//error
		}

		//	)
		p_par->parenCount--;
		pn = bcParseNode(pn_cparen);
		pn.tokens.push_back(*p_par->lexer->getToken());
		p_par->addChild(pn);
		p_par->lexer->nextToken();
		break;

	case tt_cparen:
		//p_par->setError(ec_p_unexpectedtoken,p_par->lexer->getToken()->data);
		return oper1;

	case tt_ident:
		pni = parseIdent(p_par);
		if (p_par->getSymbol(pni.tokens.at(0).data))
			sym = *p_par->getSymbol(pni.tokens.at(0).data);
		switch (sym.type)
		{
		case st_function:
			parseFuncCall(p_par, pni);		//parse as func call, passing the free floating bcParseNode we made
			if (!sym.isConst)	ex->isConst = false;
			break;
		case st_var:
			if (!sym.isConst)	
				ex->isConst = false;
			if (sym.isArray)		
				parseArrayIndex(p_par, &pni);	//the [] subscript
			p_par->lastIdent = p_par->addChild(pni);
			break;
		case st_namespace:
			p_par->setError(ec_p_invalidsymbol, p_par->lexer->getToken()->data);
			return oper1;
		default:
			p_par->setError(ec_p_undeclaredsymbol, p_par->lexer->getToken()->data);
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
		pn = bcParseNode(oper1.type);
		pn.tokens.push_back(oper1);
		p_par->addChild(pn);
		p_par->lexer->nextToken();
		break;

	default:
		//p_par->setError(ec_p_unexpectedtoken,p_par->lexer->getToken()->data);
		return oper1;	//error
	}
	return oper1;
}

//assumes a valid set of bcSymbol pointaz
string parse::getMethodStringSignature(bcParamList* pl)
{
	string out;
	for (int t = 0; t < pl->params.size(); ++t)
		out += pl->params[t] + ",";
	return out;
}

//checks whether a given ParamList has been declared for the given symbol
bool parse::checkForOverload(bcParser* p_par, bcParamList* pl, bcSymbol* id)
{
	//get function info for the given ident, check for the currently parsed function as well to allow recursion
	if (p_par->currentFunc && p_par->currentFunc->fullIdent == id->fullIdent)
		if (getMethodStringSignature(p_par->currentParamList) == getMethodStringSignature(pl))
		{
		return true;
		}
		else
		{
			p_par->setError(ec_p_badparams, id->fullIdent + " " + getMethodStringSignature(pl));
			return false;
		}
	bcFuncInfo fi = p_par->ast.funcTab->at(id->fullIdent);
	//check the given paramlist is in there somewhere
	return (fi.sigs.find(getMethodStringSignature(pl)) != fi.sigs.end());
}

//adds an ident if it doesnt exist locally
//parse, checks ident already exists in any scope, checks locally too if it does
//if it doesnt exist, its ok to make the declaration and adds it to symbol table etc
//returns null if ident already exists
bcSymbol* parse::addIdentDec(bcParser* p_par, bcSymbolType ty)
{
	bcSymbol sym = resolveIdent(p_par, consumeIdent(p_par));
	if (sym.type != st_null)
		if (p_par->getSymbol(getFullIdent(p_par, sym.ident, p_par->currentScope))
			&& p_par->getSymbol(getFullIdent(p_par, sym.ident, p_par->currentScope))->type != st_null)
		{
		//same identifier exists in this scope
		p_par->setError(ec_p_redefinition, sym.ident);
		return nullptr;	//redefinition
		}
		else
		{
			sym.fullIdent = getFullIdent(p_par, sym.ident, p_par->currentScope);
		}
	//no ident of that name in current scope, declare variable
	sym.type = ty;
	p_par->addSymbol(&sym);
	return p_par->getSymbol(sym.fullIdent);
}

int parse::getTypeSize(bcToken t)
{
	switch (t.type)
	{
	default:
	case tt_bool:
	case tt_string:
	case tt_int:
		return 1;
	case tt_float:
		return 2;
	}
}


bool parse::checkOperandTypes(bcParser* p_par, bcToken oper1, bcToken op, bcToken oper2)
{
	bcSymbol sym;
	bcToken opers[2];
	string oper_dt[2];
	opers[0] = oper1; opers[1] = oper2;

	//get dataType of each operand as a string
	for (int t = 0; t < 2; ++t)
		if (opers[t].type == tt_ident)
		{
		sym = *p_par->getSymbol(opers[t].data);
		switch (sym.type)
		{
		case st_var:	case st_function:
			oper_dt[t] = sym.dataType;
			break;
		default:
			//error
			return false;
		}
		}
		else
		{
			//handle literals
			switch (opers[t].type)
			{
			case tt_intlit:	case tt_strlit:	case tt_fltlit:
				oper_dt[t] = p_par->getSymbol(opers[t].data)->dataType;	break;
			case tt_true:	case tt_false:
				oper_dt[t] = p_par->getSymbol("bool")->dataType;	break;
			}
		}

	//banging massive table
	switch (op.type)
	{
		//float/int vvv
	case tt_div:	case tt_mult:	case tt_expo:
	case tt_mod:	case tt_minus: 	case tt_plus:
		if (oper_dt[0] == "string" || oper_dt[1] == "string")
			return false;
		//string/bool  vvv
	case tt_less:	case tt_greater:	case tt_lessequal:	case tt_greaterequal:
	case tt_equal:	case tt_notequal:	case tt_logand:		case tt_logor:		case tt_assign:
		//bools, ints and floats can mix, but strings cannot 
		if (oper_dt[0] == "string"&&oper_dt[1] != "string")
			return false;

		break;
	}

	return false;
}

string parse::getDatatype(bcParser* p_par, bcToken tk)
{
	switch (tk.type)
	{
	case tt_intlit:
		return p_par->getSymbol("int")->dataType;
	case tt_strlit:
		return p_par->getSymbol("string")->dataType;
	case tt_fltlit:
		return p_par->getSymbol("float")->dataType;
	case tt_true:
	case tt_false:
		return p_par->getSymbol("bool")->dataType;
	case tt_ident:
		if (p_par->getSymbol(getFullIdent(p_par, tk.data, p_par->currentScope)))
			return p_par->getSymbol(getFullIdent(p_par, tk.data, p_par->currentScope))->dataType;
		return "";
	default:
		return "";
	}
	return "";
}

int parse::getPrecedence(lex::bcToken tokin)
{
	switch (tokin.type)
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

	case tt_mod:
	case tt_expo:
		return 9;

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

//0 = left 1 = right
int parse::getAssociativity(lex::bcToken tokin)
{
	switch (tokin.type)
	{
	case tt_mod:
	case tt_expo:
		return 1;
	}
	return 0;
}

int parse::isOperator(lex::bcToken tokin)
{
	switch (tokin.type)
	{
	case tt_assign:
	case tt_logor:	case tt_logand:	case tt_equal:	case tt_notequal:
	case tt_greater:case tt_less:	case tt_lessequal:	case tt_greaterequal:
	case tt_mult:	case tt_div:	case tt_plus:	case tt_minus:	case tt_lognot:
		return true;
	}
	return false;
}

int parse::evalConstExp(bcParser* p_par, bcExpression ex)
{


	return 1;
}