#include "parse.h"
#include "lex.h"

using namespace std;
using namespace weec;
using namespace weec::lex;
using namespace weec::parse;

using enum weec::parse::wcParserErrorCode;
using enum weec::lex::wcTokenType;

#define ExpectAndNext(x) if (!ExpectToken(x))return Output.AddAsChild(wcParseOutput(wcParserError(UnexpectedToken, GetToken())));if (!NextToken())return Output.AddAsChild(wcParseOutput(wcParserError(UnexpectedEOF, GetToken())))


weec::parse::wcParser::wcParser(lex::wcTokenizer& _Tokenizer)
	: Tokenizer(_Tokenizer)
{
}

wcParseOutput weec::parse::wcParser::Parse()
{
	wcParseOutput Output;

	Tokenizer.NextToken();

	while (!Tokenizer.IsFinished() && !Tokenizer.IsErrored() && Output.Error.Code == None)
		Output.AddAsChild(wcStatementParser(Tokenizer, Output.SymbolTable, Scopes).Parse(true));

	Output.SymbolTable = SymbolTable;

	return Output;
}


wcParseOutput weec::parse::wcDeclarationParser::Parse()
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::Declaration), true);

	//variable/function type
	wcFullIdentifier DeclarationType; wcParseOutput TypeNode;
	if ((TypeNode = ParseType(DeclarationType, wcParseNodeType::Declaration_Type)).Error.Code != None)
		return Output.AddAsChild(TypeNode);

	///variable/function identifier
	wcFullIdentifier DeclarationIdent; wcToken IdentToken; wcParseOutput IdentNode;
	if ((IdentNode = ParseIdent(DeclarationIdent, IdentToken)).Error.Code != None)
		return Output.AddAsChild(TypeNode).AddAsChild(IdentNode);

	//semi colon - variable declaration
	if (ExpectToken(SemiColon))
	{
		//done, simple declaration no expression. add to symbol table
		AddSymbol(Output.SymbolTable, wcParseSymbol(wcParseSymbolType::Variable, DeclarationIdent, DeclarationType, IdentToken));

		return Output.AddAsChild(TypeNode).AddAsChild(IdentNode).AddAsChild(wcSemiColonParser(Tokenizer, SymbolTable, Scopes).Parse());
	}
	//function declaration
	else if (ExpectToken(OpenParenthesis))
	{
		wcParseOutput ParametersNode;
		vector<wcParseParameter> ParameterList;
		if ((ParametersNode = ParseParameters(DeclarationIdent, ParameterList)).Error.Code != None)
			return Output;

		//generate the correct scope, now including parameters
		wcFunctionIdentifier TrueFunctionName(DeclarationIdent, ParameterList);

		//hack - adjust ident node to have correct function name (including parameters)
		for (auto& It : IdentNode.AST)
			if (It.Type == wcParseNodeType::Declaration_Ident)
				It.Token.StringToken.Data = TrueFunctionName.to_string();
		//hack - adjust parameter identifiers to have correct scope, we didnt have the scope at the time without all arguments
		for (auto& It : ParametersNode.AST)
			if (It.Type == wcParseNodeType::Parameter_Ident)
				It.Token.StringToken.Data = wcFullIdentifier(TrueFunctionName.to_string(), wcIdentalyzer().GetIdentifierFromQualifiedIdentifier(It.Token.to_string())).to_string();
		for (auto& Param : ParameterList)
			Param.Identifier = wcFullIdentifier(TrueFunctionName.to_string(), Param.Identifier.to_unqualified_string());

		//register this function and its parameters
		auto FunctionSignature = wcParseFunctionSignature(DeclarationType, IdentToken, ParameterList, TrueFunctionName);
		AddSymbol(Output.SymbolTable, FunctionSignature);

		//optional semi colon
		if (ExpectToken(SemiColon))
		{
			Scopes.Pop();
			return Output.AddAsChild(TypeNode).AddAsChild(IdentNode)
				.AddAsChild(ParametersNode).AddAsChild(wcSemiColonParser(Tokenizer, Output.SymbolTable, Scopes).Parse());
		}

		//no semi colon, must be a full declaration with body
		wcParseOutput BodyNode = wcBlockParser(Tokenizer, Output.SymbolTable, Scopes).Parse(true);

		//pop here?  todo
		Scopes.Pop();

		return Output.AddAsChild(TypeNode).AddAsChild(IdentNode).AddAsChild(ParametersNode).AddAsChild(BodyNode);
	}
	else ExpectAndNext(AssignOperator);

	//add to symbol table, must be a variable
	AddSymbol(Output.SymbolTable, wcParseSymbol(wcParseSymbolType::Variable, DeclarationIdent, IdentToken));

	//optional assignment expression;
	wcParseOutput ExpNode;
	if ((ExpNode = wcExpressionParser(Tokenizer, SymbolTable, Scopes).ParseExpression()).Error.Code != None)
		return Output.AddAsChild(TypeNode).AddAsChild(IdentNode).AddAsChild(ExpNode);

	//semi colon
	return Output.AddAsChild(TypeNode).AddAsChild(IdentNode).AddAsChild(ExpNode).AddAsChild(wcSemiColonParser(Tokenizer, SymbolTable, Scopes).Parse());
}

wcParseOutput weec::parse::wcDeclarationParser::ParseType(wcFullIdentifier& DeclarationType, wcParseNodeType NodeType)
{
	//Declaration_Type
	auto TypeToken = GetToken();
	if (!TypeToken.IsBuiltinType() && TypeToken.Type != Identifier)
		return wcParseOutput(wcParserError(InvalidType, TypeToken));		//error - expected built in or user type

	//resolve type - built in types are registered in symbol table
	switch (Scopes.Resolve(wcIdentifier(TypeToken.to_string()), DeclarationType))
	{
	case wcParseScopeResolveResult::Ambiguous:
		return wcParseOutput(wcParserError(AmbiguousType, TypeToken));		//failed to resolve type
	case wcParseScopeResolveResult::Unresolved:
		return wcParseOutput(wcParserError(CouldntResolveType, TypeToken));		//failed to resolve type
	}

	if (!NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, TypeToken));

	return wcParseOutput(wcParseNode(NodeType, TypeToken));
}

wcParseOutput weec::parse::wcDeclarationParser::ParseIdent(wcFullIdentifier& DeclarationFullIdentifier, wcToken& IdentToken)
{
	//save the token for its metainfo
	IdentToken = GetToken();

	//parse Ident, discard the node it creates though
	wcIdentifier ParsedIdentifier;	wcParseOutput IdentNode;
	if ((IdentNode = wcIdentParser(Tokenizer, SymbolTable, Scopes).Parse(ParsedIdentifier, DeclarationFullIdentifier, true, true)).Error.Code != None)
		return IdentNode;

	//return a declaration type node with a fully qualified identifier added
	return wcParseOutput(wcParseNode(wcParseNodeType::Declaration_Ident, IdentToken, DeclarationFullIdentifier));
}


wcFullIdentifier GetFullIdentForFunction(wcFullIdentifier DeclarationIdent, vector<wcParseSymbol> ArgumentSymbols)
{
	string Output = DeclarationIdent.to_string() + "("; bool First = true;
	for (auto ArgumentSymbol : ArgumentSymbols)
	{
		Output += First ? ArgumentSymbol.DataType.to_string() : "," + ArgumentSymbol.DataType.to_string();
		First = false;
	}

	return wcFullIdentifier(Output + ")");
}

wcParseOutput weec::parse::wcDeclarationParser::ParseParameters(wcFullIdentifier DeclarationIdent, vector<wcParseParameter>& ParametersOut)
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::Parameters), true);

	//consume openining parenthesis, error if we dont find it
	ExpectAndNext(OpenParenthesis);

	//check for any arguments
	while (!Tokenizer.IsFinished() && !Tokenizer.IsErrored() && GetToken().Type != CloseParenthesis)
	{
		//parse a parameter
		wcParseParameter Parameter;
		if ((Output.AddAsChild(ParseParameter(DeclarationIdent, Parameter))).Error.Code != None)
			return Output;
		ParametersOut.push_back(Parameter);

		//move on to closing parenthesis if we dont encounter a comma
		if (!ExpectToken(Comma))
			break;
		if (!NextToken())
			return wcParseOutput(wcParserError(UnexpectedEOF, GetToken()));
	}

	//consume closing parenthesis
	ExpectAndNext(CloseParenthesis);

	//success
	return Output;
}

