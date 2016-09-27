#include "parser.h"
#include "util.h"

using namespace std;
using namespace wc::util;
using namespace wc::lex;


namespace wc{
namespace parse{

		//parseDecVar abbreviations
		wcSymbol parseDecVar_Type(wcParser*, wcParseNode* p_type, wcParseNode* p_ident);
		int parseDecVar_Ident(wcParser* p_par, wcSymbol* p_type, wcSymbol*& p_ident, bool p_setToConst, wcParseNode* p_identPN);
		int parseDecVar_OBracket(wcParser* p_par, wcSymbol* p_ident, int p_identX, int p_identY);
		int parseDecVar_Exp(wcParser* p_par, wcSymbol*& p_ident, wcParseNode* p_identPN);

		void parseDecFunc_Type(wcParser* p_par, wcFuncInfo* p_fi, wcSymbol* p_sym);
		void parseDecFunc_Ident(wcParser* p_par, wcFuncInfo* p_fi, wcSymbol* p_sym);
		void parseDecFunc_ParamList(wcParser* p_par);

		wcExpression parseFullExp_OuterExpression(wcParser* p_par, wcParseNode p_id, wcExpression p_ex);
		wcExpression parseExp_OuterExpression(wcParser* p_par, wcToken p_oper1, wcExpression p_ex);

		void executeRPN(wcParser* p_par, std::vector<wcParseNode*> p_rpn);

		void createGlobal(wcParser* p_par);
		void createBasicTypes(wcParser* p_par);



