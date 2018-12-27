#include "parse.h"

using namespace wc;
using namespace wc::lex;
using namespace wc::parse;
using namespace wc::error;
using namespace std;

namespace wc
{
	namespace parse
	{
		int parseStatement(wcParseParams params, bool onlyAllowDeclarations = false, bool allowNamespaceDec = true, bool allowAnyDeclarations = true);

		//declarations
		int parseDec(wcParseParams params);
		int parseDec_Func(wcParseParams params, wcSymbol*,wcSymbol);
		int parseParameterList(wcParseParams params, wcParamList& paramList);
		int parseParameterList_Dec(wcParseParams params);
		int parseNamespace(wcParseParams params);

		//conditional statements
		int parseIf(wcParseParams params);

		//control statements
		int parseWhile(wcParseParams params);		
		int parseBlock(wcParseParams params, bool allowStatements = true, bool disallowNamespaceDec = false, bool allowAnyDeclarations = false);
		int parseReturn(wcParseParams params);
		int parseSColon(wcParseParams params);

		//function call
		int parseFuncCall(wcParseParams params);

		//type identifier
		int parseType(wcParseParams params, wcSymbol*& typeSymbolOutput);

		//identifiers
		int parseUnknownIdent(wcParseParams);
		wcSymbol tryParseUnknownIdent(wcParseParams params, bool p_restoreLexIndex = true, wcSymbolType p_symbolTypeHint = st_null);
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
		bool lexIdent(wcParseParams params, wcToken* p_tokenBuffer = nullptr, bool p_restoreLexIndex = false, string* p_identifierOutput = nullptr);
		string createFullyQualifiedIdent(string p_scopeIdent, string p_ident);
		string createInternalFuncName(wcSymbol p_ident, wcParamList& p_paramList);
		bool isEOS(wcParseIndex& pIndex, wcError& pError);
		
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

		const char* parse_internalFuncNameSeprator = "@@";
		static wcLexer parsingLexer;		
	}
}

wc::parse::wcFunctionTable::wcFunctionTable()
{

}

int wc::parse::wcFunctionTable::addSymbol(std::string internalFunctionName, wcFunctionInfo p_sym)
{
	return 0;
}