wcParseOutput weec::parse::wcDeclarationParser::ParseParameter(wcFullIdentifier DeclarationIdent, wcParseParameter& ParameterOutput)
{
	wcParseOutput ParameterNode(wcParseNode(wcParseNodeType::Parameter), true);

	//Parameter type
	ParameterOutput.TypeToken = GetToken();
	if ((ParameterNode.AddAsChild(ParseType(ParameterOutput.TypeIdentifier, wcParseNodeType::Parameter_Type))).Error.Code != None)
		return ParameterNode;

	//Parameter identifier
	auto ParameterIdentifier = ParameterOutput.IdentifierToken = GetToken();
	//auto ParameterScope = ?;	//cant create the scope without all parameters...
	ParameterOutput.Identifier = wcFullIdentifier(wcIdentifierScope(""), ParameterIdentifier.to_string());

	if ((ParameterNode.AddAsChild(wcParseNode(wcParseNodeType::Parameter_Ident, ParameterOutput.IdentifierToken, ParameterOutput.Identifier))).Error.Code != None)
		return ParameterNode;

	//eof not allowed
	if (!NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, GetToken()));

	return ParameterNode;
}

weec::parse::wcIdentParser::wcIdentParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable, wcParseScopes& _Scopes)
	: wcSubParser(_Tokenizer, _SymbolTable, _Scopes)
{

}

wcParseOutput weec::parse::wcIdentParser::Parse(wcIdentifier& ParsedIdentifier, wcFullIdentifier& ResolvedFullIdentifier, bool Consume, bool IsDeclaration)
{
	//obviously we expect an identifier here
	if (!ExpectToken(wcTokenType::Identifier))
		return wcParseOutput(wcParserError(UnexpectedToken, GetToken()));

	//save the token for its meta info
	auto IdentToken = GetToken();

	//output the identifier as it was seen in source
	ParsedIdentifier = wcIdentifier(IdentToken.to_string());

	//try to resovlve it, output the fully qualified identifier to ResolvedFullIdentifier. 
	//raise an error if it needs to be previously declared (ie ExpectDeclared)
	switch (Scopes.Resolve(ParsedIdentifier, ResolvedFullIdentifier, true))
	{
	case wcParseScopeResolveResult::Ambiguous:
		if (!IsDeclaration)
			return wcParseOutput(wcParserError(AmbiguousIdentifier, IdentToken));		//failed to resolve a single identifier
		break;

	case wcParseScopeResolveResult::Unresolved:
		if (!IsDeclaration)
			return wcParseOutput(wcParserError(CouldntResolveIdentifier, IdentToken));		//failed to resolve 
		break;

	case wcParseScopeResolveResult::Resolved:
		if (IsDeclaration)
			return wcParseOutput(wcParserError(IdentRedeclaration, IdentToken));		//failed to resolve 
		break;
	}

	if(ResolvedFullIdentifier == wcFullIdentifier())
		ResolvedFullIdentifier = wcFullIdentifier(Scopes.Top().Name.to_string(), ParsedIdentifier);

	//consume the identifier token if told so (ie when expression parsing)
	if (Consume && !NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, IdentToken));	//eof never allowed after an ident i dont think

	//success
	return wcParseOutput(wcParseNode(wcParseNodeType::Identifier, IdentToken));
}

wcParseOutput weec::parse::wcStatementParser::Parse(bool AllowDeclarations)
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::Statement), true);

	auto ThisToken = GetToken();
	wcIdentifier Identifier;	wcFullIdentifier ResolvedIdentifier;	wcParseOutput IdentNode;
	switch (ThisToken.Type)
	{
	WC_SWITCHCASE_TOKENS_LITERAL
	WC_SWITCHCASE_TOKENS_OPERATORS_ALL
	case OpenParenthesis:
		if ((Output.AddAsChild(wcExpressionParser(Tokenizer, SymbolTable, Scopes).ParseExpression())).Error.Code != None)
			return Output;	//error occurred
		return Output.AddAsChild(wcSemiColonParser(Tokenizer, SymbolTable, Scopes).Parse());

		WC_SWITCHCASE_TOKENS_BUILTIN_TYPES
			if (!AllowDeclarations)
				return wcParseOutput(wcParserError(wcParserErrorCode::DeclarationsProhibited, ThisToken));
		return Output.AddAsChild(wcDeclarationParser(Tokenizer, SymbolTable, Scopes).Parse());

	case wcTokenType::SemiColon:
		return wcSemiColonParser(Tokenizer, SymbolTable, Scopes).Parse();

	case wcTokenType::IfKeyword:
		return Output.AddAsChild(wcIfParser(Tokenizer, SymbolTable, Scopes).Parse());

	case wcTokenType::ReturnKeyword:
		return Output.AddAsChild(wcReturnParser(Tokenizer, SymbolTable, Scopes).Parse());

	case wcTokenType::PrintKeyword:
		return Output.AddAsChild(wcPrintParser(Tokenizer, SymbolTable, Scopes).Parse());

	case wcTokenType::WhileKeyword:
		return Output.AddAsChild(wcWhileParser(Tokenizer, SymbolTable, Scopes).Parse());

	case wcTokenType::Identifier:
		if ((IdentNode = wcIdentParser(Tokenizer, SymbolTable, Scopes).Parse(Identifier, ResolvedIdentifier, false, false)).Error.Code != None)
			return Output.AddAsChild(IdentNode);	//error occurred
		switch (SymbolTable.Get(ResolvedIdentifier).Type)
		{
		case wcParseSymbolType::Type:
			if (!AllowDeclarations)
				return wcParseOutput(wcParserError(wcParserErrorCode::DeclarationsProhibited, ThisToken));
			return wcDeclarationParser(Tokenizer, SymbolTable, Scopes).Parse();

		case wcParseSymbolType::Variable:
		case wcParseSymbolType::Function:
		case wcParseSymbolType::FunctionAlias:
			if ((Output.AddAsChild(wcExpressionParser(Tokenizer, SymbolTable, Scopes).ParseExpression(SymbolTable.Get(ResolvedIdentifier)))).Error.Code != None)
				return Output;
			return Output.AddAsChild(wcSemiColonParser(Tokenizer, SymbolTable, Scopes).Parse());

		case wcParseSymbolType::Namespace:
		case wcParseSymbolType::Null:
		default:
			return wcParseOutput(wcParserError(wcParserErrorCode::UnexpectedToken, ThisToken));	//error
		}
	default:
		return wcParseOutput(wcParserError(wcParserErrorCode::UnexpectedToken, ThisToken));
	}
}

wcParseOutput weec::parse::wcBlockParser::Parse(bool AllowDeclarations)
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::Block), true);

	ExpectAndNext(OpenBrace);

	while (GetToken().Type != CloseBrace)
	{
		Output.AddAsChild(wcStatementParser(Tokenizer, SymbolTable, Scopes).Parse(AllowDeclarations));

		if (Tokenizer.IsErrored() || Output.Error.Code != None)
			return Output;
	}

	if (!ExpectToken(CloseBrace))
		return wcParseOutput(wcParserError(MissingClosingBrace, GetToken()));
	NextToken();

	return Output;
}