		//helps with constructor
		const std::unordered_multimap<wcTokenType, wcParseNodeType> token2ParseNodeTypes = {
			{ tt_greater , pn_greater },{ tt_less , pn_less },{ tt_equal , pn_equal },{ tt_notequal , pn_notequal } ,
			{ tt_greaterequal , pn_greaterequal },{ tt_lessequal , pn_lessequal },{ tt_lognot , pn_lognot } ,
			{ tt_logor , pn_logor },{ tt_logand , pn_logand },{ tt_incr , pn_incr },{ tt_decr , pn_decr } ,
			{ tt_plusassign , pn_plusassign },{ tt_minusassign , pn_minusassign },{ tt_multassign , pn_multassign } ,
			{ tt_divassign , pn_divassign },{ tt_intlit , pn_intlit },{ tt_strlit , pn_strlit },{ tt_fltlit , pn_fltlit } ,
			{ tt_ident , pn_ident },{ tt_minus , pn_minus },{ tt_plus , pn_plus },{ tt_div , pn_div } ,
			{ tt_mult ,	pn_mult },{ tt_mod , pn_mod }, { tt_expo , pn_expo },{ tt_assign , pn_assign },
			{ tt_oparen , pn_oparen },{ tt_cparen , pn_cparen }
		};



wc::parse::wcExpression::wcExpression()
{
	this->dataType = "";
	this->isArray = false;
	this->isAssign = false;
	this->isBool = false;
	this->isConst = false;
	this->isFunc = false;
	//this->node=null;
}

wc::parse::wcSymbol::wcSymbol()
{
	ident = fullIdent = dataType = "";
	type = st_null;
	offset = dataSize = 0;
	isStatic = isConst = isArray = false;
}

wc::parse::wcParseNode::wcParseNode()
{

}

wc::parse::wcParseNode::wcParseNode(wcSymbolType t)
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

wcParseNode::wcParseNode(wcTokenType t)
{
	if(token2ParseNodeTypes.find(t) == token2ParseNodeTypes.end())
		type = pn_null;
	else 
		type = token2ParseNodeTypes.find(t)->second;
}

wcParseNode::wcParseNode(wcTokenType t, wcToken p_tok)
{
	*this = wcParseNode(t);
	tokens.push_back(p_tok);
}

wcParseNode::wcParseNode(wcTokenType t, wcToken p_tok1, wcToken p_tok2)
{
	*this = wcParseNode(t);
	tokens.push_back(p_tok1);
	tokens.push_back(p_tok2);
}

//allocates buffers for parsing whole source
//must be called before we parse
void wc::parse::wcParser::startup()
{
	//reset data
	clear();

	//create tree
	ast.tree = tree<wcParseNode>(wcParseNode(pn_head));
	pindex = ast.tree.begin();

	//create symbol and function table
	ast.funcTab = unordered_map<string, wcFuncInfo>();
	ast.symTab = unordered_map<string, wcSymbol>();

	//setup global symbols
	createGlobal(this);

	//basic builtin type symbol entries
	createBasicTypes(this);
}

void wc::parse::createGlobal(wcParser* p_par)
{
	//global stackframe
	p_par->ast.stackFrames.push_back(wcStackFrameInfo());	
	p_par->currentStackFrame = &p_par->ast.stackFrames[0];	
	
	//global symbol table entry
	wcSymbol sym;	
	sym.fullIdent = sym.ident = "$global";
	sym.type = st_namespace;	
	p_par->ast.addSymbol(sym.ident, &sym); //add entry in symbol table
	p_par->currentScope = p_par->ast.getSymbol("$global");			//point global scope pointer to global

}

//the only symbols in the table without their scope set to "$global::"
void wc::parse::createBasicTypes(wcParser* p_par)
{
	wcSymbol sym; sym.type = st_type; sym.size = 1;
	sym.dataType = sym.fullIdent = sym.ident = "int";	sym.dataSize = 1; 	p_par->ast.addSymbol(sym.ident, &sym);
	sym.dataType = sym.fullIdent = sym.ident = "flt";	sym.dataSize = 2; 	p_par->ast.addSymbol(sym.ident, &sym);
	sym.dataType = sym.fullIdent = sym.ident = "chr";	sym.dataSize = 1; 	p_par->ast.addSymbol(sym.ident, &sym);
	sym.dataType = sym.fullIdent = sym.ident = "str";	sym.dataSize = 1; 	p_par->ast.addSymbol(sym.ident, &sym);
	sym.dataType = sym.fullIdent = sym.ident = "bool";	sym.dataSize = 1;	p_par->ast.addSymbol(sym.ident, &sym);
	sym.dataType = sym.fullIdent = sym.ident = "obj";	sym.dataSize = 1;	p_par->ast.addSymbol(sym.ident, &sym);
	sym.dataType = sym.fullIdent = sym.ident = "var";	sym.dataSize = 1;	p_par->ast.addSymbol(sym.ident, &sym);
}

void wc::parse::wcParser::shutdown()
{

}

void wc::parse::wcParser::clear()
{
	sOffset = parenCount = 0;
	error = ec_null;
	currentStackFrame = nullptr;
	currentParamList = nullptr;
	currentFunc = nullptr;
	currentScope = nullptr;
	noDecVar = noDecFunc = noDecName = false;	
	
	ast.clear();

	lexer->clear();
}

void wc::parse::wcParser::setError(wcErrorCode ec, int x, int y, string s)
{
	error = ec; errorL = y;
	errorC = x; errorS = s;
}

void wc::parse::wcParser::setError(wcErrorCode ec, string s)
{
	error = ec;
	errorL = lexer->getToken()->y;
	errorC = lexer->getToken()->x;
	errorS = s;
}

unsigned int wc::parse::wcParser::getError()
{
	return error;
}

unsigned int wc::parse::wcParser::getErrorLine()
{
	return errorL;
}

unsigned int wc::parse::wcParser::getErrorCol()
{
	return errorC;
}

string wc::parse::wcParser::getErrorString()
{
	return errorS;
}

string getErrorCodeAsString(wcErrorCode p_ec)
{
	if (errorStrings.find(p_ec) != errorStrings.end())
		return errorStrings.find(p_ec)->second;
	return "";
}

tree<wcParseNode>::iterator wc::parse::wcParser::getNode()
{
	return pindex;
}

tree<wcParseNode>::iterator wc::parse::wcParser::prevNode()
{
	return --pindex;
}

tree<wcParseNode>::iterator wc::parse::wcParser::addNode(wcParseNode pn)
{
	pindex = ast.tree.append_child(pindex, pn);
	return pindex;
}

tree<wcParseNode>::iterator wc::parse::wcParser::addChild(wcParseNode pn)
{
	ast.tree.append_child(pindex, pn);
	return ast.tree.child(pindex, ast.tree.number_of_children(pindex) - 1);
}

void wc::parse::wcParser::parent()
{
	pindex = ast.tree.parent(pindex);
}

void wc::parse::wcAST::clear()
{
	constTab.clear();
	funcTab.clear();
	stackFrames.clear();
	symTab.clear();
	tree.clear();
}

bool wc::parse::wcAST::addSymbol(wcSymbol* p_sym)
{
	return addSymbol(p_sym->fullIdent, p_sym);
}

bool wc::parse::wcAST::addSymbol(string p_id, wcSymbol* p_sym)
{
	if (symTab.find(p_id) != symTab.end())
		return false;	//already exists
	symTab.insert(make_pair(p_id, *p_sym));
	return true;
}

wcSymbol* wc::parse::wcAST::getSymbol(string p_fullid)
{
	//add global so coder doesnt have to
	if (p_fullid.size() && p_fullid.substr(0, 9) != "$global::" && (p_fullid != "$global" && p_fullid != "int" 
		&& p_fullid != "flt" && p_fullid != "str" && p_fullid != "bool" && p_fullid != "var" && p_fullid != "obj"))
		p_fullid = "$global::" + p_fullid;
	if (symTab.find(p_fullid) == symTab.end())
		return nullptr;
	return &symTab.at(p_fullid);
}

//takes a shortid in a given scope
wcSymbol* wc::parse::wcAST::getSymbol(string p_shortid, wcSymbol* p_sym)
{
	if (symTab.find(getFullIdent(p_shortid, p_sym)) == symTab.end())
		return nullptr;
	return &symTab.at(getFullIdent(p_shortid, p_sym));
}

//parses entire input 
//assumes wcParser.startup() has been called
int wc::parse::wcParser::parse()
{
	//start the lexer off
	lexer->nextToken();

	//parse til we cant parse no mo
	while (!lexer->data.done && !error)
		parseStatement(this);

	//return any errors
	return getError();
}

void wc::parse::parseStatement(wcParser* p_par)
{
	wcExpression ex;
	wcParseNode pni;
	wcSymbol sym;

	p_par->addNode(wcParseNode(pn_statement));

	switch (p_par->lexer->getToken()->type)
	{
	case tt_eof:
		return;
		break;
	case tt_scolon:
		//empty statement
		parseSColon(p_par);
		break;	
	case tt_const:	
	CASE_BASIC_TYPES_TT
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
	//treat literals as the start of an expression
	case tt_oparen:
	CASE_ALL_LITERALS_TT
		parseFullExp(p_par);
		break;
	case tt_ident:
		pni = parseIdent(p_par);
		if (p_par->ast.getSymbol(pni.tokens[0].data))
			sym = *p_par->ast.getSymbol(pni.tokens[0].data);
		switch (sym.type)
		{
		case st_function:
		case st_var:
			ex = parseFullExp(p_par, pni);
			parseSColon(p_par);
			break;
		case st_null:
		default:
			//unknown identifier
			return p_par->setError(ec_p_undeclaredsymbol, pni.tokens[0].y, pni.tokens[0].x, sym.fullIdent);
		}
		break;
	default:
		return p_par->setError(ec_p_unexpectedtoken,p_par->lexer->getToken()->data);
		break;
	}
	p_par->parent();
}

void wc::parse::parseDecNamespace(wcParser* p_par)
{
	//namespace keyword
	if (p_par->lexer->getToken()->type != tt_namespace)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->noDecName = p_par->noDecVar = p_par->noDecFunc = false;
	p_par->addNode(wcParseNode(pn_namespacedec));

	//ident
	wcSymbol* sym = parseIdentDeclaration(p_par, st_namespace);
	if (sym->type == st_null)
		return;	//error - ident already used;	handled in parseIdentDeclaration()
	wcSymbol* oldScope = p_par->currentScope;

	//block
	p_par->currentScope = p_par->ast.getSymbol(sym->fullIdent);
	parseBlock(p_par);
	p_par->currentScope = oldScope;

}

//returns a symbol containing the type info
wcSymbol wc::parse::parseDecVar_Type(wcParser* p_par, wcParseNode* p_pnType, wcParseNode* p_pnIdent)
{
	wcSymbol typeSymbol;
	wcToken typeToken = *p_par->lexer->getToken();
	p_pnType->tokens.push_back(typeToken);
	
	switch (typeToken.type)
	{	
		CASE_BASIC_TYPES_TT
		typeSymbol = *p_par->ast.getSymbol(typeToken.data);
		//typeSymbol.dataType = p_par->ast.getSymbol(typeToken.data)->ident;
		//typeSymbol.type = getTypeFromDataType(p_par, typeSymbol.dataType);
		p_par->addChild(wcParseNode(pn_type, typeToken));
		p_par->lexer->nextToken();
		break;

		//TODO User defined types....
	case tt_ident:
		//parse and save the ident
		*p_pnIdent = parseIdent(p_par);
		typeSymbol = *p_par->ast.getSymbol(p_pnIdent->tokens[0].data);

		//if it ent a type, error!
		if (typeSymbol.type != st_type)
		{
			p_par->setError(ec_p_undeclaredtype, typeToken.data);
			return wcSymbol();
		}

		//add the node
		typeSymbol.dataType = typeSymbol.fullIdent;
		p_par->addChild(*p_pnType = wcParseNode(pn_type, typeSymbol.fullIdent));
		break;

	default:
		//no cigar
		return wcSymbol();
	}

	return typeSymbol;
}

void wc::parse::parseDecVar(wcParser* p_par)
{
	bool setToConst = false;
	wcParseNode pnType, pnIdent;
	wcSymbol symType, *symIdent;
	wcExpression ex;
	p_par->addNode(wcParseNode(pn_vardec));

	//0. check for const
	if (p_par->lexer->getToken()->type == tt_const)
	{
		setToConst = true;
		p_par->lexer->nextToken();
	}

	//1. parse variables type
	if ((symType = parseDecVar_Type(p_par, &pnType, &pnIdent)).type == pn_null)
		return;	

	//2. variable identifier
	if (!parseDecVar_Ident(p_par, &symType, symIdent, setToConst, &pnIdent))
		return;	//error
	
	//optional array subscript
	if (p_par->lexer->getToken()->type == tt_obracket)
		if (!parseDecVar_OBracket(p_par, symIdent, pnIdent.tokens[0].x, pnIdent.tokens[0].y))
			return;	//error

	//now we know if its an array or not, set the stack offset (and symbols stack offset)
	//with the size of this symbol
	symIdent->offset = p_par->sOffset += getSymbolStackSize(*symIdent);

	//3. semi colon, or assignment (optional)
	if (!parseDecVar_Exp(p_par, symIdent, &pnIdent))
		return;

	p_par->parent();
}

//parses the ident in a variable declaration
int wc::parse::parseDecVar_Ident(wcParser* p_par, wcSymbol* p_type, wcSymbol*& p_identSym, bool p_setToConst, wcParseNode* p_identPN)
{
	if (p_par->lexer->getToken()->type != tt_ident)
	{
		p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
		return 0;
	}

	//parse and add the symbol to symbol table
	if ((p_identSym = parseIdentDeclaration(p_par, st_var)) == nullptr)
		return 0;	

	//fill in symbol info
	p_identSym->dataType = p_type->dataType;
	p_identSym->isConst = p_setToConst;
	p_identSym->dataSize = p_type->dataSize;
	p_identSym->size = 1;	//may be changed later if it turns out to be an array
	
	//add the details to the current local stackframe
	p_par->currentStackFrame->localVars.push_back(p_identSym->fullIdent);

	*p_identPN = *p_par->addChild(wcParseNode(pn_ident, p_identSym->fullIdent));

	return 1;
}

int wc::parse::parseDecVar_OBracket(wcParser* p_par, wcSymbol* p_ident, int p_identX, int p_identY)
{
	//consume opening bracket
	p_par->lexer->nextToken();

	//adjust symbol table entry
	p_par->ast.getSymbol(p_ident->fullIdent)->isArray = true;

	//parse expression; type must convert to int, and be const
	wcExpression ex = parseFullExp(p_par);
	if (ex.dataType != "int")
	{
		p_par->setError(ec_p_nonintsubscript, p_identX, p_identY, "Array subscript must be of type \"int\"");
		return 0;
	}
	else if (!ex.isConst)
	{
		p_par->setError(ec_p_expmustbeconst, p_identX, p_identY, ex.rpn);
		return 0;
	}

	//evaluate as a const expression, compile time expressions 
	int expResult = p_par->ast.constTab.at(p_ident->fullIdent).val = evalConstExp(p_par, ex.node);

	//expression should be const, so we know at compile time the dataSize of the array
	p_par->ast.getSymbol(p_ident->fullIdent)->dataSize = expResult * getTypeSize(wcToken(p_par->ast.getSymbol(p_ident->fullIdent)->dataType));

	//consume closing bracket
	p_par->lexer->nextToken();

	return 1;
}

int wc::parse::parseDecVar_Exp(wcParser* p_par, wcSymbol*& p_identSym, wcParseNode* p_identSymPN)
{
	wcExpression ex;
	int expResult;

	switch (p_par->lexer->getToken()->type)
	{
	case tt_scolon:
		//variable declared with no initialising value;
		parseSColon(p_par);
		break;

	case tt_assign:
		//parse as an expression, including the ident we previously parsed (ident = {exp} )
		ex = parseFullExp(p_par, *p_identSymPN);
		if (p_identSym->isConst && !ex.isConst)
		{
			p_par->setError(ec_p_expmustbeconst, p_identSymPN->tokens[0].x, p_identSymPN->tokens[0].y, ex.rpn);
			return 0;
		}
		
		//expResult = p_par->ast.constTab.at(p_identSym->fullIdent).val = evalConstExp(p_par, ex.node);

		//parse remaining semi colon
		parseSColon(p_par);
		break;

	default:
		p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
		return 0;
	}


	return 1;
}

void wc::parse::parseDecFunc(wcParser* p_par)
{
	wcSymbol sym;
	wcFuncInfo fi;
	//make a note of previous stackframe etc
	wcSymbol* oldScope = p_par->currentScope;
	wcStackFrameInfo* oldSf = p_par->currentStackFrame;
	int oldOffset = p_par->sOffset;
	p_par->sOffset = 0;

	//0. function keyword
	switch (p_par->lexer->getToken()->type)
	{
	case tt_function:
		p_par->addNode(wcParseNode(pn_funcdec, *p_par->lexer->getToken()));
		p_par->lexer->nextToken();
		p_par->currentFunc = &fi;
		break;
	default:
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	}
	
	//1. parse the type - either builtin or user defined
	parseDecFunc_Type(p_par,&fi,&sym);
	
	//2. function ident
	parseDecFunc_Ident(p_par,&fi,&sym);

	//3. paramlist
	parseDecFunc_ParamList(p_par);

	//4. (optional) function body
	switch (p_par->lexer->getToken()->type)
	{
	case tt_obrace:
		p_par->noDecFunc = true;
		parseBlock(p_par);
		p_par->sOffset = oldOffset;
		p_par->noDecFunc = false;
		break;
	default:
		break;
	}
	
	//add stackframe the currentFunc info, using the current method signature
	fi.stackFrame = *p_par->currentStackFrame;
	p_par->ast.funcTab.insert(std::make_pair(sym.fullIdent, fi));

	//change scopes back 
	p_par->currentScope = oldScope;
	p_par->currentStackFrame = oldSf;
	p_par->currentFunc = nullptr;
	p_par->currentParamList = nullptr;
	p_par->sOffset = oldOffset;
	p_par->parent();
}


void wc::parse::parseDecFunc_Type(wcParser* p_par, wcFuncInfo* p_fi,wcSymbol* p_sym)
{
	wcParseNode pni;

	switch (p_par->lexer->getToken()->type)
	{
		//basic types
	CASE_BASIC_TYPES_TT
		p_par->addChild(wcParseNode(pn_type, *p_par->lexer->getToken()));
		p_fi->dataType = p_par->lexer->getToken()->data;
		p_par->lexer->nextToken();
		break;

		//user type
	case tt_ident:
		pni = parseIdent(p_par);
		if (p_par->ast.getSymbol(pni.tokens[0].data) == nullptr)
			//error unknown symbol
			return p_par->setError(ec_p_undeclaredsymbol, p_par->lexer->getToken()->data);		

		//and check the identifier relates to a user defined type
		p_sym = p_par->ast.getSymbol(pni.tokens[0].data);
		if (p_sym->type != st_type)
			//unknow error mandem, expected a type here
			return p_par->setError(ec_p_undeclaredsymbol, p_par->lexer->getToken()->data);
		
		p_fi->dataType = p_sym->fullIdent;
		p_par->addChild(wcParseNode(pn_type, p_sym->fullIdent));
		break;

	default:
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	}
}

void wc::parse::parseDecFunc_Ident(wcParser* p_par,wcFuncInfo* p_fi, wcSymbol* p_sym)
{
	switch (p_par->lexer->getToken()->type)
	{
	case tt_ident:
		*p_sym = resolveIdent(p_par, lexIdent(p_par));
		if (p_sym->type == st_null)
		{
			//fill in symbol information
			p_sym->type = st_function;
			p_sym->dataType = p_fi->dataType;
			p_fi->ident = p_sym->ident;
			p_fi->fullIdent = p_sym->fullIdent;
			p_par->ast.addSymbol(p_sym->fullIdent, p_sym);
			p_par->currentFunc->isOverloaded = false;
		}
		else if (p_sym->type == st_function)
		{
			//paramlist must be different or its a redefinition, make available the current paramlist
			p_par->currentFunc = &p_par->ast.funcTab.at(p_sym->fullIdent);
			p_par->currentFunc->isOverloaded = true;
		}
		else
		{
			//redef - used elsewhere, not as a function name
		}

		//set our current scope to this function, using method signature
		p_par->currentScope = p_par->ast.getSymbol(p_sym->fullIdent);

		//new stackframe
		p_fi->sfIndex = p_par->ast.stackFrames.size();
		p_par->ast.stackFrames.push_back(wcStackFrameInfo());
		p_par->currentStackFrame = &p_par->ast.stackFrames.at(p_par->ast.stackFrames.size() - 1);
		p_par->addChild(wcParseNode(pn_funcident, p_sym->fullIdent));
		break;

	default:
		//error
		return p_par->setError(ec_p_undeclaredsymbol, p_par->lexer->getToken()->data);
		break;
	}
}
void wc::parse::parseDecFunc_ParamList(wcParser* p_par)
{
	switch (p_par->lexer->getToken()->type)
	{
	//parameter list
	case tt_oparen:
		parseParamListDec(p_par);
		break;
	//error
	default:
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	}
}

void wc::parse::parseParamListCall(wcParser* p_par, wcSymbol* id)
{
	wcParamList cpl;
	wcParseNode pni;
	wcSymbol sym;
	wcExpression ex;

	//get our opening parenthesis (
	if (p_par->lexer->getToken()->type != tt_oparen)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addNode(wcParseNode(pn_paramlist));

	//loop thru all the parameters in the func call, all parsed as expressions
	while (p_par->lexer->getToken()->type != tt_cparen)
	{
		if (p_par->lexer->getToken()->type != tt_comma)
		{
			//parse the expression
			ex = parseFullExp(p_par);
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
	//if (!checkForOverload(p_par, &cpl, id))
	//	return p_par->setError(ec_p_badparams, p_par->lexer->getToken()->data);

	//consume cparen
	p_par->lexer->nextToken();
	p_par->parent();
}

//this code shud be much the same as parseDecVar
void wc::parse::parseParamListDec(wcParser* p_par)
{
	wcParseNode pni, pnt;
	wcParamList pl = *new wcParamList();
	wcSymbol symt, symi;

	//get our opening parenthesis (
	if (p_par->lexer->getToken()->type != tt_oparen)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	tree<wcParseNode>::iterator plnode = p_par->addNode(wcParseNode(pn_decparamlist));

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
			CASE_BASIC_TYPES_TT
				symt = *p_par->ast.getSymbol(p_par->lexer->getToken()->data);
				pl.params.push_back(symt.fullIdent);
				p_par->addChild(pnt = wcParseNode(pn_type, *p_par->lexer->getToken()));
				p_par->lexer->nextToken();
				break;

			case tt_ident:
				pni = parseIdent(p_par);
				symt = *p_par->ast.getSymbol(pni.tokens[0].data);
				if (symt.type != st_type)
					return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);	//must be a type
				pl.params.push_back(symt.fullIdent);
				p_par->addChild(pnt = wcParseNode(pn_type, symt.fullIdent));
			}

			//ident
			symi = resolveIdent(p_par, lexIdent(p_par));
			if (symi.type != st_null)
				return p_par->setError(ec_p_redefinition, symi.ident);
			symi.type = st_var;
			symi.dataType = symt.dataType;

			//add parameters symbol
			p_par->ast.addSymbol(symi.fullIdent, &symi);
			p_par->ast.getSymbol(symi.fullIdent)->isConst = false;
			if (!(p_par->ast.getSymbol(symi.fullIdent)->isArray = symt.isArray))
				p_par->ast.getSymbol(symi.fullIdent)->dataSize = 1;

			//make sure to adjust the stack offset with the size of this variable
			p_par->ast.getSymbol(symi.fullIdent)->offset = p_par->sOffset;
			p_par->sOffset += getTypeSize(pnt.tokens[0])*p_par->ast.getSymbol(symi.fullIdent)->dataSize;

			p_par->addChild(wcParseNode(symi.type));
		}