int wc::parse::getPrecedence(wcToken p_token)
{
	switch (p_token.type)
	{
	case tt_assign:
		return 1;
	case tt_logor:
		return 2;
	case tt_logand:
		return 3;
	case tt_equal:	case tt_notequal:
		return 4;
	case tt_greater:	case tt_less:	case tt_lessequal:	case tt_greaterequal:
		return 5;
	case tt_mod:	case tt_expo:
		return 6;
	case tt_plus:	case tt_minus:
		return 7;
	case tt_mult:	case tt_div:
		return 8;
	case tt_lognot:
		return 9;
	case tt_cparen:
		return 10;
	default:
		return -1;
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
	type = deriveSymbolType(parsingLexer.deriveTokenType(p_identifier));
	fullyQualifiedIdent = ident = p_identifier;
}

wc::parse::wcSymbol::wcSymbol(std::string p_identifier, std::string scopeFQIdent)
{
	type = deriveSymbolType(parsingLexer.deriveTokenType(p_identifier));
	fullyQualifiedIdent = createFullyQualifiedIdent(scopeFQIdent, p_identifier);
	ident = p_identifier;
}

wc::parse::wcSymbol::wcSymbol(wcSymbolType p_type, std::string p_identifier, std::string scopeFQIdent)
{
	type = deriveSymbolType(parsingLexer.deriveTokenType(p_identifier));
	fullyQualifiedIdent = createFullyQualifiedIdent(scopeFQIdent, p_identifier);
	ident = p_identifier;
	type = p_type;

	switch (type)
	{
	case st_namespace:
		isStatic = isConst = isNamespace = true;
		stackOffset = isArray = size = dataSize = 0;
		return;
	case st_function:
	case st_var:
		isStatic = isConst = isNamespace = false;
		stackOffset = isArray = size = dataSize = 0;
		return;
	}
}

wc::parse::wcSymbol::wcSymbol(wcSymbolType p_type, std::string p_ident, std::string p_fullIdent, bool p_isNamespace, bool p_isArray, bool p_isConst, bool p_isStatic, unsigned int p_size, unsigned int p_dataSize, int p_stackOffset, wcSymbol* p_dataType)
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
	stackOffset = p_stackOffset;
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

wcSymbol* wc::parse::wcSymbolTable::getSymbol(string p_scopeIdent, string p_shortIdent)
{
	return getSymbol(createFullyQualifiedIdent(p_scopeIdent, p_shortIdent));
}

//returns a symbol wit matcing ident only if it's valid in given scope
wcSymbol* wc::parse::wcSymbolTable::getSymbol(wcSymbol* p_scope, string p_ident)
{
	string fqIdent = createFullyQualifiedIdent(p_scope->fullyQualifiedIdent, p_ident);
	if (getSymbol(fqIdent) != nullptr && isSymbolInScope(p_scope, getSymbol(fqIdent)))
		return getSymbol(fqIdent);
	return nullptr;
}

//TODO
//returns a symbol with matching ident only if it's valid in given scope
wcSymbol* wc::parse::wcSymbolTable::getSymbolFromShortIdent(string p_ident, vector<wcSymbol> openScopes)
{
	//search open scopes to see if the given ident is valid

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

string wc::parse::createInternalFuncName(wcSymbol p_ident, wcParamList& p_paramList)
{
	string output = p_ident.fullyQualifiedIdent;

	for (unsigned int t = 0; t < p_paramList.paramCount(); ++t)
		output += parse_internalFuncNameSeprator + p_paramList.params[t].tableEntry->fullyQualifiedIdent;

	return output;
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

wc::parse::wcParseNode::wcParseNode(wcParseNodeType p_type, wcToken p_token1, wcToken p_token2)
{
	type = p_type;
	tokens.push_back(p_token1);
	tokens.push_back(p_token2);
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
	currentStackIndex = -1;
}

wc::parse::wcParseParams::wcParseParams(wcParseIndex& p_pindex, wcAST& p_output, wcError& p_error, wcParseData& p_data)
	: index(p_pindex), output(p_output), error(p_error), data(p_data)
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
			p_error = wcError(ec_lex_unexpectedtoken, getToken().data, getToken().line, getToken().column);
			return wcToken(tt_null);
		}
	else
	{
		p_error = wcError(ec_lex_eos, "", getToken().line, getToken().column);
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

//parse statement, if onlyAllowDeclarations, everything but function, variable and namespace decs will throw an error
//if allowNamespaceDec is false, Namespace declarations will always throw an error
int wc::parse::parseStatement(wcParseParams params, bool onlyAllowDeclarations, bool allowNamespaceDec, bool allowAnyDeclarations)
{
	if (isEOS(params.index, params.error))
		return 0;

	//while there are tokens left and no parse errors
	params.output.addNode(params.index,wcParseNode(pn_statement));
	wcSymbol* identSym;
	switch (params.index.getToken().type)
	{
	CASE_BASIC_TYPES_TT
		parseDec(params);
	break;

	case tt_ident:
		identSym = tryParseKnownIdent(params, true, nullptr);
		if (identSym != nullptr)
			switch (identSym->type)
			{
			case st_function:
			case st_var:
				if (onlyAllowDeclarations || (!allowAnyDeclarations))
					return setErrorReturn0(params.error, wcError(ec_par_illegalstatement, params.index.getToken()));
				if (!parseExpression(params))
					return 0;
				if (!parseSColon(params))
					return 0;
				break;
			case st_type:
				parseDec(params);
				break;
			default:
				break;
			}
		else
			return setErrorReturn0(params.error, wcError(ec_par_undeclaredident, params.index.getToken()));
		break;

	CASE_ALL_LITERALS_TT
		if (onlyAllowDeclarations || (!allowAnyDeclarations))
			return setErrorReturn0(params.error, wcError(ec_par_illegalstatement, params.index.getToken()));
		if (!parseExpression(params))
			return 0;
		if (!parseSColon(params))
			return 0;
		break;

	case tt_keyword_if:
		if (onlyAllowDeclarations || (!allowAnyDeclarations))
			return setErrorReturn0(params.error, wcError(ec_par_illegalstatement, params.index.getToken()));
		if (!parseIf(params))
			return 0;
		break;

	case tt_keyword_namespace:
		if (allowNamespaceDec && (onlyAllowDeclarations || (allowAnyDeclarations)))
		{
			if (!parseNamespace(params))
				return 0;
		}
		else
			return setErrorReturn0(params.error, wcError(ec_par_illegalstatement, params.index.getToken()));
		break;

	case tt_keyword_return:
		if (onlyAllowDeclarations || (!allowAnyDeclarations))
			return setErrorReturn0(params.error, wcError(ec_par_illegalstatement, params.index.getToken()));
		if (!parseReturn(params))
			return 0;
		break;

	case tt_keyword_while:
		if (onlyAllowDeclarations || (!allowAnyDeclarations))
			return setErrorReturn0(params.error, wcError(ec_par_illegalstatement, params.index.getToken()));
		if (!parseWhile(params))
			return 0;
		break;

	default:
		wcToken tok = params.index.getToken();
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, tok));
	}

	if (params.error.code)
		return 0;
	return 1;
}

int wc::parse::parseIf(wcParseParams params)
{
	if (isEOS(params.index, params.error))
		return 0;
	params.output.addNode(params.index, wcParseNode(pn_if));

	//if keyword
	if (params.index.getToken().type != tt_keyword_if)
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, params.index.getToken()));
	params.index.nextToken();

	//boolean expression within parenthesis
	if (params.index.getToken().type != tt_oparen)	//opening parenthesis
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, params.index.getToken()));
	if (!parseExpression(params))	//expression
		return 0;
	if (params.index.getToken().type != tt_cparen)	//closing parenthesis
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, params.index.getToken()));

	//true block / statement
	if (!parseBlock(params))
		return 0;

	//optional else ifs

	//optional else block


	return 1;
}

