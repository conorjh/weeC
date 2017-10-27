#include "parse.h"

using namespace wc;
using namespace wc::lex;
using namespace wc::parse;
using namespace std;

namespace wc
{
	namespace parse
	{
		int parseStatement(wcParseParams params);

		//declarations
		int parseDec(wcParseParams params);
		int parseDecVar(wcParseParams params);
		int parseDecFunc(wcParseParams params);

		//conditional statements
		int parseIf(wcParseParams params);

		//control statements
		int parseWhile(wcParseParams params);		
		int parseBlock(wcParseParams params);

		//function call
		int parseFuncCall(wcParseParams params);

		//semi colon
		int parseSColon(wcParseParams params);

		//type identifier
		int parseType(wcParseParams params, wcSymbol*& typeSymbolOutput);

		//identifiers
		int parseUnknownIdent(wcParseParams);
		wcSymbol tryParseUnknownIdent(wcParseParams params, bool p_restoreLexIndex);
		int parseKnownIdent(wcParseParams params);
		wcSymbol* tryParseKnownIdent(wcParseParams params, bool p_restoreLexIndex, string* identifierAsSeen);

		//expression parser
		int parseExpression(wcParseParams params);
		wcExpression parseExpressionFull(wcParseParams params);
		wcToken parseSubExpression(wcParseParams params);
		wcToken parseTerm(wcParseParams params);
		wcToken parseFactor(wcParseParams params);
		inline bool parseFactor_oparen(wcParseParams params, wcToken& operandLeft, wcExpression& expression);
		inline bool parseFactor_ident(wcParseParams params, wcToken& operandLeft, wcExpression& expression);
		
		//derivers
		wcParseNodeType deriveParseNodeType(wcToken);
		wcParseNodeType deriveParseNodeType(wcSymbolType);
		wcSymbolType deriveSymbolType(wcTokenType);

		void addBasicTypes(wcSymbolTable& p_symTab);
		wcSymbol createGlobalSymbol();
		wcSymbol createIntSymbol();
		wcSymbol createBoolSymbol();
		wcSymbol createStringSymbol();
		wcSymbol createFloatSymbol();

		int setErrorReturn0(wcError& p_error, wcError p_newError);
		bool setErrorReturnFalse(wcError& p_error, wcError p_newError);
		wcSymbol* setErrorReturnNullPtr(wcError& p_error, wcError p_newError);
		wcSymbol setErrorReturnNullSymbol(wcError& p_error, wcError p_newError);
		wcToken setErrorReturnNullToken(wcError& p_error, wcError p_newError);
		
		bool isSymbolInScope(wcSymbol* p_scope, wcSymbol* p_symbol);
		bool lexIdent(wcParseParams params, wcToken* p_tokenBuffer, bool p_restoreLexIndex, string* p_identifierOutput);
		string createFullyQualifiedIdent(string p_scopeIdent, string p_ident);
		
		//helps with constructor
		const std::unordered_multimap<wcTokenType, wcParseNodeType> tokenType2ParseNodeTypes = {
			{ tt_greater , pn_greater },{ tt_less , pn_less },{ tt_equal , pn_equal },{ tt_notequal , pn_notequal } ,
			{ tt_greaterequal , pn_greaterequal },{ tt_lessequal , pn_lessequal },{ tt_lognot , pn_lognot } ,
			{ tt_logor , pn_logor },{ tt_logand , pn_logand },{ tt_incr , pn_incr },{ tt_decr , pn_decr } ,
			{ tt_plusassign , pn_plusassign },{ tt_minusassign , pn_minusassign },{ tt_multassign , pn_multassign } ,
			{ tt_divassign , pn_divassign },{ tt_intlit , pn_intlit },{ tt_strlit , pn_strlit },{ tt_fltlit , pn_fltlit } ,
			{ tt_ident , pn_ident },{ tt_minus , pn_minus },{ tt_plus , pn_plus },{ tt_div , pn_div } ,
			{ tt_mult ,	pn_mult },{ tt_mod , pn_mod },{ tt_expo , pn_expo },{ tt_assign , pn_assign },
			{ tt_oparen , pn_oparen },{ tt_cparen , pn_cparen }
		};
	}
}