	//consume cparen, add to current wcFuncInfo
	p_par->currentFunc->params =  pl;
	p_par->currentParamList = &p_par->currentFunc->params;
	plnode.node->data.tokens.push_back(wcToken(getMethodStringSignature(&pl)));
	p_par->lexer->nextToken();
	p_par->parent();
}


void wc::parse::parseSColon(wcParser* p_par)
{
	//semi colon 
	if (p_par->lexer->getToken()->type != tt_scolon)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
}

void wc::parse::parseBlock(wcParser* p_par)
{
	//opening brace
	if (p_par->lexer->getToken()->type != tt_obrace)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();

	//if were currently declaring a function, or (currentFunc!=nullptr), make a note of the body
	p_par->addNode(wcParseNode(pn_block));
	if (p_par->currentFunc)
		p_par->currentFunc->body = p_par->getNode();

	//inner contents
	while (!p_par->error && p_par->lexer->getToken()->type != tt_cbrace)
		parseStatement(p_par);
	if (p_par->error)	return;

	//closing brace
	p_par->lexer->nextToken();
	p_par->parent();
}

void wc::parse::parseIf(wcParser* p_par)
{
	bool blocks = false;

	//opening if
	if (p_par->lexer->getToken()->type != tt_if)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addNode(wcParseNode(pn_if));

	//conditional expression
	parseFullExp(p_par);

	//true block/statement
	p_par->addNode(wcParseNode(pn_if_trueblock));
	if (p_par->lexer->getToken()->type == tt_obrace)
	{
		blocks = true;
		parseBlock(p_par);
	}
	else
	{
		parseStatement(p_par);
	}
	p_par->parent();
	
	//optional else
	if(p_par->lexer->getToken()->type == tt_else)
	{
		p_par->addNode(wcParseNode(pn_if_elseblock));
		p_par->lexer->nextToken();
		if (blocks)
			parseBlock(p_par);
		else
			parseStatement(p_par);
		p_par->parent();
	}
	p_par->parent();
}

void wc::parse::parseReturn(wcParser* p_par)
{
	//return keyword
	if (p_par->lexer->getToken()->type != tt_return)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addChild(wcParseNode(pn_return));

	//return expression
	parseFullExp(p_par);

	//semi colon
	parseSColon(p_par);
}

void wc::parse::parseBreak(wcParser* p_par)
{
	//break keyword
	if (p_par->lexer->getToken()->type != tt_break)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addChild(wcParseNode(pn_break));

	//semi colon
	parseSColon(p_par);
}

void wc::parse::parseContinue(wcParser* p_par)
{
	//continue keyword
	if (p_par->lexer->getToken()->type != tt_continue)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addChild(wcParseNode(pn_continue));

	//semi colon
	parseSColon(p_par);
}

//increment lexer until we find the end of the identifier
//return as a string, empty on error 
string wc::parse::lexIdent(wcParser* p_par)
{
	//first token must be an ident, cant open with :: or .
	wcToken* tok = p_par->lexer->getToken();
	if (tok && tok->type != tt_ident)
	{
		//error
		p_par->setError(ec_p_illegalidentcall, tok->data);
		return "";
	}
	string id = tok->data;
	
	//handle a possible fully qualified name
	bool exit = false;	wcTokenType thisType, lastType = tok->type;
	while (!exit && (tok = p_par->lexer->nextToken()))
		//:: or .
		switch ((thisType = p_par->lexer->getToken()->type))
		{
		case tt_period:
		case tt_dcolon:
		case tt_colon:	//delimeters cant pre/procede another delim
			if (lastType != tt_ident)
			{
				p_par->setError(ec_p_illegalidentcall, id + tok->data);
				return "";
			}
			id += tok->data;
			p_par->lexer->nextToken();
			break;

		case tt_ident:	//can only follow a delimeter
			if (lastType == tt_ident)
			{
				p_par->setError(ec_p_illegalidentcall, id + tok->data);				
			}
			id += tok->data;
			p_par->lexer->nextToken();
			break;

		default:	//any other token type, or eos
			exit = true;
		}
	return id;
}

//parses an identifier, including namespaces and members
//returns the parsenode, doesnt add it to the tree
//if the ident is an array, the [] brackets are not parsed by this
wcParseNode wc::parse::parseIdent(wcParser* p_par)
{
	//increment the lexer to the end of the ident
	string p_sIdent = lexIdent(p_par);
	wcSymbol sym = resolveIdent(p_par, p_sIdent);

	//make note of the fullident and stack offset for the code generator
	wcParseNode pn(sym.type);
	pn.tokens.push_back(wcToken(tt_ident, sym.fullIdent));																
	pn.tokens.push_back(wcToken(tt_intlit, wcitos(sym.offset)));
	
	return pn;
}

//parses a declaration of an ident, but doesnt add it to the AST
//if it doesnt exist, its ok to make the declaration and adds it to symbol table
//returns null if ident already exists, and a pointer to the entry in the symbol table otherwise
wcSymbol* wc::parse::parseIdentDeclaration(wcParser* p_par, wcSymbolType ty)
{
	wcSymbol sym = resolveIdent(p_par, lexIdent(p_par));
	if (sym.type != st_null)
		if (p_par->ast.getSymbol(getFullIdent(sym.ident, p_par->currentScope))
			&& p_par->ast.getSymbol(getFullIdent(sym.ident, p_par->currentScope))->type != st_null)
		{
			//same identifier exists in this scope
			p_par->setError(ec_p_redefinition, sym.ident);
			return nullptr;	//redefinition
		}

	//no ident of that name in current scope, declare variable
	sym.fullIdent = getFullIdent(sym.ident, p_par->currentScope);
	sym.type = ty;
	p_par->ast.addSymbol(&sym);

	return p_par->ast.getSymbol(sym.fullIdent);
}

void wc::parse::parseArrayIndex(wcParser* p_par, wcParseNode* pn)
{
	wcExpression ex;
	int ix, iy;
	switch (p_par->lexer->getToken()->type)
	{
	case tt_obracket:
		ix = p_par->lexer->getToken()->x;
		iy = p_par->lexer->getToken()->y;
		p_par->lexer->nextToken();

		//expect to parse an expression, check for error, array ref must be const int
		ex = parseFullExp(p_par);
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

		//consume closing bracket
		p_par->lexer->nextToken();
	default:
		//error
		break;
	}
}

//checks if p_shortIdent is a valid symbol in current scope
//null symbol returned if ident cant be found in any previous scope (sym.type == st.null)
wcSymbol wc::parse::resolveIdent(wcParser* p_par, string p_shortIdent)
{
	string buff, fullid;
	int ind;

	//return a null type symbol if the p_shortIdentent doesnt resolve at any scope
	wcSymbol sym;	sym.type = st_null;	sym.ident = p_shortIdent;
	sym.fullIdent = getFullIdent(p_shortIdent, p_par->currentScope);

	//deal with explicit identifiers (with scope/member prefix)
	if (isIdentExplicit(p_shortIdent))
	{
		if (p_par->ast.getSymbol(p_shortIdent))
			return *p_par->ast.getSymbol(p_shortIdent);
		return sym;
	}
	else
	{
		fullid = getFullIdent(p_shortIdent, p_par->currentScope);
		ind = fullid.size() - 1;
	}

	//check all parent scopes as well, backwards to $global
	wcSymbol sc;
	while (ind > 0)
		switch (fullid[ind])
		{
			//two steps back if we get two colons
		case ':':
			if (ind > 0 && fullid[ind - 1] == ':')
				--ind;
		case '.':	//member
			--ind;
			sc = *p_par->ast.getSymbol(fullid.substr(0, ind + 1));	//get scope
			if (p_par->ast.getSymbol(getFullIdent(p_shortIdent, &sc)))
				return *p_par->ast.getSymbol(getFullIdent(p_shortIdent, &sc));
			break;
		default:
			--ind;
		}
	return sym;
}

//make a fully qualified name as a string from a given shortident and scope
string wc::parse::getFullIdent(string p_sIdent, wcSymbol* p_scope)
{
	if (!isIdentExplicit(p_sIdent))
		switch (p_scope->type)
		{
		case st_namespace:
			return p_scope->fullIdent + "::" + p_sIdent;
		case st_function:
			return p_scope->fullIdent + ":" + p_sIdent;
		default:
			return "";
		}
	return p_sIdent;//actually explicit
}

//check whether a given string contains any scope or members ie (main:arg1 or string)
bool wc::parse::isIdentExplicit(string id)
{
	string shortid;
	wcSymbol sc;
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
string wc::parse::getShortIdent(string p_fid)
{
	//work our way backwards from the end of the string
	//until we find a : or .    (this wont work for member variables)
	int i = p_fid.size() - 1;
	while (i > 0)
	{
		if (p_fid[i] == ':' || p_fid[i] == '.')
			return p_fid.substr(i + 1, p_fid.size() - i);
		i--;
	}
	return p_fid;
}

void wc::parse::parseFuncCall(wcParser* p_par, wcParseNode p_funcIdentPN)
{
	//ident must be a declared function symbol, add its fq symbol to the parsenode
	wcSymbol sym = *p_par->ast.getSymbol(p_funcIdentPN.tokens[0].data);

	if (sym.type != st_function)
		//probably the variable name was already used elswhere
		return p_par->setError(ec_p_invalidsymbol, p_par->lexer->getToken()->data);
	else if (sym.type == st_null)
		//unknown identifier
		return p_par->setError(ec_p_undeclaredsymbol, p_par->lexer->getToken()->data);

	//add the functions stack index to the parsenode
	string sfindexstr = wcitos(p_par->ast.funcTab.at(sym.fullIdent).sfIndex);	//string version
	wcToken sfindextok(tt_intlit, sfindexstr);									//token version
	p_funcIdentPN.type = pn_funccall; p_funcIdentPN.tokens.push_back(sfindextok);
	p_par->addNode(p_funcIdentPN);

	//params
	parseParamListCall(p_par, p_par->ast.getSymbol(sym.fullIdent));
	
	p_par->parent();
}

void wc::parse::parseWhile(wcParser* p_par)
{
	//while keyword
	if (p_par->lexer->getToken()->type != tt_while)
		return p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data);
	p_par->lexer->nextToken();
	p_par->addChild(wcParseNode(pn_while));

	//condition
	parseFullExp(p_par);

	//block to execute
	parseBlock(p_par);
}

//full expression - only this method makes the pn_exp parse node
wcExpression wc::parse::parseFullExp(wcParser* p_par)
{
	tree<wcParseNode>::iterator exnode = p_par->addNode(wcParseNode(pn_exp));
	wcExpression ex = parseExp(p_par);
	ex.node = exnode;
	p_par->parent();
	return ex;
}

//for when weve already consumed the identifier with parseIdent(), copy code from parseSubExp
wcExpression wc::parse::parseFullExp(wcParser* p_par, wcParseNode p_id)
{
	wcExpression ex; 
	ex.isConst = true;
	ex.node = p_par->addNode(wcParseNode(pn_exp));

	//taken from parseFactor
	wcSymbol sym = *p_par->ast.getSymbol(p_id.tokens[0].data);
	switch (sym.type)
	{
	case st_var: case st_function:
		ex.dataType = sym.dataType;
		break;
	default:
		p_par->setError(ec_p_invalidsymbol, p_par->lexer->getToken()->data);
		return ex;
	}
	p_par->addChild(p_id);

	return parseFullExp_OuterExpression(p_par, p_id, ex);
}

//we encountered an ident as the first part of the expression, this func parses the rest of the expression
wcExpression wc::parse::parseFullExp_OuterExpression(wcParser* p_par, wcParseNode p_id, wcExpression p_ex)
{
	wcToken op;
	wcParseNode pn;
	while (p_par->lexer->getToken())
	{
		op = *p_par->lexer->getToken();
		switch (op.type)
		{
		CASE_ALL_BOOLEAN_OPERATORS_TT	
		case tt_assign:
			pn = wcParseNode(op.type, op, p_par->ast.getSymbol(p_id.tokens[0].data)->fullIdent);
			pn.tokens.at(pn.tokens.size() - 1).type = tt_lvalue;	//denote that its an l-value
			p_par->addChild(pn);
			p_par->lexer->nextToken();
			break;
		default:
			p_par->parent();
			return p_ex;
		}
		parseSubExp(p_par, &p_ex);
	}
	p_par->parent();
	return p_ex;
}

wcExpression wc::parse::parseExp_OuterExpression(wcParser* p_par, wcToken p_oper1, wcExpression p_ex)
{
	wcToken op, oper2, tok;
	wcParseNode pn;

	while (p_par->lexer->getToken())
	{
		op = *p_par->lexer->getToken();
		switch (op.type)
		{
		CASE_ALL_BOOLEAN_OPERATORS_TT
		case tt_assign:
			pn = wcParseNode(op.type);
			pn.tokens.push_back(op);
			//if our previous token was an identifier, make a note of it's stack location. used in ByteCode generation
			if (p_oper1.type == tt_ident)
			{
				tok = p_par->ast.getSymbol(p_par->lastIdent.node->data.tokens[0].data)->fullIdent;
				p_par->lastIdent.node->data.tokens.at(1).type = tt_lvalue;
				pn.tokens.push_back(tok);
			}
			p_par->addChild(pn);
			p_par->lexer->nextToken();
			break;
		default:
			return p_ex; //function call or one sided expression (ie not a boolean or assignment)
		}
		oper2 = parseSubExp(p_par, &p_ex);
	}
	return p_ex;
}

wcExpression wc::parse::parseExp(wcParser* p_par)
{
	wcExpression ex; 	ex.isConst = true;
	wcToken oper1;

	oper1 = parseSubExp(p_par, &ex);
	ex.dataType = getDatatype(p_par, oper1);
	return parseExp_OuterExpression(p_par,oper1,ex);
}

lex::wcToken wc::parse::parseSubExp(wcParser* p_par, wcExpression* ex)
{
	wcToken oper1, oper2, op;
	oper1 = parseTerm(p_par, ex);
	while (p_par->lexer->getToken())
	{
		op = *p_par->lexer->getToken();
		switch (op.type)
		{
		case tt_plus:	case tt_minus:
			p_par->addChild(wcParseNode(op.type, op));
			p_par->lexer->nextToken();
			break;
		default:
			return oper1;
		}
		oper2 = parseTerm(p_par, ex);
	}
	return oper1;
}

lex::wcToken wc::parse::parseTerm(wcParser* p_par, wcExpression* ex)
{
	wcToken oper1, oper2, op;
	oper1 = parseFactor(p_par, ex);
	while (p_par->lexer->getToken())
	{
		op = *p_par->lexer->getToken();
		switch (op.type)
		{
		case tt_div:	case tt_mult:	case tt_expo:	case tt_mod:
			p_par->addChild(wcParseNode(op.type, op));
			p_par->lexer->nextToken();
			break;
		default:
			return oper1;
		}
		oper2 = parseFactor(p_par, ex);
	}
	return oper1;
}

wcToken wc::parse::parseFactor(wcParser* p_par, wcExpression* ex)
{
	wcExpression ex2;
	wcParseNode pn, pni;
	wcSymbol sym; sym.type = st_null;
	wcToken oper1, oper2, op;
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
	case tt_oparen:	//opening parenthesis
		p_par->parenCount++;
		p_par->addChild(wcParseNode(oper1.type, oper1));
		p_par->lexer->nextToken();

		//parse inner expression, marking a change to non-constness if needed (isConst true by default)
		if (parseExp(p_par).isConst == false && ex->isConst)
			ex->isConst = false;
		
		//closing parenthesis
		if (p_par->lexer->getToken()->type != tt_cparen)
		{
			p_par->setError(ec_p_unexpectedtoken, p_par->lexer->getToken()->data + ", expected \")\"");
			return oper1;	//error
		}
		
		p_par->parenCount--;
		p_par->addChild(wcParseNode(pn_cparen, *p_par->lexer->getToken()));
		p_par->lexer->nextToken();
		break;

	case tt_cparen:
		return oper1;

	case tt_ident:
		pni = parseIdent(p_par);
		if (p_par->ast.getSymbol(pni.tokens[0].data))
			sym = *p_par->ast.getSymbol(pni.tokens[0].data);
		switch (sym.type)
		{
		case st_var:
		case st_function:

			break;
		case st_namespace:
			p_par->setError(ec_p_invalidsymbol, p_par->lexer->getToken()->data);
			return oper1;
		default:
			p_par->setError(ec_p_undeclaredsymbol, p_par->lexer->getToken()->data);
			return oper1;
			break;
		}
		if (!sym.isConst)
			ex->isConst = false;
		if (sym.isArray)
			parseArrayIndex(p_par, &pni);	//the [] subscript
		p_par->lastIdent = p_par->addChild(pni);
		//p_par->lexer->nextToken();  //this broke assignments
		break;

	CASE_ALL_LITERALS_TT		
		p_par->addChild(wcParseNode(oper1.type, oper1));
		p_par->lexer->nextToken();
		break;

	default:
		//p_par->setError(ec_p_unexpectedtoken,p_par->lexer->getToken()->data);
		return oper1;	//error
	}
	return oper1;
}