int wc::parse::parseExpression(wcParseParams params)
{
	if (isEOS(params.index, params.error))
		return 0;
	params.output.addNode(params.index, wcParseNode(pn_exp));

	wcExpression exp = parseExpressionFull(params);

	params.index.backToParent();

	return 1;
}

wcExpression wc::parse::parseExpressionFull(wcParseParams params)
{
	wcExpression exp = wcExpression();
	wcToken operatorToken, operandRight, operandLeft = parseSubExpression(params);
	string fqOperandLeft;

	while (params.index.isLexIndexValid() && !params.error.code)
	{
		operatorToken = params.index.getToken();
		switch (operatorToken.type)
		{
		case tt_assign:
			//get fully qualified ident for left operand, code generators need it to get correct stackindex;
			fqOperandLeft = params.output.symTab.getSymbol(params.data.currentScope, operandLeft.data)->fullyQualifiedIdent;
			params.output.addChild(params.index, wcParseNode(pn_assign, operatorToken, fqOperandLeft));
			params.index.nextToken();
			break;
		CASE_ALL_BOOLEAN_OPERATORS_TT
			params.output.addChild(params.index, wcParseNode(deriveParseNodeType(operatorToken), operatorToken));
			params.index.nextToken();
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

	while (params.index.isLexIndexValid() && !params.error.code)
	{
		operatorToken = params.index.getToken();
		switch (operatorToken.type)
		{
		case tt_plus:	case tt_minus:
			params.output.addChild(params.index, wcParseNode(operatorToken));
			params.index.nextToken();
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

	while (params.index.isLexIndexValid() && !params.error.code)
	{
		operatorToken = params.index.getToken();
		switch (operatorToken.type)
		{
		case tt_div:	case tt_mult:	case tt_expo:	case tt_mod:
			params.output.addChild(params.index, wcParseNode(operatorToken));
			params.index.nextToken();
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

	wcToken operandLeft = params.index.getToken();
	if (operandLeft.type == tt_minus)
	{
		negate = true;
		operandLeft = params.index.nextToken();
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
		params.output.addChild(params.index, wcParseNode(operandLeft));
		params.index.nextToken();
		break;

	default:	//error if we get here
		setErrorReturnNullToken(params.error,wcError(ec_par_unexpectedtoken, operandLeft.data, operandLeft.line, operandLeft.column));
	}
	return operandLeft;
}

bool wc::parse::parseFactor_oparen(wcParseParams params, wcToken& operandLeft, wcExpression& expression)
{
	//opening (
	params.data.parenCount++;
	params.output.addChild(params.index, wcParseNode(operandLeft));
	params.index.nextToken();

	//expression within
	wcExpression innerExpression;
	if ((innerExpression = parseExpressionFull(params)).isError)
		return false;
	if (!innerExpression.isConst)	expression.isConst = false;

	//expect a (
	if (params.index.getToken().type != tt_cparen)
		return setErrorReturnFalse(params.error, wcError(ec_par_unexpectedtoken, params.index.getToken()));
	params.data.parenCount--;
	params.output.addChild(params.index, wcParseNode(params.index.getToken()));
	params.index.nextToken();

	return true;
}

bool wc::parse::parseFactor_ident(wcParseParams params, wcToken& operandLeft, wcExpression& expression)
{
	string identifierAsSeen;
	wcSymbol* identifier = tryParseKnownIdent(params, true, &identifierAsSeen);
	if (!identifier)
		return setErrorReturnFalse(params.error, wcError(ec_par_undeclaredident, identifierAsSeen));

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
		return setErrorReturnFalse(params.error, wcError(ec_par_invalidsymbol));
	}

	return true;
}

int wc::parse::parseType(wcParseParams params, wcSymbol*& p_typeSymbolOutput)
{
	if (isEOS(params.index, params.error))
		return 0;

	wcToken token = params.index.getToken();
	switch (token.type)
	{
	case tt_ident:
		if ((p_typeSymbolOutput = tryParseKnownIdent(params,false,nullptr))!=nullptr)
			return 0;
		params.output.addChild(params.index, wcParseNode(pn_type, wcToken(*p_typeSymbolOutput)));
		params.index.nextToken();
		break;

	CASE_BASIC_TYPES_TT
		params.output.addChild(params.index, wcParseNode(pn_type, token));
		params.index.nextToken();
		p_typeSymbolOutput = params.output.symTab.getSymbol(token.data);
		break;

	default:
		string errorMsg = "Expected a Type Idenfitifer, got " + token.data;
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, errorMsg, token.line, token.column));
	}

	return 1;
}

//
//returns false on error
bool wc::parse::lexIdent(wcParseParams params, wcToken* p_tokenBuffer, bool p_restoreLexIndex, string* p_identifierOutput)
{
	if (!p_tokenBuffer)
		p_tokenBuffer = &params.index.getToken();

	//get a string that makes up the whole identifier
	int originalTokenIndexOffset = 1;
	bool breakSwitch = false;
	bool punctuationLexedLast = false;
	while (params.index.isLexIndexValid() && !breakSwitch)
		switch (p_tokenBuffer->type)
		{
		case tt_dcolon:
		case tt_period:
			*p_identifierOutput += p_tokenBuffer->data;
			*p_tokenBuffer = params.index.nextToken();
			originalTokenIndexOffset++;
			punctuationLexedLast = true;
			break;

		case tt_ident:
			*p_identifierOutput += p_tokenBuffer->data;
			*p_tokenBuffer = params.index.nextToken();
			originalTokenIndexOffset++;
			punctuationLexedLast = false;
			break;

		default:
			punctuationLexedLast = false;
			if (!parsingLexer.isDelim(p_tokenBuffer->type))
			{
				if (parsingLexer.isPunctuation(p_tokenBuffer->type))
					punctuationLexedLast = true;
				*p_identifierOutput += p_tokenBuffer->data;
				*p_tokenBuffer = params.index.nextToken();
				originalTokenIndexOffset++;
				break;
			}
			breakSwitch = true;
		}

	//return index to its original state
	if (p_restoreLexIndex)
		for (int t = 0; t < originalTokenIndexOffset; ++t)
			params.index.decToken();

	return !punctuationLexedLast;
}

//attempt to parse an ident from the lex stream
//if p_restoreLexIndex == true then lexIndex will remain unaltered once this method returns,
//and no errors are thrown
wcSymbol* wc::parse::tryParseKnownIdent(wcParseParams params, bool p_restoreLexIndex = true, string* identifierOutput = nullptr)
{
	//buffers
	string identifier = params.index.getToken().data;
	wcToken openingToken = params.index.getToken();
	wcToken currentToken = params.index.nextToken();

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
			return setErrorReturnNullPtr(params.error, wcError(ec_par_malformedident, *identifierOutput, openingToken.line, openingToken.column));	//malformed ident

	//check whether ident exists
	wcSymbol* outputSymbol = params.output.symTab.getSymbol(createFullyQualifiedIdent(params.data.currentScope->fullyQualifiedIdent, *identifierOutput));
	if (outputSymbol == nullptr)
		return nullptr;

	//must be valid at this point
	return outputSymbol;
}

//parse the already declared ident, produce an ident node and add it to the tree
int wc::parse::parseKnownIdent(wcParseParams params)
{
	//make sure the first token is an ident
	if (params.index.getToken().type != tt_ident)
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, "", params.index.getToken().line, params.index.getToken().column));

	//must be valid
	string identifierAsSeen;
	wcSymbol* identSymbol = tryParseKnownIdent(params, true, &identifierAsSeen);
	if(identSymbol == nullptr)
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, "", params.index.getToken().line, params.index.getToken().column));
	
	//do it again but dont restore 
	identSymbol = tryParseKnownIdent(params, false);
	params.output.addChild(params.index, wcParseNode(*identSymbol, identifierAsSeen));

	return 1;
}