int wc::parse::getPrecedence(wcToken p_token)
{
	switch (p_token.type)
	{
	case tt_assign:
		return 15;
	case tt_logor:
		return 14;
	case tt_logand:
		return 13;
	case tt_equal:	case tt_notequal:
		return 12;
	case tt_greater:	case tt_less:	case tt_lessequal:	case tt_greaterequal:
		return 10;
	case tt_mod:	case tt_expo:
		return 9;
	case tt_mult:	case tt_div:
		return 8;
	case tt_plus:	case tt_minus:
		return 6;
	case tt_lognot:
		return 4;
	case tt_cparen:
		return -1;
	default:
		return 0;
	}
}

bool wc::parse::isRightAssociative(wcToken token)
{
	switch (token.type)
	{
	case tt_mod:
	case tt_expo:
		return true;
	}
	return false;
}

wcSymbol* wc::parse::setErrorReturnNullPtr(wcError& p_error, wcError p_newError)
{
	p_error = p_newError;
	return nullptr;
}

wcSymbol wc::parse::setErrorReturnNullSymbol(wcError& p_error, wcError p_newError)
{
	p_error = p_newError;
	return wcSymbol();
}

wcToken wc::parse::setErrorReturnNullToken(wcError& p_error, wcError p_newError)
{
	p_error = p_newError;
	return wcToken(tt_null);
}

wc::parse::wcSymbol::wcSymbol()
{

}

wc::parse::wcSymbol::wcSymbol(wcTokenType p_tokenType)
{
	type = deriveSymbolType(p_tokenType);
}

wc::parse::wcSymbol::wcSymbol(string p_identifier)
{
	type = deriveSymbolType(deriveTokenType(p_identifier));
	fullyQualifiedIdent = ident = p_identifier;
}

wc::parse::wcSymbol::wcSymbol(wcSymbolType p_type, std::string p_ident, std::string p_fullIdent, bool p_isNamespace, bool p_isArray, bool p_isConst, bool p_isStatic, unsigned int p_size, unsigned int p_dataSize, wcSymbol* p_dataType)
{
	type = p_type;
	ident = p_ident;
	fullyQualifiedIdent = p_fullIdent;
	isNamespace = p_isNamespace;
	isArray = p_isNamespace;
	isConst = p_isConst;
	isStatic = p_isStatic;
	size = p_size;
	dataSize = p_dataSize;
	dataType = p_dataType;
}

wc::parse::wcSymbolTable::wcSymbolTable()
{
	//create global scope
	addSymbol(createGlobalSymbol());

	//basic types
	addBasicTypes(*this);
}

wcSymbol* wc::parse::wcSymbolTable::getSymbol(string p_fullyQualifiedIdent)
{
	if (ident2Symbol.find(p_fullyQualifiedIdent) == ident2Symbol.end())
		return nullptr;
	return &ident2Symbol.find(p_fullyQualifiedIdent)->second;
}

wcSymbol* wc::parse::wcSymbolTable::getSymbol(string p_scopeIdent, string p_fullyQualifiedIdent)
{
	return getSymbol(createFullyQualifiedIdent(p_scopeIdent, p_fullyQualifiedIdent));
}

wcSymbol* wc::parse::wcSymbolTable::getSymbol(wcSymbol* p_scope, string p_ident)
{
	if (getSymbol(p_ident) != nullptr && isSymbolInScope(p_scope, getSymbol(p_ident)))
		return getSymbol(p_ident);
	return nullptr;
}

int wc::parse::wcSymbolTable::addSymbol(wcSymbol p_sym)
{
	if (getSymbol(p_sym.fullyQualifiedIdent) != nullptr)
		return 0;	//symbol already exists
	ident2Symbol.insert(make_pair(p_sym.fullyQualifiedIdent, p_sym));
	return 1;
}

bool wc::parse::wcSymbolTable::isSymbolInScope(wcSymbol* p_scope, wcSymbol* p_symbol)
{
	//if(getSymbol())
	
		return true;
}

int wc::parse::setErrorReturn0(wcError& p_error, wcError p_newError)
{
	p_error = p_newError;
	return 0;
}

bool wc::parse::setErrorReturnFalse(wcError& p_error, wcError p_newError)
{
	p_error = p_newError;
	return false;
}

wcParseNodeType wc::parse::deriveParseNodeType(wcToken p_token)
{
	if(tokenType2ParseNodeTypes.find(p_token.type) == tokenType2ParseNodeTypes.end())
		return pn_null;
	return tokenType2ParseNodeTypes.find(p_token.type)->second;
}