//HELPER FUNCTIONS
//assumes a valid set of wcSymbol pointaz
//returns each parameter's datatype as a comma seperated string 
string wc::parse::getMethodStringSignature(wcParamList* pl)
{
	string out;
	for (int t = 0; t < pl->params.size(); ++t)
		out += pl->params[t] + ",";
	return out;
}

//checks whether a given ParamList has been declared for the given symbol
bool wc::parse::checkForOverload(wcParser* p_par, wcParamList* pl, wcSymbol* id)
{
	//get function info for the given ident, check for the currently parsed function as well to allow recursion
	if (p_par->currentFunc && p_par->currentFunc->fullIdent == id->fullIdent)
		if (getMethodStringSignature(p_par->currentParamList) == getMethodStringSignature(pl))
			return true;
		//p_par->setError(ec_p_badparams, id->fullIdent + " " + getMethodStringSignature(pl));	
	return false;
		
	wcFuncInfo fi = p_par->ast.funcTab.at(id->fullIdent);
	//check the given paramlist is in there somewhere
//	return (fi.params.find(getMethodStringSignature(pl)) != fi.params.end());
	return false;
}


int wc::parse::getTypeSize(wcToken t)
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

//returns how many push_backs to the runtime stack are required
//to hold this symbol. Assumes that wcSymbol.dataSize has been entered correctly
int wc::parse::getSymbolStackSize(wcSymbol p_sym)
{
	return p_sym.dataSize * p_sym.size;
}