//attempt to parse an undeclared identifier, reset the lexIndex afterwards, return a wcSymbol based on the identifier
wcSymbol wc::parse::tryParseUnknownIdent(wcParseParams params, bool p_restoreLexIndex, wcSymbolType p_symbolTypeHint)
{
	if (isEOS(params.index, params.error))
		return 0;

	//buffers
	string identifier = params.index.getToken().data;
	wcToken openingToken = params.index.getToken();
	wcToken currentToken = params.index.nextToken();

	//check the ident didnt end with :: or . (malformed identifier)
	if (!lexIdent(params, &currentToken, p_restoreLexIndex, &identifier))
		if (p_restoreLexIndex)
			return nullptr;	//malformed ident
		else
			return setErrorReturnNullSymbol(params.error, wcError(ec_par_malformedident, identifier, openingToken.line, openingToken.column));	//malformed ident
	
	if(p_symbolTypeHint != st_null)
		return wcSymbol(p_symbolTypeHint,identifier, params.data.currentScope->fullyQualifiedIdent);
	return wcSymbol(identifier,params.data.currentScope->fullyQualifiedIdent);
}

//checks whether the tokens make a valid new ident name
int wc::parse::parseUnknownIdent(wcParseParams params)
{
	if (isEOS(params.index, params.error))
		return 0;

	//make sure the first token is an ident
	if (params.index.getToken().type != tt_ident)
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, params.index.getToken()));

	//must be valid
	wcSymbol identSymbol = tryParseUnknownIdent(params, true);
	if (identSymbol.ident == "")
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken,params.index.getToken()));

	//do it again but dont restore lexer
	identSymbol = tryParseUnknownIdent(params, false);
	params.output.addChild(params.index, wcParseNode(identSymbol, identSymbol.ident));

	return 1;
}