wcParseNodeType wc::parse::deriveParseNodeType(wcSymbolType p_symType)
{
	switch (p_symType)
	{
	default:
	case st_null:
		return pn_null;
	case st_var:
		return pn_varident;
	case st_function:
		return pn_funcident;
	case st_namespace:
		return pn_namespaceident;
	case st_type:
		return pn_type;
	}
}

wcSymbolType wc::parse::deriveSymbolType(wcTokenType p_tokenType)
{
	return st_null;
}

bool wc::parse::isSymbolInScope(wcSymbol* p_scope, wcSymbol* p_symbol)
{

	return false;
}

void wc::parse::addBasicTypes(wcSymbolTable& p_symTab)
{
	p_symTab.addSymbol(createIntSymbol());
	p_symTab.addSymbol(createFloatSymbol());
	p_symTab.addSymbol(createStringSymbol());
	p_symTab.addSymbol(createBoolSymbol());
}

wcSymbol wc::parse::createGlobalSymbol()
{
	wcSymbol sym;
	sym.isArray = false;
	sym.isConst = sym.isStatic = sym.isNamespace = true;
	sym.dataType = &sym;
	sym.ident = sym.fullyQualifiedIdent = "$global";
	sym.type = st_namespace;
	sym.dataSize = 0;
	return sym;
}

wcSymbol wc::parse::createStringSymbol()
{
	wcSymbol sym;
	sym.isArray = sym.isNamespace = false;
	sym.isConst = sym.isStatic = true;
	sym.dataType = &sym;
	sym.ident = sym.fullyQualifiedIdent = "string";
	sym.type = st_type;
	sym.dataSize = 1;
	return sym;
}

wcSymbol wc::parse::createFloatSymbol()
{
	wcSymbol sym;
	sym.isArray = sym.isNamespace = false;
	sym.isConst = sym.isStatic = true;
	sym.dataType = &sym;
	sym.ident = sym.fullyQualifiedIdent = "float";
	sym.type = st_type;
	sym.dataSize = 2;
	return sym;
}

wcSymbol wc::parse::createBoolSymbol()
{
	wcSymbol sym;
	sym.isArray = sym.isNamespace = false;
	sym.isConst = sym.isStatic = true;
	sym.dataType = &sym;
	sym.ident = sym.fullyQualifiedIdent = "bool";
	sym.type = st_type;
	sym.dataSize = 1;
	return sym;
}

wcSymbol wc::parse::createIntSymbol()
{
	wcSymbol sym;
	sym.isArray = sym.isNamespace = false;
	sym.isConst = sym.isStatic = true;
	sym.dataType = &sym;
	sym.ident = sym.fullyQualifiedIdent = "int";
	sym.type = st_type;
	sym.dataSize = 1;
	return sym;
}

string wc::parse::createFullyQualifiedIdent(string p_scopeIdent, string p_ident)
{
	return p_scopeIdent + "::" + p_ident;
}

wc::parse::wcParseNode::wcParseNode()
{

}

wc::parse::wcParseNode::wcParseNode(wcParseNodeType p_type)
{
	type = p_type;
}

wc::parse::wcParseNode::wcParseNode(wcToken p_token)
{
	type = deriveParseNodeType(p_token);
	tokens.push_back(p_token);
}

wc::parse::wcParseNode::wcParseNode(wcParseNodeType p_type, wcToken p_token)
{
	type = p_type;
	tokens.push_back(p_token);
}

wc::parse::wcParseNode::wcParseNode(wcSymbol p_sym, string p_identifierAsSeen)
{
	type = pn_ident;
	tokens.push_back(wcToken(tt_ident, p_sym.fullyQualifiedIdent));
	tokens.push_back(wcToken(tt_varident, p_identifierAsSeen));
}

wc::parse::wcParseData::wcParseData()
{
	parenCount = 0;
}

wc::parse::wcParseParams::wcParseParams(wcParseIndex& p_pindex, wcAST& p_output, wcError& p_error, wcParseData& p_data)
	: pIndex(p_pindex), pOutput(p_output), pError(p_error), pData(p_data)
{
	wcParseData();
}

wc::parse::wcAST::wcAST()
{
	parseTree = tree<wcParseNode>(wcParseNode(pn_head));
	
}