wcParseOutput weec::parse::wcIfParser::Parse()
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::IfStatement), true);

	//if keyword
	ExpectAndNext(IfKeyword);

	// ( 
	ExpectAndNext(OpenParenthesis);

	//expression
	if (Output.AddAsChild(wcExpressionParser(Tokenizer, SymbolTable, Scopes).ParseExpression()).Error.Code != None)
		return Output;

	// )
	if (!ExpectToken(CloseParenthesis))
		return wcParseOutput(wcParserError(If_MissingClosingParenthesis, GetToken()));
	if (!NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, GetToken()));

	//block/statement
	Output.AddAsChild(wcParseNode(wcParseNodeType::If_TrueBlock), true);
	if (ExpectToken(OpenBrace))
		Output.AddAsChild(wcBlockParser(Tokenizer, SymbolTable, Scopes).Parse(false));
	else
		Output.AddAsChild(wcStatementParser(Tokenizer, SymbolTable, Scopes).Parse(false));
	Output.Up();	//return pointer to parent of true block
	if (Output.Error.Code != None)
		return Output;

	//optional else keyword
	if (!ExpectToken(ElseKeyword))
		return Output;
	if (!NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, GetToken()));

	//optional else block/statement
	Output.AddAsChild(wcParseNode(wcParseNodeType::If_ElseBlock), true);
	GetToken().Type == OpenBrace 
		? Output.AddAsChild(wcBlockParser(Tokenizer, SymbolTable, Scopes).Parse(false))
		: Output.AddAsChild(wcStatementParser(Tokenizer, SymbolTable, Scopes).Parse(false));
	Output.Up();	//return pointer to parent of else block

	return Output;
}

//Note: always returns an empty parseoutput, we only need to know if it errored or not, so always 
//append it to parent output to see the error cascade back up the call chain
wcParseOutput weec::parse::wcSemiColonParser::Parse()
{
	if (GetToken().Type != SemiColon)
		return Tokenizer.IsFinished()
			? wcParseOutput(wcParserError(UnexpectedEOF, GetToken()))		
			: wcParseOutput(wcParserError(UnexpectedToken, GetToken()));		

	NextToken();

	return wcParseOutput(SymbolTable);	//no nodes created
}

wcParseOutput weec::parse::wcReturnParser::Parse()
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::ReturnStatement), true);

	//return keyword;
	ExpectAndNext(ReturnKeyword);

	//semi colon/optional return expression
	if (ExpectToken(SemiColon))
		return Output.AddAsChild(wcSemiColonParser(Tokenizer, SymbolTable, Scopes).Parse());

	if (Output.AddAsChild(wcExpressionParser(Tokenizer, SymbolTable, Scopes).ParseExpression()).Error.Code != None)
		return Output;

	return Output.AddAsChild(wcSemiColonParser(Tokenizer, SymbolTable, Scopes).Parse());
}

wcParseOutput weec::parse::wcWhileParser::Parse()
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::WhileStatement), true);

	//while keyword
	if (!ExpectToken(WhileKeyword))
		return wcParseOutput(wcParserError(UnexpectedToken, GetToken()));		//error - expected built in or user type

	// ( 
	ExpectAndNext(OpenParenthesis);

	//expression
	if (Output.AddAsChild(wcExpressionParser(Tokenizer, SymbolTable, Scopes).ParseExpression()).Error.Code != None)
		return Output;

	// )
	if (!ExpectToken(CloseParenthesis))
		return wcParseOutput(wcParserError(While_MissingClosingParenthesis, GetToken()));
	if (!NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, GetToken()));

	//block/statement
	if (ExpectToken(OpenBrace))
		return Output.AddAsChild(wcBlockParser(Tokenizer, SymbolTable, Scopes).Parse(false));
	else
		return Output.AddAsChild(wcStatementParser(Tokenizer, SymbolTable, Scopes).Parse(false));
}


wcParseOutput weec::parse::wcPrintParser::Parse()
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::PrintStatement), true);

	//print keyword;
	ExpectAndNext(PrintKeyword);

	//expression
	if (Output.AddAsChild(wcExpressionParser(Tokenizer, SymbolTable, Scopes).ParseExpression()).Error.Code != None)
		return Output;

	//semi colon
	return Output.AddAsChild(wcSemiColonParser(Tokenizer, SymbolTable, Scopes).Parse());
}

wcParseOutput weec::parse::wcStructParser::Parse()
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::StructDeclaration), true);

	ExpectAndNext(StructKeyword);
	ExpectAndNext(OpenBrace);

	Output.AddAsChild(wcBlockParser(Tokenizer, SymbolTable, Scopes).Parse(true));

	ExpectAndNext(CloseBrace);

	return wcParseOutput();
}

tree<wcParseNode>::pre_order_iterator weec::parse::wcParseExpression::GetExpressionRootNodeBegin()
{
	auto it = AST.begin();
	while (it != AST.end())
	{
		if (it->Type == wcParseNodeType::Expression)
			return it;
		it++;
	}
	return AST.end();
}

tree<wcParseNode>::pre_order_iterator weec::parse::wcParseExpression::GetExpressionRootNodeEnd()
{
	auto it = GetExpressionNodeBegin();
	unsigned int StartingDepth = AST.depth(it);
	while (AST.depth(it) > StartingDepth)
		it++;
	return it;
}

tree<wcParseNode>::pre_order_iterator weec::parse::wcParseExpression::GetExpressionNodeEnd()
{
	return GetExpressionRootNodeEnd()--;
}

wcParseExpression weec::parse::wcParseExpression::operator=(wcParseExpression Other)
{
	AST = Other.AST;
	Error = Other.Error;

	for (auto Token : Other.Tokens)
		Tokens.push_back(Token);

	Type = Other.Type;
	DataType = Other.DataType;
	return *this;
}


tree<wcParseNode>::pre_order_iterator weec::parse::wcParseExpression::GetExpressionNodeBegin()
{
	auto t = GetExpressionRootNodeBegin();
	t++;
	return t;
}

weec::parse::wcParseExpression::wcParseExpression()
{
	auto ExpNode = AST.insert(AST.begin(), *new wcParseNode(wcParseNodeType::Expression));
	Type = wcParseExpressionType::Grouping;
	DataType = wcFullIdentifier("null");
}