//Declaration of some kind
int wc::parse::parseDec(wcParseParams params)
{
	//check for end of stream
	if (isEOS(params.index, params.error))
		return 0;

	//type identifier 
	auto openingNode = params.output.addNode(params.index, wcParseNode(pn_dec));
	wcSymbol* typeSymbol = nullptr;
	if (!parseType(params, typeSymbol))
		return 0;

	//variables identifier
	wcSymbol identSymbol;
	identSymbol = tryParseUnknownIdent(params);
	identSymbol.fullyQualifiedIdent = createFullyQualifiedIdent(params.data.currentScope->fullyQualifiedIdent, identSymbol.ident);
	if (!parseUnknownIdent(params))
		return 0;

	switch (params.index.getToken().type)
	{
	case tt_scolon:
		//variable declaration
		openingNode->type = pn_decvar;
		if (!parseSColon(params))
			return 0; 
		break;

	case tt_assign:
		//variable declaration
		openingNode->type = pn_decvar;
		params.index.nextToken();
		if (!parseExpression(params))
			return 0;
		if (!parseSColon(params))
			return 0;
		break;

	case tt_oparen:
		//function declaration
		openingNode->type = pn_funcdec;
		if (!parseDec_Func(params, typeSymbol, identSymbol))
			return 0;
		return 1;	

	default:
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, params.index.getToken()));
	}

	//must be a variable if we havent branched
	params.data.currentStackIndex++;
	params.index.backToParent();
	wcSymbol newSymbol(st_var, identSymbol.ident, identSymbol.fullyQualifiedIdent,
		false, false, false, false, 1, typeSymbol->dataSize, params.data.currentStackIndex, typeSymbol);
	params.output.symTab.addSymbol(newSymbol);

	return 1;
}