wc::parse::wcAST::wcAST(tree<wcParseNode> p_tree)
{
	parseTree = tree<wcParseNode>(wcParseNode(pn_head));
	parseTree = p_tree;
}

tree<wcParseNode>::iterator wc::parse::wcAST::addNode(wcParseIndex& p_index, wcParseNode p_node)
{
	//append a child to the current node, then set the index to point at that new addition
	p_index.setNode(parseTree.append_child(p_index.getNode(), p_node));

	//return the new addition
	return p_index.getNode();
}

tree<wcParseNode>::iterator wc::parse::wcAST::addChild(wcParseIndex& p_index, wcParseNode p_node)
{
	//append a child to the current node, dont set the index to it though
	parseTree.append_child(p_index.getNode(), p_node);
	
	//then return the new addition
	return parseTree.child(p_index.getNode(), parseTree.number_of_children(p_index.getNode()) - 1);
}

void wc::parse::wcParseIndex::setTokens(vector<wcToken>* p_tokens)
{
	tokenIndex = 0;
	tokens = p_tokens;
}

bool wc::parse::wcParseIndex::isLexIndexValid(int p_newIndex)
{
	if (tokens == nullptr)
		return false;
	return p_newIndex < tokens->size();
}

bool wc::parse::wcParseIndex::isLexIndexValid()
{
	if (tokens == nullptr)
		return false;
	return tokenIndex < tokens->size();
}

wcToken wc::parse::wcParseIndex::getToken()
{
	if(isLexIndexValid())
		return tokens->at(tokenIndex);
	return tokens->at(tokens->size() - 1);
}

wcToken wc::parse::wcParseIndex::getToken(int p_index)
{
	if (isLexIndexValid(p_index))
		return tokens->at(tokenIndex);
	return tokens->at(tokens->size() - 1);
}

wcToken wc::parse::wcParseIndex::nextToken()
{
	tokenIndex++;
	if(isLexIndexValid())
		return getToken();
	
	if (tokens == nullptr)
		return wcToken();
	
	//tokenIndex = tokens->size() - 1;
	return getToken(); 
}

//expect a given token, throw a lex error if we dont encounter it or eos
wcToken wc::parse::wcParseIndex::nextToken(wcTokenType p_expectedType, wcError& p_error)
{
	tokenIndex++;
	if(isLexIndexValid())
		if(getToken().type == p_expectedType)
			return getToken();
		else
		{
			p_error = wcError(ec_lex_unexpectedtoken, getToken().data, getToken().line, getToken().col);
			return wcToken(tt_null);
		}
	else
	{
		p_error = wcError(ec_lex_eos, "", getToken().line, getToken().col);
		return wcToken(tt_null);
	}
}

wcToken wc::parse::wcParseIndex::decToken()
{
	tokenIndex--;
	if (isLexIndexValid())
		return getToken();

	if (tokens == nullptr)
		return wcToken();

	return wcToken();
}

void wc::parse::wcParseIndex::setNode(tree<wcParseNode>::iterator p_it)
{
	node = p_it;
}

void wc::parse::wcParseIndex::setNode(tree<wcParseNode>* p_tree, tree<wcParseNode>::iterator p_it)
{
	parseTree = p_tree;
	node = p_it;
}


tree<wcParseNode>::iterator wc::parse::wcParseIndex::getNode()
{
	return node;
}

tree<wcParseNode>::iterator wc::parse::wcParseIndex::nextNode()
{
	node++;
	return node;
}

tree<wcParseNode>::iterator wc::parse::wcParseIndex::backToParent()
{
	if(node.node->parent != nullptr)
		return (node = node.node->parent);
	return node;
}

int wc::parse::wcParseIndex::getNodeDepth(tree<wcParseNode>::iterator p_node)
{
	if (parseTree == nullptr)
		return -1;
	return parseTree->depth(p_node);
}

int wc::parse::wcParseIndex::getCurrentNodeDepth()
{
	
	return getNodeDepth(node);
}

wc::parse::wcParser::wcParser()
{
	
}

void wc::parse::wcParser::init()
{
	
}

bool wc::parse::wcParser::isError()
{
	return (error.code != ec_null);
}

wcError wc::parse::wcParser::getError()
{
	return error;
}