weec::parse::wcParseExpression::wcParseExpression(wcParserError _Error)
{
	auto ExpNode = AST.insert(AST.begin(), *new wcParseNode(wcParseNodeType::Expression));
	Error = _Error;
	Type = wcParseExpressionType::Grouping;
	DataType = wcFullIdentifier("null");
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, lex::wcToken OperatorOrLiteral)
{
	auto ExpNode = AST.insert(AST.begin(), *new wcParseNode(wcParseNodeType::Expression));

	AST.append_child(ExpNode, *new wcParseNode(HeadType, OperatorOrLiteral));

	Tokens.push_back(OperatorOrLiteral);

	Type = OperatorOrLiteral.Type == Identifier ? wcParseExpressionType::Variable : wcParseExpressionType::Literal;

	if (Type == wcParseExpressionType::Literal)
		switch (OperatorOrLiteral.Type)
		{
		case IntLiteral:
			DataType = wcFullIdentifier(ParserConsts.GlobalIdentifier + ParserConsts.ScopeDelimiter + "int");
			break;
		case StringLiteral:
			DataType = wcFullIdentifier(ParserConsts.GlobalIdentifier + ParserConsts.ScopeDelimiter + "string");
			break;
		case FloatLiteral:
			DataType = wcFullIdentifier(ParserConsts.GlobalIdentifier + ParserConsts.ScopeDelimiter + "float");
			break;
		case CharLiteral:
			DataType = wcFullIdentifier(ParserConsts.GlobalIdentifier + ParserConsts.ScopeDelimiter + "char");
			break;
		default:
			DataType = wcFullIdentifier("null");
			break;
		}
	else
		DataType = wcFullIdentifier("null");
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, lex::wcToken Variable, wcFullIdentifier _DataType)
{
	auto ExpNode = AST.insert(AST.begin(), *new wcParseNode(wcParseNodeType::Expression));

	AST.append_child(ExpNode, *new wcParseNode(HeadType, Variable));

	Tokens.push_back(Variable);
	Type = Variable.Type == Identifier ? wcParseExpressionType::Variable : wcParseExpressionType::Literal;

	DataType = _DataType;
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, wcFullIdentifier FullIdent, wcFullIdentifier _DataType, lex::wcToken OperatorOrLiteral)
{
	auto ExpNode = AST.insert(AST.begin(), *new wcParseNode(wcParseNodeType::Expression));

	OperatorOrLiteral.StringToken.Data = FullIdent.to_string();
	AST.append_child(ExpNode, *new wcParseNode(HeadType, OperatorOrLiteral));
	Tokens.push_back(OperatorOrLiteral);

	Type = wcParseExpressionType::Variable;
	DataType = _DataType;
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, wcParseExpression LeftHand, wcToken Operator, wcParseExpression RightHand)
{
	//build the ast
	auto ExpRootNode = AST.insert(AST.begin(), *new wcParseNode(wcParseNodeType::Expression));

	auto OpNode = AST.append_child(ExpRootNode, wcParseNode(HeadType, Operator));
	auto OpNodeChild = AST.append_child(OpNode);

	AST.insert_subtree(OpNodeChild, LeftHand.GetExpressionNodeBegin());
	AST.insert_subtree(OpNodeChild, RightHand.GetExpressionNodeBegin());
	AST.erase(OpNodeChild);

	//tokens in order
	for (auto t = LeftHand.Tokens.begin(); t != LeftHand.Tokens.end(); ++t)
		Tokens.push_back(*t);
	Tokens.push_back(Operator);
	for (auto t = RightHand.Tokens.begin(); t != RightHand.Tokens.end(); ++t)
		Tokens.push_back(*t);

	Error = LeftHand.Error;
	if (LeftHand.Error.Code == None && RightHand.Error.Code != None)
		Error = RightHand.Error;

	if (Operator.Type == AssignOperator)
		Type = wcParseExpressionType::Assignment;
	else if (Operator.Type == LogAndOperator || Operator.Type == LogOrOperator)
		Type = wcParseExpressionType::Logical;
	else
		Type = wcParseExpressionType::Binary;

	DataType = LeftHand.DataType;
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, wcParseExpression LeftHand, lex::wcToken Operator, wcParseExpression Middle, wcParseExpression RightHand)
{
	//build the ast
	auto ExpRootNode = AST.insert(AST.begin(), *new wcParseNode(wcParseNodeType::Expression));

	auto OpNode = AST.append_child(ExpRootNode, wcParseNode(HeadType, Operator));
	auto OpNodeChild = AST.append_child(OpNode);

	AST.insert_subtree(OpNodeChild, LeftHand.GetExpressionNodeBegin());
	AST.insert_subtree(OpNodeChild, Middle.GetExpressionNodeBegin());
	AST.insert_subtree(OpNodeChild, RightHand.GetExpressionNodeBegin());
	AST.erase(OpNodeChild);

	//tokens in order
	for (auto t : LeftHand.Tokens)
		Tokens.push_back(t);
	Tokens.push_back(Operator);
	for (auto t : Middle.Tokens)
		Tokens.push_back(t);
	for (auto t : RightHand.Tokens)
		Tokens.push_back(t);

	Error = LeftHand.Error;
	if (LeftHand.Error.Code == None && Middle.Error.Code != None && RightHand.Error.Code != None)
		Error = RightHand.Error;
	
	Type = wcParseExpressionType::Ternary;

	DataType = LeftHand.DataType;
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, lex::wcToken Operator, wcParseExpression RightHand)
{
	auto ExpRootNode = AST.insert(AST.begin(), *new wcParseNode(wcParseNodeType::Expression));

	auto OpNode = AST.append_child(ExpRootNode, wcParseNode(HeadType, Operator));
	auto OpNodeChild = AST.append_child(OpNode);

	AST.insert_subtree(OpNodeChild, RightHand.GetExpressionNodeBegin());
	AST.erase(OpNodeChild);

	Tokens.push_back(Operator);

	for (auto t = RightHand.Tokens.begin(); t != RightHand.Tokens.end(); ++t)
		Tokens.push_back(*t);

	Error = RightHand.Error;
	Type = wcParseExpressionType::Unary;
	DataType = RightHand.DataType;
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, wcFullIdentifier CalleeFullIdent, wcParseExpression Callee, std::vector<wcParseExpression> Arguments)
{
	auto ExpNode = AST.insert(AST.begin(), *new wcParseNode(wcParseNodeType::Expression));
	auto ExpNodeChild = AST.append_child(ExpNode);

	auto CalleeNode = AST.insert_subtree(ExpNodeChild, Callee.GetExpressionNodeBegin());
	CalleeNode.node->data.Type = wcParseNodeType::Expression_Call;
	(CalleeNode.node)->data.Token.StringToken.Data = CalleeFullIdent.to_string();	//adjust to include params
	Callee.Tokens[0].StringToken.Data = CalleeFullIdent.to_string();	//adjust to include params

	AST.erase(ExpNodeChild);
	auto CalleeNodeChild = AST.append_child(CalleeNode);
	Tokens.push_back(Callee.Tokens[0]);

	for (auto Arg : Arguments)
	{
		//add argument ASTs as a sub child to the callee
		AST.insert_subtree(CalleeNodeChild, Arg.GetExpressionRootNodeBegin());

		//add an error if it had one
		if (Arg.Error.Code != None)
			Error = Arg.Error;

		//add all tokens from this argument
		for (auto Tok : Arg.Tokens)
			Tokens.push_back(Tok);
	}

	AST.erase(CalleeNodeChild);

	Type = wcParseExpressionType::Call;
	DataType = Callee.DataType;
}


wcParseOutput weec::parse::wcExpressionParser::ParseExpression()
{
	if (Tokenizer.IsFinished())
		return wcParserError(wcParserErrorCode::Expression_Empty, GetToken());

	auto eExpression = ParseExpression_Expression();
	wcParseOutput Output;
	Output.AST = eExpression.AST;
	Output.Error = eExpression.Error;
	return Output;
}