//branches from parseDec() if opening parenthesis are found after identifier
int wc::parse::parseDec_Func(wcParseParams params, wcSymbol* p_typeSymbol, wcSymbol p_identSymbol)
{
	//parameters
	wcParamList paramList;
	if (!parseParameterList(params, paramList))
		return 0;

	//register this function so we have a scope to reference against in the symbol table
	wcSymbol funcSymbol(st_function, p_identSymbol.ident, p_identSymbol.fullyQualifiedIdent,
		false, p_identSymbol.isArray, p_identSymbol.isConst, p_identSymbol.isStatic, 1, p_typeSymbol->dataSize, params.data.currentStackIndex, p_typeSymbol);
	params.output.symTab.addSymbol(funcSymbol);

	//create a new stackframe for this function
	string internalFuncName = createInternalFuncName(p_identSymbol, paramList);
	wcStackframe thisFrame(params.data.currentScope);
	params.output.frames.insert(make_pair(thisFrame.owner->fullyQualifiedIdent, thisFrame));

	//create func info
	wcFunctionInfo funcInfo;
	funcInfo.internalFuncName = internalFuncName;
	funcInfo.symbol = params.output.symTab.getSymbol(internalFuncName);
	funcInfo.paramList = paramList;

	//prototype only
	if (params.index.getToken().type == tt_scolon)
		if (!parseSColon(params))
			return 0;

	//function body present - change current scope
	wcSymbol* oldScope = params.data.currentScope;
	params.data.currentScope = params.output.symTab.getSymbol(funcSymbol.fullyQualifiedIdent);
 	if (!parseBlock(params,true,false))
		return 0;	

	params.index.backToParent();

	return 1;
}

int wc::parse::parseParameterList(wcParseParams params, wcParamList& p_paramList)
{
	if (isEOS(params.index, params.error))
		return 0;

	//make sure the first token is an opening parenthesis
	if (params.index.getToken().type != tt_oparen)
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, "Expected '(' (tt_oparen), got " + params.index.getToken().data, params.index.getToken().line, params.index.getToken().column));
	params.index.nextToken();
	params.output.addNode(params.index, wcParseNode(pn_paramlist));

	while (params.index.getToken().type != tt_cparen)
		if (!parseParameterList_Dec(params))
			return 0;

	//consume closing parenthesis
	params.index.nextToken();

	params.index.backToParent();
	return 1;
}

int wc::parse::parseParameterList_Dec(wcParseParams params)
{
	if (isEOS(params.index, params.error))
		return 0;

	return 1;
}