void wc::parse::wcParser::setError(wcError p_error)
{
	error = p_error;
}

wcAST wc::parse::wcParser::parse(vector<wcToken> p_tokens)
{
	init();

	wcAST output;
	index.setTokens(&p_tokens);
	index.setNode(output.parseTree.begin());
	data.currentScope = output.symTab.getSymbol("$global");

	wcParseParams params(index,output,error,data);
	
	while (index.isLexIndexValid() && !getError().code)
		if (!parseStatement(params))
			return output;

	return output;
}

int wc::parse::parseStatement(wcParseParams params)
{
	//while there are tokens left and no parse errors
	params.pOutput.addNode(params.pIndex,wcParseNode(pn_statement));
	wcSymbol* identSym;
	switch (params.pIndex.getToken().type)
	{
	CASE_BASIC_TYPES_TT			
		parseDecVar(params);
		break;

	case tt_ident: 
		identSym = tryParseKnownIdent(params, true, nullptr);
		if (identSym != nullptr)
			switch (identSym->type)
			{
			case st_function:
			case st_var:
				if (!parseExpression(params))
					return 0;
				if (!parseSColon(params))
					return 0;
				break;
			case st_type:
				parseDecVar(params);
				break;
			default:
				break;
			}
		else
			return setErrorReturn0(params.pError, wcError(ec_par_undeclaredident, params.pIndex.getToken().data, params.pIndex.getToken().line, params.pIndex.getToken().col));
		break;

	CASE_ALL_LITERALS_TT
		if (!parseExpression(params))
			return 0;
		if (!parseSColon(params))
			return 0;
		break;

	case tt_if:
		if (!parseIf(params))
			return 0;
		break;

	default:
		wcToken tok = params.pIndex.getToken();
		return setErrorReturn0(params.pError, wcError(ec_par_unexpectedtoken, tok.data, tok.line, tok.col));
	}

	if (params.pError.code)
		return 0;
	return 1;
}

int wc::parse::parseIf(wcParseParams params)
{
	params.pOutput.addNode(params.pIndex, wcParseNode(pn_if));
	wcToken token;

	//if keyword
	if ((token = params.pIndex.getToken()).type != tt_if)
		return setErrorReturn0(params.pError, wcError(ec_par_unexpectedtoken, token.data, token.line, token.col));
	token = params.pIndex.nextToken();

	//boolean expression within parenthesis
	if ((token = params.pIndex.getToken()).type != tt_oparen)	//opening parenthesis
		return setErrorReturn0(params.pError, wcError(ec_par_unexpectedtoken, token.data, token.line, token.col));
	if (!parseExpression(params))								//expression
		return 0;
	if ((token = params.pIndex.getToken()).type != tt_cparen)	//closing parenthesis
		return setErrorReturn0(params.pError, wcError(ec_par_unexpectedtoken, token.data, token.line, token.col));

	//true block / statement

	//optional else ifs

	//optional else block


	return 1;
}

int wc::parse::parseExpression(wcParseParams params)
{
	params.pOutput.addNode(params.pIndex, wcParseNode(pn_exp));

	wcExpression exp = parseExpressionFull(params);

	params.pIndex.backToParent();

	return 1;
}

wcExpression wc::parse::parseExpressionFull(wcParseParams params)
{
	wcExpression exp = wcExpression();
	wcToken operatorToken, operandRight, operandLeft = parseSubExpression(params);

	while (params.pIndex.isLexIndexValid() && !params.pError.code)
	{
		operatorToken = params.pIndex.getToken();
		switch (operatorToken.type)
		{
		case tt_assign:
		CASE_ALL_BOOLEAN_OPERATORS_TT
			params.pOutput.addChild(params.pIndex, wcParseNode(operatorToken));
			params.pIndex.nextToken();
			break;

		default:
			return exp;	//one sided expression
		}
		operandRight = parseSubExpression(params);
	}
	return exp;
}

wcToken wc::parse::parseSubExpression(wcParseParams params)
{
	wcToken operatorToken, operandRight, operandLeft = parseTerm(params);

	while (params.pIndex.isLexIndexValid() && !params.pError.code)
	{
		operatorToken = params.pIndex.getToken();
		switch (operatorToken.type)
		{
		case tt_plus:	case tt_minus:
			params.pOutput.addChild(params.pIndex, wcParseNode(operatorToken));
			params.pIndex.nextToken();
			break;

		default:
			return operandLeft;
		}
		operandRight = parseTerm(params);
	}
	return operandLeft;
}