wcParseOutput weec::parse::wcExpressionParser::ParseExpression(wcParseSymbol Symbol)
{
	if (Tokenizer.IsFinished())
		return wcParserError(wcParserErrorCode::Expression_Empty, GetToken());

	wcParseOutput Output;
	auto Expression = ParseExpression_Expression();
	if (Expression.Error.Code != wcParserErrorCode::None)
		return wcParseOutput(Expression.Error);

	if (!ExpectToken(SemiColon))
	{
		Output.Error = wcParserError(wcParserErrorCode::Expression_UnexpectedToken, GetToken());
		return Output;
	}

	Output.AST = Expression.AST;
	Output.Error = Expression.Error;
	return Output;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Expression()
{
	return ParseExpression_SubExpression();
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_SubExpression()
{
	return  ParseExpression_Assignment();
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Assignment()
{
	wcParseExpression LeftHandSide = ParseExpression_Ternary();
	if (LeftHandSide.Error.Code != None)
		return LeftHandSide;

	auto Operator = GetToken();
	while (Operator.Type == AssignOperator)
	{
		NextToken();

		if (LeftHandSide.Type != wcParseExpressionType::Variable)
		{
			LeftHandSide.Error = wcParserError(wcParserErrorCode::Expression_NotAnLValue, GetToken());
			return LeftHandSide;
		}

		return wcParseExpression(wcParseNodeType::Expression_Assignment, LeftHandSide, Operator, ParseExpression_Assignment());
	}

	return LeftHandSide;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Ternary()
{
	wcParseExpression LeftHandSide = ParseExpression_LogicOr();
	if (LeftHandSide.Error.Code != None)
		return LeftHandSide;

	auto LeftOperator = GetToken();
	wcParseExpression Middle, Right;
	while (LeftOperator.Type == TernaryOperator)
	{
		NextToken();
		
		if ((Middle = ParseExpression_Expression()).Error.Code != None)
			return Middle;

		if (!ExpectToken(Colon))
			return wcParseExpression(wcParserError(UnexpectedToken, GetToken()));
		if (!NextToken())
			return wcParseExpression(wcParserError(UnexpectedEOF, GetToken()));

		if ((Right = ParseExpression_Expression()).Error.Code != None)
			return Right;

		LeftHandSide = wcParseExpression(wcParseNodeType::Expression_Ternary, LeftHandSide, LeftOperator, Middle, Right);

		LeftOperator = GetToken();
	}

	return LeftHandSide;
}


wcParseExpression weec::parse::wcExpressionParser::ParseExpression_LogicOr()
{
	wcParseExpression LeftHandSide = ParseExpression_LogicAnd();
	if (LeftHandSide.Error.Code != None)
		return LeftHandSide;

	auto Operator = GetToken();
	while (Operator.Type == LogOrOperator)
	{
		NextToken();

		LeftHandSide = wcParseExpression(wcParseNodeType::Expression_LogicOr, LeftHandSide, Operator, ParseExpression_LogicAnd());

		Operator = GetToken();
	}

	return LeftHandSide;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_LogicAnd()
{
	wcParseExpression LeftHandSide = ParseExpression_Equality();
	if (LeftHandSide.Error.Code != None)
		return LeftHandSide;

	auto Operator = GetToken();
	while (Operator.Type == LogAndOperator)
	{
		NextToken();

		LeftHandSide = wcParseExpression(wcParseNodeType::Expression_LogicAnd, LeftHandSide, Operator, ParseExpression_Equality());

		Operator = GetToken();
	}

	return LeftHandSide;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Equality()
{
	wcParseExpression LeftHandSide = ParseExpression_Comparison();
	if (LeftHandSide.Error.Code != None)
		return LeftHandSide;

	auto Operator = GetToken();
	while (Operator.Type == NotEqualOperator || Operator.Type == EqualOperator)
	{
		NextToken();

		auto RightExp = ParseExpression_Comparison();
		LeftHandSide = wcParseExpression(wcParseNodeType::Expression_Equality, LeftHandSide, Operator, RightExp);

		Operator = GetToken();
	}

	return LeftHandSide;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Comparison()
{
	wcParseExpression LeftHandSide = ParseExpression_Term();
	if (LeftHandSide.Error.Code != None)
		return LeftHandSide;

	auto Operator = GetToken();
	while (Operator.Type == GreaterEqualOperator || Operator.Type == GreaterOperator ||
		Operator.Type == LessOperator || Operator.Type == LessEqualOperator)
	{
		NextToken();

		LeftHandSide = wcParseExpression(wcParseNodeType::Expression_Comparison, LeftHandSide, Operator, ParseExpression_Term());

		Operator = GetToken();
	}

	return LeftHandSide;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Term()
{
	wcParseExpression LeftHandSide = ParseExpression_Factor();
	if (LeftHandSide.Error.Code != None)
		return LeftHandSide;

	auto Operator = GetToken();
	while (Operator.Type == MinusOperator || Operator.Type == PlusOperator)
	{
		if (!NextToken())
		{
			LeftHandSide.Error = wcParserError(wcParserErrorCode::Expression_UnexpectedEOF, GetToken());
			return LeftHandSide;
		}

		LeftHandSide = wcParseExpression(wcParseNodeType::Expression_Term, LeftHandSide, Operator, ParseExpression_Factor());

		Operator = GetToken();
	}

	return LeftHandSide;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Factor()
{
	wcParseExpression LeftHandSide = ParseExpression_Unary();
	if (LeftHandSide.Error.Code != None)
		return LeftHandSide;

	auto Operator = GetToken();
	while (Operator.Type == DivideOperator || Operator.Type == MultiplyOperator)
	{
		NextToken();

		LeftHandSide = wcParseExpression(wcParseNodeType::Expression_Factor, LeftHandSide, Operator, ParseExpression_Unary());

		Operator = GetToken();
	}

	return LeftHandSide;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Unary()
{
	//wcParseExpression Output = ParseExpression_Primary();

	auto Operator = GetToken();
	if (Operator.Type == MinusOperator || Operator.Type == PlusOperator || Operator.Type == LogNotOperator)
	{
		NextToken();

		return wcParseExpression(wcParseNodeType::Expression_Unary, Operator, ParseExpression_Unary());
	}

	return ParseExpression_Call();
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Call()
{
	wcParseExpression Primary = ParseExpression_Primary();
	if (Primary.Error.Code != None)
		return Primary;

	wcParseExpression Arguments;
	if (GetToken().Type == OpenParenthesis)
	{
		if (!NextToken())
			return wcParseExpression(wcParserError(wcParserErrorCode::UnexpectedEOF, GetToken()));

		Arguments = ParseExpression_CallArguments(Primary);
	}
	else
		return Primary;

	//adjust the
	return Arguments;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_CallArguments(wcParseExpression Callee)
{
	vector<wcParseExpression> Arguments;

	wcParseExpression Buff;
	while (!Tokenizer.IsFinished() && GetToken().Type != CloseParenthesis)
	{
		if ((Buff = ParseExpression_Expression()).Error.Code != None)
			return Buff;

		Arguments.push_back(Buff);
		if (GetToken().Type != Comma)
			break;
		if (!NextToken())
			return wcParseExpression(wcParserError(wcParserErrorCode::UnexpectedEOF, GetToken()));
	}

	if (GetToken().Type != CloseParenthesis)
		return wcParseExpression(wcParserError(wcParserErrorCode::Expression_FunctionCallMissingOpenParenthesis, GetToken()));
	NextToken();

	//todo check symbol type

	//todo check arity

	//check we dont exceed max arguments
	if (Arguments.size() > 255)
		return wcParseExpression(wcParserError(wcParserErrorCode::FunctionCall_MaxArgumentsExceeded, GetToken()));

	//todo get the functions full qualified name (with arugments), add as second param
	return wcParseExpression(wcParseNodeType::Expression_Call, wcFullIdentifier(Callee.Tokens[0].to_string(), Arguments), Callee, Arguments);
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Primary()
{
	wcParseExpression Output;

	auto Operand = GetToken();
	wcIdentifier IdentAsSeen;
	wcFullIdentifier ResolvedIdenfitifer;
	wcParseOutput IdentNode;
	switch (Operand.Type)
	{
	case TrueKeyword:
	case FalseKeyword:
	case StringLiteral:
	case FloatLiteral:
	case CharLiteral:
	case IntLiteral:
		NextToken();
		return wcParseExpression(wcParseNodeType::Expression_Primary, Operand);

	case Identifier:
		if ((IdentNode = wcIdentParser(Tokenizer, SymbolTable, Scopes).Parse(IdentAsSeen, ResolvedIdenfitifer, true, false)).Error.Code != None)
			return wcParserError(wcParserErrorCode::UndeclaredIdent, GetToken());
		return wcParseExpression(wcParseNodeType::Expression_Primary, ResolvedIdenfitifer, SymbolTable.Get(ResolvedIdenfitifer).DataType, Operand);

	case OpenParenthesis:
		//consume opening parenthesis
		if (Tokenizer.IsFinished() || !NextToken())
			return wcParserError(wcParserErrorCode::UnexpectedEOF, GetToken());			//end of file before last paren

		if ((Output = ParseExpression_SubExpression()).Error.Code != None)
			return Output;

		//consume closing parenthesis
		if (Tokenizer.IsFinished() || GetToken().Type != CloseParenthesis)
			return wcParserError(wcParserErrorCode::Expression_MissingClosingParenthesis, GetToken());			//didnt find one, madness
		NextToken();
		return Output;

	default:
		//error
		Output.Error = wcParserError(wcParserErrorCode::Expression_UnexpectedToken, GetToken());
		return Output;
	}

	//error
	return Output;
}

weec::parse::wcParseOutput::wcParseOutput()
{
	NodeIndex = UnofficialHead = AST.insert(AST.begin(), wcParseNode(wcParseNodeType::Head));
}

weec::parse::wcParseOutput::wcParseOutput(wcParserError _Error)
{
	NodeIndex = UnofficialHead = AST.insert(AST.begin(), wcParseNode(wcParseNodeType::Head));
	Error = _Error;
}

weec::parse::wcParseOutput::wcParseOutput(wcParseNode _Node, bool PointToChild)
{
	NodeIndex = UnofficialHead = AST.insert(AST.begin(), wcParseNode(wcParseNodeType::Head));
	AddAsChild(_Node, PointToChild);
}

wcParseOutput& weec::parse::wcParseOutput::AddAsChild(wcParseOutput Child, bool PointToChild)
{
	if (Child.AST.size() > 1)
		(PointToChild)
			?	NodeIndex = AST.append_child(NodeIndex, Child.GetSubHead())
			:	AST.append_child(NodeIndex, Child.GetSubHead());

	Error = Child.Error;
	SymbolTable.Merge(Child.SymbolTable);

	return *this;
}

wcParseOutput& weec::parse::wcParseOutput::AddAsChild(wcParseNode Node, bool PointToChild)
{
	(PointToChild)
		? NodeIndex = AST.append_child(NodeIndex, Node)
		: AST.append_child(NodeIndex, Node);

	return *this;
}

tree<wcParseNode>::pre_order_iterator weec::parse::wcParseOutput::GetSubHead()
{
	auto HeadPtr = AST.begin(), t = AST.begin();

	//search for a head pointer
	while (t != AST.end())
	{
		if (t->Type == wcParseNodeType::Head)
		{
			HeadPtr = t;
			break;
		}
		++t;
	}

	if (HeadPtr->Type != wcParseNodeType::Head)
		return HeadPtr;

	t = HeadPtr;
	while (++t != AST.end())
		return t;

	return HeadPtr; //no children/only siblings
}


//append as child
wcParseOutput weec::parse::wcParseOutput::operator+(wcParseOutput other)
{
	AST.insert_subtree(AST.begin(), other.AST.begin());
	Error = other.Error;
	SymbolTable.Merge(other.SymbolTable);

	return *this;
}

wcParseOutput weec::parse::wcParseOutput::operator=(wcParseOutput other)
{
	AST = other.AST;
	Error = other.Error;
	SymbolTable.Merge(other.SymbolTable);

	return *this;
}

wcParseOutput weec::parse::wcParseOutput::operator+=(wcParseOutput other)
{
	*this = *this + other;

	return *this;
}

weec::parse::wcParseNode::wcParseNode()
{
	Type = wcParseNodeType::Empty;
	Token = wcToken();
}

weec::parse::wcParseNode::wcParseNode(const wcParseNode& Other)
{
	Token = Other.Token;
	Type = Other.Type;
}

weec::parse::wcParseNode::wcParseNode(wcParseNodeType _Type)
{
	Type = _Type;
}

weec::parse::wcParseNode::wcParseNode(wcParseNodeType _Type, wcToken _Token)
{
	Type = _Type;
	Token = _Token;
}

weec::parse::wcParseNode::wcParseNode(wcParseNodeType _Type, wcToken _Token, wcFullIdentifier FullIdentifer)
{
	Type = _Type;
	Token = _Token;
	Token.StringToken.Data = FullIdentifer.to_string();
	Memo = FullIdentifer.to_string();
}

weec::parse::wcParseNode::wcParseNode(lex::wcToken _Token)
{
	Type = wcParseNodeType::Expression_Operator;	//assumption
	Token = _Token;
}

bool weec::parse::wcParseNode::operator==(const wcParseNode& Other)
{
	if (Other.Token == Token && Other.Type == Type)
		return true;
	return false;
}

bool weec::parse::wcParseNode::operator==(wcParseNode Other)
{
	if (Other.Token == Token && Other.Type == Type)
		return true;
	return false;
}

wcParseNode& weec::parse::wcParseNode::operator=(const wcParseNode& Other)
{
	Token = Other.Token;
	Type = Other.Type;
	return *this;
}

std::string weec::parse::to_string(wcParserErrorCode Code)
{
	switch (Code)
	{
	case None:									return "Head";
	case FuckKnows:								return "FuckKnows";
	case UnexpectedToken:						return "UnexpectedToken";
	case If_MissingClosingParenthesis:			return "If_MissingClosingParenthesis";
	case While_MissingClosingParenthesis:		return "While_MissingClosingParenthesis";
	case MissingClosingBrace:					return "MissingClosingBrace";
	case InvalidType:							return "InvalidType";
	case IdentRedeclaration:					return "IdentRedeclaration";
	case UndeclaredIdent:						return "UndeclaredIdent";
	case UnexpectedEOF:							return "UnexpectedEOF";
	case Expression_Empty:						return "Expression_Empty";
	case Expression_UnexpectedToken:			return "Expression_UnexpectedToken";
	case Expression_MissingClosingParenthesis:	return "Expression_MissingClosingParenthesis";
	case Expression_UnexpectedEOF:				return "Expression_UnexpectedEOF";
	default:
		return "";
	}
}


std::string weec::parse::to_string(wcParseNodeType Type)
{
	switch (Type)
	{
	case wcParseNodeType::Head:					return "Head";
	case wcParseNodeType::Empty:				return "Empty";
	case wcParseNodeType::WhileStatement:		return "WhileStatement";
	case wcParseNodeType::ReturnStatement:		return "ReturnStatement";
	case wcParseNodeType::PrintStatement:		return "PrintStatement";
	case wcParseNodeType::Return_Expression:	return "Return_Expression";
	case wcParseNodeType::IfStatement:			return "IfStatement";
	case wcParseNodeType::If_Expression:		return "If_Expression";
	case wcParseNodeType::If_TrueBlock:			return "If_TrueBlock";
	case wcParseNodeType::If_ElseBlock:			return "If_ElseBlock";
	case wcParseNodeType::StructDeclaration:	return "StructDeclaration";
	case wcParseNodeType::Declaration:			return "Declaration";
	case wcParseNodeType::Declaration_Ident:	return "Declaration_Ident";
	case wcParseNodeType::Declaration_Type:		return "Declaration_Type";
	case wcParseNodeType::Parameters:			return "Parameters";
	case wcParseNodeType::Parameter:			return "Parameter";
	case wcParseNodeType::Parameter_Type:		return "Parameter_Type";
	case wcParseNodeType::Parameter_Ident:		return "Parameter_Ident";
	case wcParseNodeType::DeclaratationBody:	return "DeclaratationBody";
	case wcParseNodeType::Identifier:			return "Identifier";
	case wcParseNodeType::Statement:			return "Statement";
	case wcParseNodeType::Block:				return "Block";
	case wcParseNodeType::Variable:				return "Variable";
	case wcParseNodeType::Type:					return "Type";
	case wcParseNodeType::Expression:			return "Expression";
	case wcParseNodeType::Expression_Assignment:return "Expression_Assignment";
	case wcParseNodeType::Expression_Comparison:return "Expression_Comparison";
	case wcParseNodeType::Expression_Equality:	return "Expression_Equality";
	case wcParseNodeType::Expression_Factor:	return "Expression_Factor";
	case wcParseNodeType::Expression_LogicAnd:	return "Expression_LogicAnd";
	case wcParseNodeType::Expression_LogicOr:	return "Expression_LogicOr";
	case wcParseNodeType::Expression_Primary:	return "Expression_Primary";
	case wcParseNodeType::Expression_Term:		return "Expression_Term";
	case wcParseNodeType::Expression_Call:		return "Expression_Call";
	case wcParseNodeType::Expression_Unary:		return "Expression_Unary";
	case wcParseNodeType::Expression_Operator:	return "Expression_Operator";
	default:
		return "";
	}
}

wcParseNodeType wcTokenTypeToParseNodeType(wcTokenType Type)
{
	switch (Type)
	{
	case NullToken:
		return wcParseNodeType::Head;

	default:
		return wcParseNodeType::Head;
	}
}

weec::parse::wcParseSymbolTable::wcParseSymbolTable()
	: NullSymbol(wcParseSymbolType::Null, wcFullIdentifier(string(""), string("")), wcToken())
{
	//global scope
	Add(wcParseSymbol(wcParseSymbolType::Namespace, wcFullIdentifier(ParserConsts.GlobalIdentifier, ParserConsts.GlobalIdentifier), wcToken()));

	//built-in types
	Add(wcParseSymbol(wcParseSymbolType::Type, wcFullIdentifier(ParserConsts.GlobalIdentifier, string("void")), wcToken()));
	Add(wcParseSymbol(wcParseSymbolType::Type, wcFullIdentifier(ParserConsts.GlobalIdentifier, string("int")), wcToken()));
	Add(wcParseSymbol(wcParseSymbolType::Type, wcFullIdentifier(ParserConsts.GlobalIdentifier, string("uint")), wcToken()));
	Add(wcParseSymbol(wcParseSymbolType::Type, wcFullIdentifier(ParserConsts.GlobalIdentifier, string("double")), wcToken()));
	Add(wcParseSymbol(wcParseSymbolType::Type, wcFullIdentifier(ParserConsts.GlobalIdentifier, string("float")), wcToken()));
	Add(wcParseSymbol(wcParseSymbolType::Type, wcFullIdentifier(ParserConsts.GlobalIdentifier, string("bool")), wcToken()));
	Add(wcParseSymbol(wcParseSymbolType::Type, wcFullIdentifier(ParserConsts.GlobalIdentifier, string("string")), wcToken()));
}

wcParseSymbolTable& weec::parse::wcParseSymbolTable::operator=(const wcParseSymbolTable& Other)
{
	Container = Other.Container;
	return *this;
}

bool weec::parse::wcParseSymbolTable::Add(wcParseFunctionSignature Sig, vector<wcParseParameter> _Parameters, bool SetScopeToThisSymbol)
{
	if (Exists(Sig.to_string()))
		return false;

	//Add the function symbol for identifier lookups
	Add(wcParseSymbol(Sig));

	//add the function signature
	FunctionContainer.insert(make_pair(Sig.to_string(), Sig));

	auto FunctionAlias = wcParseSymbol(Sig);
	FunctionAlias.Type = wcParseSymbolType::FunctionAlias;
	FunctionAlias.FullIdent = wcParseSymbol(Sig).FullIdent.to_string_no_arguments();
	Add(FunctionAlias);

	//register parameters as variables scoped to this function
	//Scopes.push(wcParseScope(Sig.to_string())); << todo

	//for (auto& Parameter : _Parameters)
	//	Add(wcParseSymbol(wcParseSymbolType::Variable, Parameter.Identifier, Sig.FunctionIdentifier.to_string()), Parameter.TypeIdentifier, Parameter.IdentifierToken));

	return true;
}

bool weec::parse::wcParseSymbolTable::Add(wcParseSymbol Sym, bool SetScopeToThisSymbol)
{
	if (Exists(Sym.FullIdent))
		return false;

	Sym.Registered = true;
	Container.insert(make_pair(Sym.FullIdent.to_string(), Sym));

	if(Sym.Type == wcParseSymbolType::Function)
	{
		auto FunctionAlias = Sym;
		FunctionAlias.Type = wcParseSymbolType::FunctionAlias;
		FunctionAlias.FullIdent = Sym.FullIdent.to_string_no_arguments();
		Add(FunctionAlias);
	}

	//Scopes.top().Symbols.push_back(Sym.FullIdent);	<< todo

	return true;
}

bool weec::parse::wcParseSymbolTable::Exists(wcFullIdentifier FullIdent) const
{

	for (auto FuncSig : FunctionContainer)
	{
		auto t1 = FuncSig.second.FunctionIdentifier.to_string_no_arguments();
		auto t2 = FullIdent.to_string();

		if (t1 == t2)
			return true;
	}

	return Container.find(FullIdent.to_string()) != Container.end() ? true : false;
}

wcParseSymbol weec::parse::wcParseSymbolTable::Get(wcFullIdentifier FullIdent) const
{
	return (Container.find(FullIdent.to_string()) != Container.end())
		? Container.at(FullIdent.to_string())
		: NullSymbol;
}

weec::parse::wcIdentifier::wcIdentifier(const wcIdentifier& Other) : Identifier(Other.Identifier)
{
	Identifier = Other.Identifier;
}

weec::parse::wcIdentifier::wcIdentifier(std::string _Data)
{
	Identifier = _Data;
}

weec::parse::wcFullIdentifier::wcFullIdentifier(std::string FullIdent, std::vector<wcParseSymbol> Arguments)
{
	//wcIdentalyzer().Create(FullIdent, Arguments, ScopeIdentifier, ShortIdentifier);
}

weec::parse::wcFullIdentifier::wcFullIdentifier(std::string FullIdent, std::vector<wcParseExpression> Arguments)
{
	wcIdentalyzer().Create(FullIdent, Arguments, ScopeIdentifier, ShortIdentifier);
}

weec::parse::wcFullIdentifier::wcFullIdentifier(std::string FullIdent, std::vector<wcParseParameter> Parameters)
{
	wcIdentalyzer().Create(FullIdent, Parameters, ScopeIdentifier, ShortIdentifier);
}

weec::parse::wcParseSymbol::wcParseSymbol() : FullIdent()
{
	Type = wcParseSymbolType::Null;
}

weec::parse::wcParseSymbol::wcParseSymbol(const wcParseSymbol& Other)
{
	Type = Other.Type;
	FullIdent = Other.FullIdent;
	Const = Other.Const;
	Arguments = Other.Arguments;
	DataType = Other.DataType;
	Registered = Other.Registered;
	IdentToken = Other.IdentToken;
	HasOverloads = false;
}

weec::parse::wcParseSymbol::wcParseSymbol(wcParseSymbolType _Type, wcFullIdentifier _FullIdent, wcToken _Token) 
	: FullIdent(_FullIdent)
{
	FullIdent = wcFullIdentifier(_FullIdent);
	Type = _Type;
	IdentToken = _Token;
	Const = false;
	Registered = false;
	HasOverloads = false;
	Arguments = 0;
}

weec::parse::wcParseSymbol::wcParseSymbol(wcParseSymbolType _Type, wcFullIdentifier _FullIdent, wcFullIdentifier _DataType, wcToken _Token) 
	: FullIdent(_FullIdent), DataType(_DataType)
{
	FullIdent = wcFullIdentifier(_FullIdent);
	Type = _Type;
	DataType = _DataType;
	IdentToken = _Token;
	Const = false;
	Registered = false;
	HasOverloads = false;
	Arguments = 0;
}

weec::parse::wcParseSymbol::wcParseSymbol(wcParseFunctionSignature FuntionSignature) 
	: FullIdent(FuntionSignature.FunctionIdentifier.to_string()), DataType(FuntionSignature.DataType)
{
	Type = wcParseSymbolType::Function;
	IdentToken = FuntionSignature.IdentToken;
	Const = false;
	Registered = false;
	HasOverloads = false;
	Arguments = FuntionSignature.FunctionIdentifier.ArgumentCount();
}

wcIdentalyzerAnalyzeResultCode weec::parse::wcIdentalyzer::Analyze(std::string PossibleIdentifier)
{
	//if(IsValid(PossibleIdentifier))
	if (IsQualified(PossibleIdentifier))
		return IsFunction(PossibleIdentifier)
			? wcIdentalyzerAnalyzeResultCode::QualifiedFunction
			: wcIdentalyzerAnalyzeResultCode::QualifiedIdentifier;
	else
		return IsFunction(PossibleIdentifier)
			? wcIdentalyzerAnalyzeResultCode::ShortFunction
			: wcIdentalyzerAnalyzeResultCode::ShortIdentifier;
	//else
	//	return wcIdentalyzerAnalyzeResultCode::InvalidIdentifier;
}

bool weec::parse::wcIdentalyzer::Create(string IdentifierString, wcIdentifierScope& ScopeOutput, wcIdentifier& IdentifierOutput)
{
	switch (Analyze(IdentifierString))
	{
	case wcIdentalyzerAnalyzeResultCode::ShortFunction:
	case wcIdentalyzerAnalyzeResultCode::ShortIdentifier:
		ScopeOutput = wcIdentifierScope();
		IdentifierOutput = wcIdentifier(IdentifierString);
		return true;

	case wcIdentalyzerAnalyzeResultCode::QualifiedIdentifier:
	case wcIdentalyzerAnalyzeResultCode::QualifiedFunction:
		ScopeOutput = wcIdentifierScope(GetNamespaceFromQualifiedIdentifier(IdentifierString));
		IdentifierOutput = wcIdentifier(GetIdentifierFromQualifiedIdentifier(IdentifierString));
		return true;

	case wcIdentalyzerAnalyzeResultCode::InvalidIdentifier:
		break;
	}
	return false;
}

bool weec::parse::wcIdentalyzer::Create(string IdentifierString, const vector<wcParseExpression>& Parameters, wcIdentifierScope& ScopeOutput, wcIdentifier& IdentifierOutput)
{
	vector<wcParseParameter> NewParams;
	for (auto& Param : Parameters)
		NewParams.push_back(wcParseParameter(Param.DataType, ""));
	return Create(IdentifierString, NewParams, ScopeOutput, IdentifierOutput);
}

bool weec::parse::wcIdentalyzer::Create(string IdentifierString, const vector<wcParseParameter>& Parameters, wcIdentifierScope& ScopeOutput, wcIdentifier& IdentifierOutput)
{
	switch (Analyze(IdentifierString))
	{
	case wcIdentalyzerAnalyzeResultCode::ShortIdentifier:
		ScopeOutput = wcIdentifierScope();
		IdentifierOutput = wcIdentifier(IdentifierString + "("+ GetParameterListIdentifierString(Parameters) + ")");
		return true;

	case wcIdentalyzerAnalyzeResultCode::ShortFunction:
		ScopeOutput = wcIdentifierScope();
		IdentifierOutput = wcIdentifier(IdentifierString);
		return true;

	case wcIdentalyzerAnalyzeResultCode::QualifiedIdentifier:
		ScopeOutput = wcIdentifierScope(GetNamespaceFromQualifiedIdentifier(IdentifierString));
		IdentifierOutput = wcIdentifier(GetIdentifierFromQualifiedIdentifier(IdentifierString) + "(" + GetParameterListIdentifierString(Parameters) + ")");
		return true;
	case wcIdentalyzerAnalyzeResultCode::QualifiedFunction:
		ScopeOutput = wcIdentifierScope(GetNamespaceFromQualifiedIdentifier(IdentifierString));
		IdentifierOutput = wcIdentifier(GetIdentifierFromQualifiedIdentifier(IdentifierString));
		return true;

	case wcIdentalyzerAnalyzeResultCode::InvalidIdentifier:
		break;
	}
	return false;
}

std::string weec::parse::wcIdentalyzer::GetIdentifierFromQualifiedIdentifier(std::string IdentifierString)
{
	if (IsFunction(IdentifierString))
		if (IsQualified(IdentifierString))
		{
			auto noArgs = IdentifierString.substr(0, IdentifierString.find_first_of("("));
			return IdentifierString.substr(noArgs.find_last_of(ParserConsts.ScopeDelimiter) + 1, IdentifierString.size() - noArgs.find_last_of(ParserConsts.ScopeDelimiter) + 1);
		}
		else
			return IdentifierString;		 
	
	return IsQualified(IdentifierString)
		? IdentifierString.substr(IdentifierString.find_last_of(ParserConsts.ScopeDelimiter) + 1, IdentifierString.size() - IdentifierString.find_last_of(ParserConsts.ScopeDelimiter) + 1)
		: IdentifierString;
}

std::string weec::parse::wcIdentalyzer::GetNamespaceFromQualifiedIdentifier(std::string IdentifierString)
{
	return IsQualified(IdentifierString)
		? IsFunction(IdentifierString)
			? wcIdentalyzer().StripArgumentsFromFunctionIdentifier(IdentifierString).substr(0, wcIdentalyzer().StripArgumentsFromFunctionIdentifier(IdentifierString).find_last_of(ParserConsts.ScopeDelimiter) - 1)
			: IdentifierString.substr(0, IdentifierString.find_last_of(ParserConsts.ScopeDelimiter) - 1)
		: IdentifierString;
}

std::string weec::parse::wcIdentalyzer::GetParameterListIdentifierString(const std::vector<wcParseParameter>& Parameters)
{
	std::string ParameterListString;

	for (auto& Parameter : Parameters)
		ParameterListString += ParameterListString.size()
			? "," + Parameter.TypeIdentifier.to_string()
			: Parameter.TypeIdentifier.to_string();

	return ParameterListString;
}

std::string weec::parse::wcIdentalyzer::GetParameterListIdentifierString(const std::vector<wcFullIdentifier>& Parameters)
{
	std::string ParameterListString;

	for (auto& Parameter : Parameters)
		ParameterListString += ParameterListString.size()
			? "," + Parameter.to_string()
			: Parameter.to_string();

	return ParameterListString;
}

std::string weec::parse::wcIdentalyzer::StripArgumentsFromFunctionIdentifier(std::string FunctionIdentifierString)
{
	return IsFunction(FunctionIdentifierString)
		? FunctionIdentifierString.substr(0, FunctionIdentifierString.find_first_of("("))
		: FunctionIdentifierString;
}

bool weec::parse::wcIdentalyzer::IsQualified(std::string PossibleIdentifier)
{
	if (IsFunction(PossibleIdentifier))
		return StripArgumentsFromFunctionIdentifier(PossibleIdentifier).find(ParserConsts.ScopeDelimiter) != std::string::npos
			? true
			: false;
	else
		return PossibleIdentifier.find(ParserConsts.ScopeDelimiter) != std::string::npos;	//could still be an invalid ident
}

bool weec::parse::wcIdentalyzer::IsQualifiedWithGlobal(std::string PossibleIdentifier)
{
	return (PossibleIdentifier.starts_with(ParserConsts.GlobalIdentPrefix) || (PossibleIdentifier == ParserConsts.GlobalIdentifier))
		? true	//could still be an invalid ident
		: false;
}

bool weec::parse::wcIdentalyzer::IsFunction(std::string PossibleIdentifier)
{
	return (PossibleIdentifier.find_first_of("(") != std::string::npos	&& PossibleIdentifier.find_last_of(")") == PossibleIdentifier.size() - 1 )
		? true	//could still be an invalid ident
		: false;
}

bool weec::parse::wcIdentalyzer::IsValid(std::string PossibleIdentifier)
{
	return lex::wcTokenTypeAlizer().IsValidIdent(PossibleIdentifier);
}

weec::parse::wcParseScopes::wcParseScopes()
{
	//global scope
	Push(wcParseScope(ParserConsts.GlobalIdentifier, { "void","int","uint","double","float","bool","string" }));
}

wcParseScopeResolveResult weec::parse::wcParseScopes::Resolve(wcIdentifier In, wcFullIdentifier& Out, bool ReturnFirstMatch )
{
	stack<wcParseScope> Receiver;
	unsigned int MatchingIdentifierCount = 0;
	auto Result = wcParseScopeResolveResult::Unresolved;

	while (Size() > 0)
	{
		//check if top stackframe declares the given identifier
		for (auto& Symbol : Top().Container)
			if (Symbol.second.GetIdentifier().to_string_no_arguments() == In)
			{
				//found it
				MatchingIdentifierCount++;
				Out = Symbol.second;
				Result = wcParseScopeResolveResult::Resolved;

				//return the first match we get if were forced to resolve ambiguous symbols
				if (ReturnFirstMatch)
					goto _wcIdentifierResolver_Resolve_Cleanup;
			}

		//pop this stackframe, try the next if there is one
		Receiver.push(Top());
		Pop();
	}

	Result = MatchingIdentifierCount > 1 
		? wcParseScopeResolveResult::Ambiguous 
		: MatchingIdentifierCount == 1
		? wcParseScopeResolveResult::Resolved
		: wcParseScopeResolveResult::Unresolved;
	

	//restore Scopes
_wcIdentifierResolver_Resolve_Cleanup:
	while (Receiver.size() > 0)
	{
		Push(Receiver.top());
		Receiver.pop();
	}

	return Result;
}