bool wc::parse::checkOperandTypes(wcParser* p_par, wcToken oper1, wcToken op, wcToken oper2)
{
	wcSymbol sym;
	wcToken opers[2];
	string oper_dt[2];
	opers[0] = oper1; opers[1] = oper2;

	//get dataType of each operand as a string
	for (int t = 0; t < 2; ++t)
		if (opers[t].type == tt_ident)
		{
			sym = *p_par->ast.getSymbol(opers[t].data);
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
				oper_dt[t] = p_par->ast.getSymbol(opers[t].data)->dataType;	break;
			case tt_true:	case tt_false:
				oper_dt[t] = p_par->ast.getSymbol("bool")->dataType;	break;
			}
		}

	//banging massive table
	switch (op.type)
	{
		//float/int
	CASE_ALL_ARITHMETIC_TT
		if (oper_dt[0] == "string" || oper_dt[1] == "string")
			return false;
		//string/bool  
	CASE_ALL_BOOLEAN_OPERATORS_TT
		//bools, ints and floats can mix, but strings cannot 
		if (oper_dt[0] == "string" && oper_dt[1] != "string")
			return false;

		break;
	}

	return false;
}

wcSymbolType wc::parse::getTypeFromDataType(wcParser *p_par, std::string p_dt)
{
	if (p_dt == "int")
		return st_type;
	if (p_par->ast.getSymbol(p_dt) != nullptr)
		return (p_par->ast.getSymbol(p_dt)->type);
	return wcSymbolType();
}