wcToken wc::parse::parseTerm(wcParseParams params)
{
	wcToken operandRight, operatorToken, operandLeft = parseFactor(params);

	while (params.pIndex.isLexIndexValid() && !params.pError.code)
	{
		operatorToken = params.pIndex.getToken();
		switch (operatorToken.type)
		{
		case tt_div:	case tt_mult:	case tt_expo:	case tt_mod:
			params.pOutput.addChild(params.pIndex, wcParseNode(operatorToken));
			params.pIndex.nextToken();
			break;

		default:
			return operandLeft;
		}
		operandRight = parseFactor(params);
	}
	return operandLeft;
}

wcToken wc::parse::parseFactor(wcParseParams params)
{
	wcExpression expression;
	bool negate;

	wcToken operandLeft = params.pIndex.getToken();
	if (operandLeft.type == tt_minus)
	{
		negate = true;
		operandLeft = params.pIndex.nextToken();
	}

	switch (operandLeft.type)
	{
	case tt_cparen:
		break;

	case tt_oparen:
		if (!parseFactor_oparen(params, operandLeft, expression))
			return wcToken(tt_null);
		break; 

	case tt_ident:
		if (!parseFactor_ident(params, operandLeft, expression))
			return wcToken(tt_null);
		break;

	CASE_ALL_LITERALS_TT
		params.pOutput.addChild(params.pIndex, wcParseNode(operandLeft));
		params.pIndex.nextToken();
		break;

	default:	//error if we get here
		//params.pOutput.addChild(params.pIndex, wcParseNode(operandLeft));
		setErrorReturnNullToken(params.pError,wcError(ec_par_unexpectedtoken, operandLeft.data, operandLeft.line, operandLeft.col));
	}
	return operandLeft;
}

bool wc::parse::parseFactor_oparen(wcParseParams params, wcToken& operandLeft, wcExpression& expression)
{
	//opening (
	params.pData.parenCount++;
	params.pOutput.addChild(params.pIndex, wcParseNode(operandLeft));
	params.pIndex.nextToken();

	//expression within
	wcExpression innerExpression;
	if ((innerExpression = parseExpressionFull(params)).isError)
		return false;
	if (!innerExpression.isConst)	expression.isConst = false;

	//expect a (
	if (params.pIndex.getToken().type != tt_cparen)
		return setErrorReturnFalse(params.pError, wcError(ec_par_unexpectedtoken, params.pIndex.getToken().data, params.pIndex.getToken().line, params.pIndex.getToken().col));
	params.pData.parenCount--;
	params.pOutput.addChild(params.pIndex, wcParseNode(params.pIndex.getToken()));
	params.pIndex.nextToken();

	return true;
}

bool wc::parse::parseFactor_ident(wcParseParams params, wcToken& operandLeft, wcExpression& expression)
{
	string identifierAsSeen;
	wcSymbol* identifier = tryParseKnownIdent(params, true, &identifierAsSeen);
	if (!identifier)
		return setErrorReturnFalse(params.pError, wcError(ec_par_undeclaredident, identifierAsSeen));

	switch (identifier->type)
	{
	case st_function:
		if (!parseFuncCall(params))
			return false;
		break;

	case st_var:
		if (!parseKnownIdent(params))
			return false;
		break;

	default:
		return setErrorReturnFalse(params.pError, wcError(ec_par_invalidsymbol));
	}

	return true;
}

int wc::parse::parseType(wcParseParams params, wcSymbol*& p_typeSymbolOutput)
{
	wcToken token = params.pIndex.getToken();
	switch (token.type)
	{
	case tt_ident:
		if ((p_typeSymbolOutput = tryParseKnownIdent(params,false,nullptr))!=nullptr)
			return 0;
		params.pOutput.addChild(params.pIndex, wcParseNode(pn_type, wcToken(*p_typeSymbolOutput)));
		params.pIndex.nextToken();
		break;

	CASE_BASIC_TYPES_TT
		params.pOutput.addChild(params.pIndex, wcParseNode(pn_type, token));
		params.pIndex.nextToken();
		p_typeSymbolOutput = params.pOutput.symTab.getSymbol(token.data);
		break;

	default:
		string errorMsg = "Expected a Type Idenfitifer, got " + token.data;
		return setErrorReturn0(params.pError, wcError(ec_par_unexpectedtoken, errorMsg, token.line, token.col));
	}

	return 1;
}