int wc::parse::parseNamespace(wcParseParams params)
{
	//check for end of stream
	if (isEOS(params.index, params.error))
		return 0;

	//make sure the first token is the namespace reserved word
	if (params.index.getToken().type != tt_keyword_namespace)
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, "Expected 'namespace', got " + params.index.getToken().data, params.index.getToken().line, params.index.getToken().column));
	params.output.addNode(params.index, wcParseNode(pn_namespacedec));
	params.index.nextToken();

	//get namespace name
	wcSymbol identSymbol;
	identSymbol = tryParseUnknownIdent(params,true,st_namespace);
	if (!parseUnknownIdent(params))
		return 0;

	//declare namespace symbol in symtab, set it to be currentScope
	wcSymbol* oldScope = params.data.currentScope;
	params.output.symTab.addSymbol(identSymbol);
	params.data.currentScope = params.output.symTab.getSymbol(identSymbol.fullyQualifiedIdent);

	//parse a block for declarations only
	if (!parseBlock(params,false,true))
		return 0;

	//restore
	params.index.backToParent();
	params.data.currentScope = oldScope;

	return 1;
}

int wc::parse::parseWhile(wcParseParams params)
{
	//check for end of stream
	if (isEOS(params.index, params.error))
		return 0;

	//make sure the first token is 'while'
	if (params.index.getToken().type != tt_keyword_while)
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, "Expected 'while', got " + params.index.getToken().data, params.index.getToken().line, params.index.getToken().column));
	params.index.nextToken();
	params.output.addNode(params.index, wcParseNode(pn_while));
	
	//opening parenthesis
	if (params.index.getToken().type != tt_oparen)	
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, params.index.getToken()));
	//expression
	if (!parseExpression(params))					
		return 0;
	//closing parenthesis
	if (params.index.getToken().type != tt_cparen)	
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, params.index.getToken()));

	//while block, statements only, no declarations
	if (!parseBlock(params, true, false, false))
		return 0;

	return 1;
}

int wc::parse::parseBlock(wcParseParams params, bool allowStatements, bool allowNamespaceDec, bool allowAnyDeclarations)
{
	//check for end of stream
	if (isEOS(params.index, params.error))
		return 0;

	//make sure the first token is an opening brace
	if (params.index.getToken().type != tt_obrace)
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, "Expected '{' (tt_obrace), got " + params.index.getToken().data, params.index.getToken().line, params.index.getToken().column));
	params.index.nextToken();
	params.output.addNode(params.index, wcParseNode(pn_body));

	//parse statements inside the block
	while (params.index.getToken().type != tt_cbrace)
		if (!parseStatement(params, !allowStatements, allowNamespaceDec, allowAnyDeclarations))
			return 0;

	//consume closing brace
	params.index.nextToken();
	params.index.backToParent();
	return 1;
}

int wc::parse::parseReturn(wcParseParams params)
{
	//check for end of stream
	if (isEOS(params.index, params.error))
		return 0;

	//make sure the first token is return
	if (params.index.getToken().type != tt_keyword_return)
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, "Expected 'return', got " + params.index.getToken().data, params.index.getToken().line, params.index.getToken().column));
	params.output.addChild(params.index, wcParseNode(pn_return));
	params.index.nextToken();

	//expression
	if (!parseExpression(params))
		return 0;

	//consume semi colon
	if (!parseSColon(params))
		return 0;
	return 1;
}

int wc::parse::parseFuncCall(wcParseParams params)
{

	return 1;
}

int wc::parse::parseSColon(wcParseParams params)
{
	if (isEOS(params.index, params.error))
		return 0;

	//expect a semi colon
	wcToken token = params.index.getToken();
	if (token.type != tt_scolon)
		return setErrorReturn0(params.error, wcError(ec_par_unexpectedtoken, "Expected ';' (tt_scolon), got " + token.data,token.line,token.column));

	//simples
	params.index.nextToken();
	return 1;
}

//have we reached end of stream or not
bool wc::parse::isEOS(wcParseIndex& pIndex, wcError& pError)
{
	//check for eos
	if (!pIndex.isLexIndexValid())
		return !setErrorReturnFalse(pError, wcError(ec_par_eos, "Unexpected end of stream"));
	return false;
}