string wc::parse::getDatatype(wcParser* p_par, wcToken tk)
{
	switch (tk.type)
	{
	case tt_intlit:
		return p_par->ast.getSymbol("int")->dataType;
	case tt_strlit:
		return p_par->ast.getSymbol("string")->dataType;
	case tt_fltlit:
		return p_par->ast.getSymbol("float")->dataType;
	case tt_true:
	case tt_false:
		return p_par->ast.getSymbol("bool")->dataType;
	case tt_ident:
		if (p_par->ast.getSymbol(getFullIdent(tk.data, p_par->currentScope)))
			return p_par->ast.getSymbol(getFullIdent(tk.data, p_par->currentScope))->dataType;
		return "";
	default:
		return "";
	}
	return "";
}

int wc::parse::getPrecedence(lex::wcToken tokin)
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
int wc::parse::getAssociativity(lex::wcToken tokin)
{
	switch (tokin.type)
	{
	case tt_mod:
	case tt_expo:
		return 1;
	}
	return 0;
}

int wc::parse::isOperator(lex::wcToken tokin)
{
	switch (tokin.type)
	{
	CASE_ALL_OPERATORS_TT
		return true;
	}
	return false;
}


int wc::parse::evalConstExp(wcParser* p_par, tree<wcParseNode>::iterator p_exp)
{
	bool foundParen = false;
	std::vector<wcParseNode*> out, stk;
	int olddepth = p_par->ast.tree.depth(p_exp);	++p_exp;

	evalConstExp(p_par, p_exp, out, stk);
	
	//execute out
	return 1;
}