//
//returns false on error
bool wc::parse::lexIdent(wcParseParams params, wcToken* p_tokenBuffer = nullptr, bool p_restoreLexIndex = false, string* p_identifierOutput = nullptr)
{
	if (!p_tokenBuffer)
		p_tokenBuffer = &params.pIndex.getToken();

	//get a string that makes up the whole identifier
	int originalTokenIndexOffset = 1;
	bool breakSwitch = false;
	bool punctuationLexedLast = false;
	while (params.pIndex.isLexIndexValid() && !breakSwitch)
		switch (p_tokenBuffer->type)
		{
		case tt_dcolon:
		case tt_period:
			*p_identifierOutput += p_tokenBuffer->data;
			*p_tokenBuffer = params.pIndex.nextToken();
			originalTokenIndexOffset++;
			punctuationLexedLast = true;
			break;

		case tt_ident:
			*p_identifierOutput += p_tokenBuffer->data;
			*p_tokenBuffer = params.pIndex.nextToken();
			originalTokenIndexOffset++;
			punctuationLexedLast = false;
			break;

		default:
			punctuationLexedLast = false;
			if (!isDelim(p_tokenBuffer->type))
			{
				if (isPunctuation(p_tokenBuffer->type))
					punctuationLexedLast = true;
				*p_identifierOutput += p_tokenBuffer->data;
				*p_tokenBuffer = params.pIndex.nextToken();
				originalTokenIndexOffset++;
				break;
			}
			breakSwitch = true;
		}

	//return index to its original state
	if (p_restoreLexIndex)
		for (int t = 0; t < originalTokenIndexOffset; ++t)
			params.pIndex.decToken();

	return !punctuationLexedLast;
}

//attempt to parse an ident from the lex stream
//if p_restoreLexIndex == true then lexIndex will remain unaltered once this method returns,
//and no errors are thrown
wcSymbol* wc::parse::tryParseKnownIdent(wcParseParams params, bool p_restoreLexIndex = true, string* identifierOutput = nullptr)
{
	//buffers
	string identifier = params.pIndex.getToken().data;
	wcToken openingToken = params.pIndex.getToken();
	wcToken currentToken = params.pIndex.nextToken();

	//if we dont have a user buffer for the identifier as it was seen
	//just use our local scope string as the output.
	if (!identifierOutput)
		identifierOutput = &identifier;
	else
		*identifierOutput = identifier;	//bring the users buffer up to date
	
	//check the ident didnt end with :: or . (malformed identifier)
	if (!lexIdent(params, &currentToken, p_restoreLexIndex, identifierOutput))
		if(p_restoreLexIndex)
			return nullptr;	//malformed ident
		else
			return setErrorReturnNullPtr(params.pError, wcError(ec_par_malformedident, *identifierOutput, openingToken.line, openingToken.col));	//malformed ident

	//check whether ident exists
	wcSymbol* outputSymbol = params.pOutput.symTab.getSymbol(createFullyQualifiedIdent(params.pData.currentScope->fullyQualifiedIdent, *identifierOutput));
	if (outputSymbol == nullptr)
		return nullptr;

	//must be valid at this point
	return outputSymbol;
}

//parse the already declared ident, produce an ident node and add it to the tree
int wc::parse::parseKnownIdent(wcParseParams params)
{
	//make sure the first token is an ident
	if (params.pIndex.getToken().type != tt_ident)
		return setErrorReturn0(params.pError, wcError(ec_par_unexpectedtoken, "", params.pIndex.getToken().line, params.pIndex.getToken().col));

	//must be valid
	string identifierAsSeen;
	wcSymbol* identSymbol = tryParseKnownIdent(params, true, &identifierAsSeen);
	if(identSymbol == nullptr)
		return setErrorReturn0(params.pError, wcError(ec_par_unexpectedtoken, "", params.pIndex.getToken().line, params.pIndex.getToken().col));
	
	//do it again but dont restore 
	identSymbol = tryParseKnownIdent(params, false);
	params.pOutput.addChild(params.pIndex, wcParseNode(*identSymbol, identifierAsSeen));

	return 1;
}

//attempt to parse an undeclared identifier, reset the lexIndex afterwards, return a wcSymbol based on the identifier
wcSymbol wc::parse::tryParseUnknownIdent(wcParseParams params, bool p_restoreLexIndex = true)
{
	//buffers
	string identifier = params.pIndex.getToken().data;
	wcToken openingToken = params.pIndex.getToken();
	wcToken currentToken = params.pIndex.nextToken();

	//check the ident didnt end with :: or . (malformed identifier)
	if (!lexIdent(params, &currentToken, p_restoreLexIndex, &identifier))
		if (p_restoreLexIndex)
			return nullptr;	//malformed ident
		else
			return setErrorReturnNullSymbol(params.pError, wcError(ec_par_malformedident, identifier, openingToken.line, openingToken.col));	//malformed ident

	return wcSymbol(identifier);
}

//checks whether the tokens make a valid new ident name
int wc::parse::parseUnknownIdent(wcParseParams params)
{
	//make sure the first token is an ident
	if (params.pIndex.getToken().type != tt_ident)
		return setErrorReturn0(params.pError, wcError(ec_par_unexpectedtoken, params.pIndex.getToken().data, params.pIndex.getToken().line, params.pIndex.getToken().col));

	//must be valid
	wcSymbol identSymbol = tryParseUnknownIdent(params, true);
	if (identSymbol.ident == "")
		return setErrorReturn0(params.pError, wcError(ec_par_unexpectedtoken, "", params.pIndex.getToken().line, params.pIndex.getToken().col));

	//do it again but dont restore 
	identSymbol = tryParseUnknownIdent(params, false);
	params.pOutput.addChild(params.pIndex, wcParseNode(identSymbol, identSymbol.ident));

	return 1;
}

//Declaration of some kind
int wc::parse::parseDec(wcParseParams params)
{
	if(params.pIndex.getToken().type == tt_function)
		return parseDecFunc(params);
	return parseDecVar(params);
}

int wc::parse::parseDecFunc(wcParseParams params)
{
	//function keyword

	//function identifier

	//declaration parameters

	//function body

	
	return 1;
}

//variable declaration
int wc::parse::parseDecVar(wcParseParams params)
{
	params.pOutput.addNode(params.pIndex, wcParseNode(pn_decvar));

	//type 
	wcSymbol* typeSymbol = nullptr;
	if (!parseType(params,typeSymbol))
		return 0;

	//identifier
	wcSymbol identSymbol;
	identSymbol = tryParseUnknownIdent(params);
	identSymbol.fullyQualifiedIdent = createFullyQualifiedIdent(params.pData.currentScope->fullyQualifiedIdent, identSymbol.ident);
	if (!parseUnknownIdent(params))
		return 0;

	//semi colon / opt. assignment operator 
	switch (params.pIndex.getToken().type)
	{
	case tt_scolon:
		if (!parseSColon(params))
			return 0;
		goto registerSymbol;

	case tt_assign:
		params.pIndex.nextToken();
		break;
	default:
		return setErrorReturn0(params.pError, wcError(ec_par_unexpectedtoken, params.pIndex.getToken().data, params.pIndex.getToken().line, params.pIndex.getToken().col));
	}

	//opt. expression
	if (!parseExpression(params))
		return 0;
	if (!parseSColon(params))
		return 0;

	registerSymbol:
	params.pIndex.backToParent();
	wcSymbol newSymbol(st_var, identSymbol.ident, identSymbol.fullyQualifiedIdent, 
		false, false, false, false, 1, typeSymbol->dataSize, typeSymbol);
	params.pOutput.symTab.addSymbol(newSymbol);

	return 1;
}

int wc::parse::parseFuncCall(wcParseParams params)
{

	return 1;
}

int wc::parse::parseSColon(wcParseParams params)
{
	//check for eos
	if (!params.pIndex.isLexIndexValid())
		return setErrorReturn0(params.pError, wcError(ec_par_eos, "Unexpected end of stream"));

	//expect a semi colon
	wcToken token = params.pIndex.getToken();
	if (token.type != tt_scolon)
		return setErrorReturn0(params.pError, wcError(ec_par_unexpectedtoken, "Expected ';' (tt_scolon), got " + token.data, token.line, token.col));

	//simples
	params.pIndex.nextToken();
	return 1;
}