int wc::parse::evalConstExp(wcParser* p_par, tree<wcParseNode>::iterator p_exp, std::vector<wcParseNode*> out, std::vector<wcParseNode*> stk)
{
	bool foundParen = false;
	int olddepth = p_par->ast.tree.depth(p_exp);

	while (p_par->ast.tree.depth(p_exp) > olddepth)
	{
		switch (p_exp->type)
		{
		case pn_exp:
			evalConstExp(p_par, p_exp, out, stk);
			break;

			//literals
		case pn_strlit:	case pn_fltlit:	case pn_intlit:
		case pn_true:	case pn_false:
			out.push_back(&p_exp.node->data);
			break;

			//variables
		case pn_ident:	case pn_varident:	case pn_funcident:	case pn_funccall:
			out.push_back(&p_exp.node->data);
			//bg->pi.node->data
			break;

			//operators
		case pn_logor:	case pn_logand:	case pn_equal:	case pn_notequal:case pn_lognot:
		case pn_greater:case pn_less:	case pn_lessequal:	case pn_greaterequal:
		case pn_mult:	case pn_div:	case pn_plus:	case pn_minus:		case pn_expo:	case pn_mod:case pn_assign:
			while (
				stk.size() > 0 && (getPrecedence(p_exp.node->data.tokens[0]) == getPrecedence(stk[stk.size() - 1]->tokens[0]) && !getAssociativity(p_exp.node->data.tokens[0]) ||
					getAssociativity(p_exp.node->data.tokens[0]) && getPrecedence(p_exp.node->data.tokens[0]) < getPrecedence(stk[stk.size() - 1]->tokens[0]))
				)
			{
				out.push_back(stk[stk.size() - 1]);
				stk.erase(stk.end() - 1, stk.end());
			}
			stk.push_back(&p_exp.node->data);
			break;

		case pn_oparen:
			stk.push_back(&p_exp.node->data);
			break;

		case pn_cparen:
			foundParen = false;
			//pop stack to ouput til we find a (, error if not
			while (!foundParen && stk.size())
				if (stk[stk.size() - 1]->type == pn_oparen)
				{
					foundParen = true;
				}
				else
				{
					out.push_back(stk[stk.size() - 1]);
					stk.erase(stk.end() - 1, stk.end());
				}

			//mismatched params - error
			if (!foundParen)
				return 0;
			stk.erase(stk.end() - 1, stk.end());

			//func ident
			if (stk.size() && stk[stk.size() - 1]->type == pn_funcident)
			{
				out.push_back(stk[stk.size() - 1]);
				stk.erase(stk.end() - 1, stk.end());
			}
			break;

		case pn_type:
		default:
			break;
		}
		++p_exp;
	}

	//pop off remaining operators
	while (stk.size() > 0)
	{
		out.push_back(stk[stk.size() - 1]);
		stk.erase(stk.end() - 1, stk.end());
	}

	executeRPN(p_par, out);
}

void wc::parse::executeRPN(wcParser* p_par, std::vector<wcParseNode*> p_rpn)
{	
	/*wcParseNode* lastnode;
	std::vector<int> rpnStk;
	while (p_rpn.size())
		switch (p_rpn[0]->type)
		{
		case pn_strlit:
			//bg->addByteCode(oc_push,vt_string,wcstoi(p_rpn[0]->tokens[0].data));
			break;
		case pn_fltlit:
			rpnStk.push_back(wcstoi(p_rpn[0]->tokens[0].data));
			break;
		case pn_intlit:
			rpnStk.push_back(wcstoi(p_rpn[0]->tokens[0].data));
			break;
		case pn_true:
			rpnStk.push_back(1);
			break;
		case pn_false:
			rpnStk.push_back(0);
			break;

			//variables
		case pn_ident:
		case pn_varident:
			//rpnStk.push_back(1); 
			p_par->ast.getSymbol(p_rpn[0]->tokens[0].data)->
			break;
		case pn_funcident: case pn_funccall:
			bg->addByteCode(oc_call, wcstoi(p_rpn[0]->tokens[0].data));
			break;

			//operators
		case pn_lognot:
			bg->addByteCode(oc_not); break;
		case pn_logor:
			bg->addByteCode(oc_or); break;
		case pn_logand:
			bg->addByteCode(oc_and); break;
		case pn_equal:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jne, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_notequal:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_je, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_greater:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jle, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_less:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jge, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_lessequal:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jg, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_greaterequal:
			bg->addByteCode(oc_cmp);
			bg->addByteCode(oc_jl, bg->istream->size() + 3);
			bg->addByteCode(oc_push, 1);
			bg->addByteCode(oc_jmp, bg->istream->size() + 2);
			bg->addByteCode(oc_push, 0); break;
		case pn_assign:
			//if(wcstoi(p_rpn[0]->tokens.at(1).data)>0)
			bg->addByteCode(oc_assign, bg->ast->getSymbol(p_rpn[0]->tokens.at(1).data)->offset);	break;
		case pn_mult:
			bg->addByteCode(oc_mult); break;
		case pn_div:
			bg->addByteCode(oc_div); break;
		case pn_plus:
			bg->addByteCode(oc_plus); break;
		case pn_minus:
			bg->addByteCode(oc_minus); break;
		case pn_mod:
			bg->addByteCode(oc_mod); break;
		case pn_expo:
			bg->addByteCode(oc_expo); break;

			//case pn_ident:
		case pn_type:
		default:
			break;
		}
		lastnode = p_rpn[0];
		p_rpn.erase(p_rpn.begin());
	*/
}

}	//wc::parse
}	//wc