#include "parse.h"
#include "lex.h"

using namespace std;
using namespace weec;
using namespace weec::lex;
using namespace weec::parse;

using enum weec::parse::wcParserErrorCode;
using enum weec::lex::wcTokenType;
using enum weec::parse::wcParseNodeType;

struct wcParserConsts
{
	std::string Parser_GlobalScopeIdent = "$g",
		Parser_ScopeDelimiter = "::";
};

static wcParserConsts ParserConsts;

weec::parse::wcParser::wcParser(lex::wcTokenizer& _Tokenizer) : Tokenizer(_Tokenizer)
{

}

wcParseOutput weec::parse::wcParser::Parse()
{
	wcParseOutput Output;

	Tokenizer.NextToken();

	while (!Tokenizer.IsFinished() && !Tokenizer.IsErrored() && Output.Error.Code == None)
	{
		auto ThisToken = Tokenizer.GetToken();

		Output.AddAsChild(ParseStatement(true));
	}

	Output.SymbolTable = SymbolTable;

	return Output;
}

wcParseOutput weec::parse::wcParser::ParseDeclaration()
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::Declaration), true);
	auto OriginalScope = SymbolTable.GetCurrentScope();

	//Declaration_Type
	auto TypeToken = Tokenizer.GetToken();
	if (!TypeToken.IsBuiltinType() && TypeToken.Type != Identifier)
		return wcParseOutput(wcParserError(InvalidType, TypeToken));		//error - expected built in or user type
	Output.AddAsChild(wcParseNode(wcParseNodeType::Declaration_Type, TypeToken));
	auto DeclarationType = wcFullIdent(TypeToken.StringToken.Data, SymbolTable.GetCurrentScope().FullIdent.to_string());

	//Declaration_Ident
	if (!Tokenizer.NextToken(Identifier))
		return wcParseOutput(wcParserError(UnexpectedToken, TypeToken));
	auto IdentToken = Tokenizer.GetToken();
	auto DeclarationIdent = wcFullIdent(IdentToken.StringToken.Data, SymbolTable.GetCurrentScope().FullIdent.to_string());
	if (SymbolTable.Exists(DeclarationIdent))
		return wcParseOutput(wcParserError(IdentRedeclaration, IdentToken));
	Output.AddAsChild(wcParseNode(wcParseNodeType::Declaration_Ident, IdentToken));

	//open parenthesis/semi colon/optional assignment
	if (!Tokenizer.NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));
	if (Tokenizer.GetToken().Type == wcTokenType::SemiColon)
	{
		//add to symbol table
		SymbolTable.Add(wcParseSymbol(wcParseSymbolType::Variable, DeclarationIdent, DeclarationType, IdentToken));

		Output.AddAsChild(ParseSemiColon());
		return Output;
	}
	else if (Tokenizer.GetToken().Type == wcTokenType::OpenParenthesis)
	{
		//add to symbol table

		//function declaration prototype
		wcFullIdent TrueFunctionName;
		Output.AddAsChild(ParseDeclarationArguments(DeclarationIdent, DeclarationType, IdentToken, TrueFunctionName));

		//optional semi colon
		if (Tokenizer.GetToken().Type == wcTokenType::SemiColon)
		{
			Output.AddAsChild(ParseSemiColon());
			return Output;
		}

		//no semi colon, must be a full declaration with body
		SymbolTable.SetScope(TrueFunctionName.to_string());
		Output.AddAsChild(ParseDeclarationBody());
		SymbolTable.SetScope(OriginalScope);
		return Output;
	}
	else if (Tokenizer.GetToken().Type != wcTokenType::AssignOperator)
		return Output;
	Tokenizer.NextToken();

	//add to symbol table, must be a variable
	SymbolTable.Add(wcParseSymbolType::Variable, DeclarationIdent, IdentToken);

	//optional assignment expression;
	Output.AddAsChild(wcExpressionParser(Tokenizer, SymbolTable).ParseExpression());
	if (Output.Error.Code != None)
		return Output;

	//semi colon
	Output.AddAsChild(ParseSemiColon());

	return Output;
}

wcFullIdent GetFullIdentForFunction(wcFullIdent DeclarationIdent, vector<wcParseSymbol> ArgumentSymbols)
{
	string Output = DeclarationIdent.to_string(); bool First = true;
	for (auto ArgumentSymbol : ArgumentSymbols)
	{
		Output += First ? "@" + ArgumentSymbol.DataType.to_string() : "," + ArgumentSymbol.DataType.to_string();
		First = false;
	}

	return wcFullIdent(Output);
}

wcParseOutput weec::parse::wcParser::ParseDeclarationArguments(wcFullIdent DeclarationIdent, wcFullIdent DeclarationType, wcToken IdentToken, wcFullIdent& ReturnTrueFunctionIdent)
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::DeclarationArguments), true);

	//consume openining parenthesis, error if we dont find it
	if (Tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
		return wcParseOutput(wcParserError(UnexpectedToken, Tokenizer.GetToken()));
	if (!Tokenizer.NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));

	//check for any arguments
	vector<wcParseSymbol> ArgumentSymbolsToRegister;
	while (!Tokenizer.IsFinished() && !Tokenizer.IsErrored() && Tokenizer.GetToken().Type != CloseParenthesis)
	{
		wcParseOutput ArgumentNode(wcParseNode(wcParseNodeType::DeclarationArgument), true);

		//argument type
		auto TypeToken = Tokenizer.GetToken();
		if (!TypeToken.IsBuiltinType() && TypeToken.Type != Identifier)
			return wcParseOutput(wcParserError(InvalidType, TypeToken));		//error - expected built in or user type
		ArgumentNode.AddAsChild(wcParseNode(wcParseNodeType::DeclarationArgument_Type, TypeToken));
		auto TypeIdentifier = wcFullIdent(TypeToken.StringToken.Data, ParserConsts.Parser_GlobalScopeIdent);

		//argument identifier
		if (!Tokenizer.NextToken(Identifier))
			return wcParseOutput(wcParserError(UnexpectedToken, TypeToken));
		auto IdentToken = Tokenizer.GetToken();
		auto ArgumentIdentifier = wcFullIdent(IdentToken.StringToken.Data, SymbolTable.GetCurrentScope().FullIdent.to_string());
		ArgumentNode.AddAsChild(wcParseNode(wcParseNodeType::DeclarationArgument_Ident, IdentToken));

		if (!Tokenizer.NextToken())
			return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));

		//Add the argument we just constructed to the output AST
		Output.AddAsChild(ArgumentNode);

		//add it to the symbol table once we have the full function signature
		ArgumentSymbolsToRegister.push_back(wcParseSymbol(wcParseSymbolType::Variable, ArgumentIdentifier, TypeIdentifier, IdentToken));

		//move on to closing parenthesis if we dont encounter a comma
		if (Tokenizer.GetToken().Type != Comma)
			break;
		if (!Tokenizer.NextToken())
			return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));
	}

	//add the function to symbol table with the short name, no arguments
	ReturnTrueFunctionIdent = GetFullIdentForFunction(DeclarationIdent, ArgumentSymbolsToRegister);
	SymbolTable.Add(wcParseSymbol(wcParseSymbolType::Function, ReturnTrueFunctionIdent, DeclarationType, IdentToken));
	SymbolTable.Add(wcParseFunctionSignature(wcParseSymbol(wcParseSymbolType::Function, ReturnTrueFunctionIdent, DeclarationType, IdentToken), ArgumentSymbolsToRegister));

	//set the scope to this function so the arguments are in the correct scope
	auto OriginalScope = SymbolTable.GetCurrentScope();
	SymbolTable.SetScope(ReturnTrueFunctionIdent);

	//make a delayed attempt to register each argument
	for (auto SymbolToRegister : ArgumentSymbolsToRegister)
	{
		//correct the scope
		SymbolToRegister.FullIdent.Scope = ReturnTrueFunctionIdent.to_string();

		//check its not a redeclaration
		if (SymbolTable.Exists(SymbolToRegister.FullIdent))
			return wcParseOutput(wcParserError(IdentRedeclaration, SymbolToRegister.IdentToken));

		//add
		SymbolTable.Add(SymbolToRegister);
	}

	//reset scope
	SymbolTable.SetScope(OriginalScope);

	//consume closing parenthesis
	if (Tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
		return wcParseOutput(wcParserError(UnexpectedToken, Tokenizer.GetToken()));
	Tokenizer.NextToken();

	return Output;
}

wcParseOutput weec::parse::wcParser::ParseDeclarationBody()
{
	return ParseBlock(true);
}


weec::parse::wcIdentParser::wcIdentParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable) : Tokenizer(_Tokenizer), SymbolTable(_SymbolTable)
{

}

wcParseSymbol weec::parse::wcIdentParser::ParseIdent()
{
	auto Ident1 = Tokenizer.GetToken();

	wcFullIdent FullIdent(wcIdent(Ident1.StringToken.Data), wcScope(SymbolTable.GetCurrentScope().FullIdent.to_string()));

	if (SymbolTable.Exists(FullIdent))
	{

	}
	wcParseSymbol Symbol(SymbolTable.ClassifyIdent(Ident1).Type, FullIdent, Ident1);

	Symbol.FullIdent = FullIdent;
	return Symbol;
}

wcParseOutput weec::parse::wcParser::ParseStatement(bool AllowDeclarations)
{
	wcParseOutput Output(wcParseNode(wcParseNodeType::Statement), true);

	auto ThisToken = Tokenizer.GetToken();
	wcParseOutput Buffer;
	switch (ThisToken.Type)
	{
		WC_SWITCHCASE_TOKENS_LITERAL
			WC_SWITCHCASE_TOKENS_OPERATORS_ALL
	case OpenParenthesis:
		Buffer = wcExpressionParser(Tokenizer, SymbolTable).ParseExpression();
		if (Buffer.Error.Code != None)
			return Buffer;
		Buffer.AddAsChild(ParseSemiColon());
		break;

		WC_SWITCHCASE_TOKENS_BUILTIN_TYPES
			if (!AllowDeclarations)
				return wcParseOutput(wcParserError(wcParserErrorCode::DeclarationsProhibited, Tokenizer.GetToken()));
		Buffer = ParseDeclaration();
		break;

	case wcTokenType::SemiColon:
		Buffer = ParseSemiColon();
		break;

	case wcTokenType::IfKeyword:
		Buffer = ParseIf();
		break;

	case wcTokenType::ReturnKeyword:
		Buffer = ParseReturn();
		break;

	case wcTokenType::WhileKeyword:
		Buffer = ParseWhile();
		break;

	default:
		//error
		Output.Error = wcParserError(wcParserErrorCode::UnexpectedToken, Tokenizer.GetToken());
		return Output;

	case Identifier:
		auto Ident = wcIdentParser(Tokenizer, SymbolTable).ParseIdent();
		switch (Ident.Type)
		{
		case wcParseSymbolType::Type:
			if (!AllowDeclarations)
				return wcParseOutput(wcParserError(wcParserErrorCode::DeclarationsProhibited, Tokenizer.GetToken()));
			//Buffer = ParseDeclaration(Ident);
			break;

		case wcParseSymbolType::Variable:
		case wcParseSymbolType::Function:
			Buffer = wcExpressionParser(Tokenizer, SymbolTable).ParseExpression(Ident);
			if (Buffer.Error.Code != None)
				return Buffer;
			if (Tokenizer.GetToken().Type != SemiColon)
				return wcParseOutput(wcParserError(wcParserErrorCode::UnexpectedToken, Tokenizer.GetToken()));
			Tokenizer.NextToken();
			break;

		case wcParseSymbolType::Namespace:
		case wcParseSymbolType::Null:
			break;

		default:
			//error
			Output.Error = wcParserError(wcParserErrorCode::UnexpectedToken, Tokenizer.GetToken());
			return Output;
			break;
		}
		break;
	}

	Output.AddAsChild(Buffer);
	return Output;
}

wcParseOutput weec::parse::wcParser::ParseBlock(bool AllowDeclarations)
{
	wcParseOutput Output;

	if (Tokenizer.GetToken().Type != OpenBrace)
		return wcParseOutput(wcParserError(UnexpectedToken, Tokenizer.GetToken()));
	if (!Tokenizer.NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));

	Output.AddAsChild(wcParseNode(wcParseNodeType::Block), true);

	while (Tokenizer.GetToken().Type != CloseBrace)
	{
		Output.AddAsChild(ParseStatement(AllowDeclarations));

		if (Tokenizer.IsErrored() || Output.Error.Code != None)
			return Output;
	}

	if (Tokenizer.GetToken().Type != CloseBrace)
		return wcParseOutput(wcParserError(MissingClosingBrace, Tokenizer.GetToken()));

	Tokenizer.NextToken();

	return Output;
}

wcParseOutput weec::parse::wcParser::ParseIf()
{
	wcParseOutput Output;
	Output.AddAsChild(wcParseNode(wcParseNodeType::IfStatement), true);

	//if keyword
	if (Tokenizer.GetToken().Type != IfKeyword)
		return wcParseOutput(wcParserError(UnexpectedToken, Tokenizer.GetToken()));		//error - expected built in or user type

	// ( 
	if (!Tokenizer.NextToken(OpenParenthesis))
		return wcParseOutput(wcParserError(UnexpectedToken, Tokenizer.GetToken()));
	if (!Tokenizer.NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));

	//expression
	Output.AddAsChild(wcExpressionParser(Tokenizer, SymbolTable).ParseExpression());
	if (Output.Error.Code != None)
		return Output;

	// )
	if (Tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
		return wcParseOutput(wcParserError(If_MissingClosingParenthesis, Tokenizer.GetToken()));
	if (!Tokenizer.NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));

	//block/statement
	Output.AddAsChild(wcParseNode(wcParseNodeType::If_TrueBlock), true);
	if (Tokenizer.GetToken().Type == wcTokenType::OpenBrace)
		Output.AddAsChild(ParseBlock(false));
	else
		Output.AddAsChild(ParseStatement(false));
	Output.Up();	//return pointer to parent of true block
	if (Output.Error.Code != None)
		return Output;

	//optional else keyword
	if (Tokenizer.GetToken().Type != wcTokenType::ElseKeyword)
		return Output;
	Tokenizer.NextToken();

	//optional else block/statement
	Output.AddAsChild(wcParseNode(wcParseNodeType::If_ElseBlock), true);
	if (Tokenizer.GetToken().Type == wcTokenType::OpenBrace)
		Output.AddAsChild(ParseBlock(false));
	else
		Output.AddAsChild(ParseStatement(false));
	Output.Up();	//return pointer to parent of else block

	return Output;
}

//Note: always returns an empty parseoutput, we only need to know if it errored or not, so always 
//append it to parent output to see the error cascade back up the call chain
wcParseOutput weec::parse::wcParser::ParseSemiColon()
{
	if (Tokenizer.GetToken().Type != SemiColon)
		if (Tokenizer.IsFinished())
			return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));		//error
		else
			return wcParseOutput(wcParserError(UnexpectedToken, Tokenizer.GetToken()));		//error
	Tokenizer.NextToken();

	//no nodes created
	return wcParseOutput();
}

wcParseOutput weec::parse::wcParser::ParseReturn()
{
	wcParseOutput Output;
	Output.AddAsChild(wcParseNode(wcParseNodeType::ReturnStatement), true);

	//return keyword;
	if (Tokenizer.GetToken().Type != ReturnKeyword)
		return wcParseOutput(wcParserError(UnexpectedToken, Tokenizer.GetToken()));		//error	
	if (!Tokenizer.NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));		//error

	//semi colon/optional return expression
	if (Tokenizer.GetToken().Type == SemiColon)
	{
		Output.AddAsChild(ParseSemiColon());
		return Output;
	}

	Output.AddAsChild(wcExpressionParser(Tokenizer, SymbolTable).ParseExpression());
	if (Output.Error.Code != None)
		return Output;

	Output.AddAsChild(ParseSemiColon());
	return Output;
}

wcParseOutput weec::parse::wcParser::ParseWhile()
{
	wcParseOutput Output;
	Output.AddAsChild(wcParseNode(wcParseNodeType::WhileStatement), true);

	//while keyword
	if (Tokenizer.GetToken().Type != WhileKeyword)
		return wcParseOutput(wcParserError(UnexpectedToken, Tokenizer.GetToken()));		//error - expected built in or user type

	// ( 
	if (!Tokenizer.NextToken(OpenParenthesis))
		return wcParseOutput(wcParserError(UnexpectedToken, Tokenizer.GetToken()));
	if (!Tokenizer.NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));

	//expression
	Output.AddAsChild(wcExpressionParser(Tokenizer, SymbolTable).ParseExpression());
	if (Output.Error.Code != None)
		return Output;

	// )
	if (Tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
		return wcParseOutput(wcParserError(If_MissingClosingParenthesis, Tokenizer.GetToken()));
	if (!Tokenizer.NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));

	//block/statement
	if (Tokenizer.GetToken().Type == wcTokenType::OpenBrace)
		Output.AddAsChild(ParseBlock(false));
	else
		Output.AddAsChild(ParseStatement(false));
	if (Output.Error.Code != None)
		return Output;

	return Output;
}

tree<wcParseNode>::pre_order_iterator weec::parse::wcParseExpression::GetExpressionRootNodeBegin()
{
	auto it = AST.begin();
	while (it != AST.end())
	{
		if (it->Type == Expression)
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

void weec::parse::wcParseExpression::Eval()
{
	//tree<wcParseNode>::iterator
}

bool weec::parse::wcParseExpression::IsErrored()
{
	return false;
}

tree<wcParseNode>::pre_order_iterator weec::parse::wcParseExpression::GetExpressionNodeBegin()
{
	auto t = GetExpressionRootNodeBegin();
	t++;
	return t;
}

weec::parse::wcParseExpression::wcParseExpression()
{
	auto ExpNode = AST.insert(AST.begin(), *new wcParseNode(Expression));
	Type = wcParseExpressionType::Grouping;
}

weec::parse::wcParseExpression::wcParseExpression(wcParserError _Error)
{
	auto ExpNode = AST.insert(AST.begin(), *new wcParseNode(Expression));
	Error = _Error;
	Type = wcParseExpressionType::Grouping;
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, lex::wcToken OperatorOrLiteral)
{
	auto ExpNode = AST.insert(AST.begin(), *new wcParseNode(Expression));

	AST.append_child(ExpNode, *new wcParseNode(HeadType, OperatorOrLiteral));

	Tokens.push_back(OperatorOrLiteral);

	Type = OperatorOrLiteral.Type == wcTokenType::Identifier ? wcParseExpressionType::Variable : wcParseExpressionType::Literal;
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, wcParseExpression LeftHand, wcToken Operator, wcParseExpression RightHand)
{
	//build the ast
	auto ExpRootNode = AST.insert(AST.begin(), *new wcParseNode(Expression));

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

	if (Operator.Type == wcTokenType::AssignOperator)
		Type = wcParseExpressionType::Assignment;
	else if (Operator.Type == wcTokenType::LogAndOperator || Operator.Type == wcTokenType::LogOrOperator)
		Type = wcParseExpressionType::Logical;
	else
		Type = wcParseExpressionType::Binary;
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, lex::wcToken Operator, wcParseExpression RightHand)
{
	auto ExpRootNode = AST.insert(AST.begin(), *new wcParseNode(Expression));

	auto OpNode = AST.append_child(ExpRootNode, wcParseNode(HeadType, Operator));
	auto OpNodeChild = AST.append_child(OpNode);

	AST.insert_subtree(OpNodeChild, RightHand.GetExpressionNodeBegin());
	AST.erase(OpNodeChild);

	Tokens.push_back(Operator);

	for (auto t = RightHand.Tokens.begin(); t != RightHand.Tokens.end(); ++t)
		Tokens.push_back(*t);

	Error = RightHand.Error;
	Type = wcParseExpressionType::Unary;
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, wcParseExpression Callee, std::vector<wcParseExpression> Arguments)
{
	auto ExpNode = AST.insert(AST.begin(), *new wcParseNode(Expression));
	auto ExpNodeChild = AST.append_child(ExpNode);

	auto CalleeNode = AST.insert_subtree(ExpNodeChild, Callee.GetExpressionNodeBegin());
	CalleeNode.node->data.Type = Expression_Call;

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
}


wcParseOutput weec::parse::wcExpressionParser::ParseExpression()
{
	if (Tokenizer.IsFinished())
		return wcParserError(wcParserErrorCode::Expression_Empty, Tokenizer.GetToken());

	auto eExpression = ParseExpression_Expression();
	wcParseOutput Output;
	Output.AST = eExpression.AST;
	Output.Error = eExpression.Error;
	return Output;
}
wcParseOutput weec::parse::wcExpressionParser::ParseExpression(wcParseSymbol Symbol)
{

	if (Tokenizer.IsFinished())
		return wcParserError(wcParserErrorCode::Expression_Empty, Tokenizer.GetToken());

	wcParseOutput Output;
	auto Expression = ParseExpression_Expression();
	if (Expression.Error.Code != wcParserErrorCode::None)
	{
		Output.Error = Expression.Error;
		return Output;
	}

	if (Tokenizer.GetToken().Type != wcTokenType::SemiColon)
	{
		Output.Error = wcParserError(wcParserErrorCode::Expression_UnexpectedToken, Tokenizer.GetToken());
		return Output;
	}

	Output.AST = Expression.AST;
	Output.Error = Expression.Error;
	return Output;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Expression()
{
	wcParseExpression Expression1;

	Expression1 = ParseExpression_SubExpression();


	return Expression1;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_SubExpression()
{
	auto Expression = ParseExpression_Assignment();

	//if(!Tokenizer.IsFinished())

	return Expression;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Assignment()
{
	wcParseExpression Output = ParseExpression_LogicOr();
	if (Output.Error.Code != None)
		return Output;

	auto Operator = Tokenizer.GetToken();
	while (Operator.Type == AssignOperator)
	{
		Tokenizer.NextToken();

		if (Output.Type != wcParseExpressionType::Variable)
		{
			Output.Error = wcParserError(wcParserErrorCode::Expression_NotAnLValue, Tokenizer.GetToken());
			return Output;
		}
		auto RightExp = ParseExpression_Assignment();

		return wcParseExpression(Expression_Assignment, Output, Operator, RightExp);
	}

	return Output;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Equality()
{
	wcParseExpression Output = ParseExpression_Comparison();
	if (Output.Error.Code != None)
		return Output;

	auto Operator = Tokenizer.GetToken();
	while (Operator.Type == NotEqualOperator || Operator.Type == EqualOperator)
	{
		Tokenizer.NextToken();
		auto RightExp = ParseExpression_Comparison();
		Output = wcParseExpression(Expression_Equality, Output, Operator, RightExp);

		Operator = Tokenizer.GetToken();
	}

	return Output;
}


wcParseExpression weec::parse::wcExpressionParser::ParseExpression_LogicOr()
{
	wcParseExpression Output = ParseExpression_LogicAnd();
	if (Output.Error.Code != None)
		return Output;

	auto Operator = Tokenizer.GetToken();
	while (Operator.Type == wcTokenType::LogOrOperator)
	{
		Tokenizer.NextToken();
		auto RightExp = ParseExpression_LogicAnd();
		Output = wcParseExpression(Expression_LogicOr, Output, Operator, RightExp);

		Operator = Tokenizer.GetToken();
	}

	return Output;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_LogicAnd()
{
	wcParseExpression Output = ParseExpression_Equality();
	if (Output.Error.Code != None)
		return Output;

	auto Operator = Tokenizer.GetToken();
	while (Operator.Type == wcTokenType::LogAndOperator)
	{
		Tokenizer.NextToken();
		auto RightExp = ParseExpression_Equality();
		Output = wcParseExpression(Expression_LogicAnd, Output, Operator, RightExp);

		Operator = Tokenizer.GetToken();
	}

	return Output;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Comparison()
{
	wcParseExpression Output = ParseExpression_Term();
	if (Output.Error.Code != None)
		return Output;

	auto Operator = Tokenizer.GetToken();
	while (Operator.Type == GreaterEqualOperator || Operator.Type == GreaterOperator ||
		Operator.Type == LessOperator || Operator.Type == LessEqualOperator)
	{
		Tokenizer.NextToken();
		auto RightExp = ParseExpression_Term();
		Output = wcParseExpression(Expression_Comparison, Output, Operator, RightExp);

		Operator = Tokenizer.GetToken();
	}

	return Output;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Term()
{
	wcParseExpression Output = ParseExpression_Factor();
	if (Output.Error.Code != None)
		return Output;

	auto Operator = Tokenizer.GetToken();
	while (Operator.Type == MinusOperator || Operator.Type == PlusOperator)
	{
		if (!Tokenizer.NextToken())
		{
			Output.Error = wcParserError(wcParserErrorCode::Expression_UnexpectedEOF, Tokenizer.GetToken());
			return Output;
		}

		auto RightExp = ParseExpression_Factor();

		Output = wcParseExpression(Expression_Term, Output, Operator, RightExp);

		Operator = Tokenizer.GetToken();
	}

	return Output;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Factor()
{
	wcParseExpression Output = ParseExpression_Unary();
	if (Output.Error.Code != None)
		return Output;

	auto Operator = Tokenizer.GetToken();
	while (Operator.Type == DivideOperator || Operator.Type == MultiplyOperator)
	{
		Tokenizer.NextToken();
		auto RightExp = ParseExpression_Unary();
		Output = wcParseExpression(Expression_Factor, Output, Operator, RightExp);

		Operator = Tokenizer.GetToken();
	}

	return Output;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Unary()
{
	//wcParseExpression Output = ParseExpression_Primary();

	auto Operator = Tokenizer.GetToken();
	if (Operator.Type == MinusOperator || Operator.Type == PlusOperator || Operator.Type == LogNotOperator)
	{
		Tokenizer.NextToken();
		auto RightExp = ParseExpression_Unary();
		return wcParseExpression(Expression_Unary, Operator, RightExp);
	}

	return ParseExpression_Call();
}
/*
private Expr call() {
	Expr expr = primary();

	while (true) {
		if (match(LEFT_PAREN)) {
			expr = finishCall(expr);
		}
		else {
			break;
		}
	}

	return expr;
}
*/
wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Call()
{
	wcParseExpression Output = ParseExpression_Primary();
	if (Output.Error.Code != None)
		return Output;

	if (Tokenizer.GetToken().Type == wcTokenType::OpenParenthesis)
	{
		if (!Tokenizer.NextToken())
			return wcParseExpression(wcParserError(wcParserErrorCode::UnexpectedEOF, Tokenizer.GetToken()));

		Output = ParseExpression_CallArguments(Output);
	}

	return Output;
}
/*
private Expr finishCall(Expr callee) {
	List<Expr> arguments = new ArrayList<>();
	if (!check(RIGHT_PAREN)) {
	  do {
		arguments.add(expression());
	  } while (match(COMMA));
	}

	Token paren = consume(RIGHT_PAREN,
						  "Expect ')' after arguments.");

	return new Expr.Call(callee, paren, arguments);
  }
*/
wcParseExpression weec::parse::wcExpressionParser::ParseExpression_CallArguments(wcParseExpression Callee)
{
	vector<wcParseExpression> Arguments;

	wcParseExpression Buff;
	while (!Tokenizer.IsFinished() && Tokenizer.GetToken().Type != CloseParenthesis)
	{
		if ((Buff = ParseExpression_Expression()).Error.Code != None)
			return Buff;

		Arguments.push_back(Buff);
		if (Tokenizer.GetToken().Type != wcTokenType::Comma)
			break;
		if (!Tokenizer.NextToken())
			return wcParseExpression(wcParserError(wcParserErrorCode::UnexpectedEOF, Tokenizer.GetToken()));
	}

	if (Tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
		return wcParseExpression(wcParserError(wcParserErrorCode::Expression_FunctionCallMissingOpenParenthesis, Tokenizer.GetToken()));
	Tokenizer.NextToken();

	//check we dont exceed max arguments
	if (Arguments.size() > 255)
		return wcParseExpression(wcParserError(wcParserErrorCode::FunctionCall_MaxArgumentsExceeded, Tokenizer.GetToken()));

	return wcParseExpression(Expression_Call, Callee, Arguments);
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Primary()
{
	wcParseExpression Output;

	auto Operand = Tokenizer.GetToken();
	wcParseSymbol Ident;
	switch (Operand.Type)
	{
	case TrueKeyword:
	case FalseKeyword:
	case StringLiteral:
	case FloatLiteral:
	case CharLiteral:
	case IntLiteral:
		Output = wcParseExpression(Expression_Primary, Operand);
		Tokenizer.NextToken();
		return Output;

	case Identifier:
		Ident = wcIdentParser(Tokenizer, SymbolTable).ParseIdent();
		if (!SymbolTable.Exists(Ident.FullIdent))
		{
			Output.Error = wcParserError(wcParserErrorCode::UndeclaredIdent, Tokenizer.GetToken());
			return Output;
		}
		Output = wcParseExpression(Expression_Primary, Operand);
		Tokenizer.NextToken();
		return Output;

	case OpenParenthesis:
		//consume opening parenthesis
		if (Tokenizer.IsFinished() || !Tokenizer.NextToken())
		{
			//end of file before last paren
			Output.Error = wcParserError(wcParserErrorCode::UnexpectedEOF, Tokenizer.GetToken());
			return Output;
		}

		Output = ParseExpression_SubExpression();
		if (Output.Error.Code != None)
			return Output;

		//consume closing parenthesis
		if (Tokenizer.IsFinished() || Tokenizer.GetToken().Type != CloseParenthesis)
		{
			//didnt find one, madness
			Output.Error = wcParserError(wcParserErrorCode::Expression_MissingClosingParenthesis, Tokenizer.GetToken());
			return Output;
		}
		Tokenizer.NextToken();
		return Output;

	default:
		//error
		Output.Error = wcParserError(wcParserErrorCode::Expression_UnexpectedToken, Tokenizer.GetToken());
		return Output;
		break;
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

void weec::parse::wcParseOutput::AddAsChild(wcParseOutput Child, bool PointToChild)
{
	if (Child.AST.size() > 1)
		if (PointToChild)
			NodeIndex = AST.append_child(NodeIndex, Child.GetSubHead());
		else
			AST.append_child(NodeIndex, Child.GetSubHead());

	Error = Child.Error;
}

void weec::parse::wcParseOutput::AddAsChild(wcParseNode Node, bool PointToChild)
{
	if (PointToChild)
		NodeIndex = AST.append_child(NodeIndex, Node);
	else
		AST.append_child(NodeIndex, Node);
}

tree<wcParseNode>::pre_order_iterator weec::parse::wcParseOutput::GetSubHead()
{
	auto HeadPtr = AST.begin(), t = AST.begin();

	//search for a head pointer
	while (t != AST.end())
	{
		if (t->Type == Head)
		{
			HeadPtr = t;
			break;
		}
		++t;
	}

	if (HeadPtr->Type != Head)
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

	return *this;
}

wcParseOutput weec::parse::wcParseOutput::operator=(wcParseOutput other)
{
	AST = other.AST;
	Error = other.Error;
	SymbolTable = other.SymbolTable;

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
	case None:
		return "Head";
	case FuckKnows:
		return "FuckKnows";
	case UnexpectedToken:
		return "UnexpectedToken";
	case If_MissingClosingParenthesis:
		return "If_MissingClosingParenthesis";
	case MissingClosingBrace:
		return "MissingClosingBrace";
	case InvalidType:
		return "InvalidType";
	case IdentRedeclaration:
		return "IdentRedeclaration";
	case UndeclaredIdent:
		return "UndeclaredIdent";
	case UnexpectedEOF:
		return "UnexpectedEOF";
	case Expression_Empty:
		return "Expression_Empty";
	case Expression_UnexpectedToken:
		return "Expression_UnexpectedToken";
	case Expression_MissingClosingParenthesis:
		return "Expression_MissingClosingParenthesis";
	case Expression_UnexpectedEOF:
		return "Expression_UnexpectedEOF";
	default:
		return "";
	}
}


std::string weec::parse::to_string(wcParseNodeType Type)
{
	switch (Type)
	{
	case wcParseNodeType::Head:
		return "Head";
	case wcParseNodeType::Empty:
		return "Empty";
	case wcParseNodeType::WhileStatement:
		return "WhileStatement";
	case wcParseNodeType::ReturnStatement:
		return "ReturnStatement";
	case wcParseNodeType::Return_Expression:
		return "Return_Expression";
	case wcParseNodeType::IfStatement:
		return "IfStatement";
	case wcParseNodeType::If_Expression:
		return "If_Expression";
	case wcParseNodeType::If_TrueBlock:
		return "If_TrueBlock";
	case wcParseNodeType::If_ElseBlock:
		return "If_ElseBlock";
	case wcParseNodeType::Declaration:
		return "Declaration";
	case wcParseNodeType::Declaration_Ident:
		return "Declaration_Ident";
	case wcParseNodeType::Declaration_Type:
		return "Declaration_Type";
	case wcParseNodeType::DeclarationArguments:
		return "DeclarationArguments";
	case wcParseNodeType::DeclarationArgument:
		return "DeclarationArgument";
	case wcParseNodeType::DeclarationArgument_Type:
		return "DeclarationArgument_Type";
	case wcParseNodeType::DeclarationArgument_Ident:
		return "DeclarationArgument_Ident";
	case wcParseNodeType::DeclaratationBody:
		return "DeclaratationBody";
	case wcParseNodeType::Statement:
		return "Statement";
	case wcParseNodeType::Block:
		return "Block";
	case wcParseNodeType::Variable:
		return "Variable";
	case wcParseNodeType::Type:
		return "Type";
	case wcParseNodeType::Expression:
		return "Expression";
	case wcParseNodeType::Expression_Assignment:
		return "Expression_Assignment";
	case wcParseNodeType::Expression_Comparison:
		return "Expression_Comparison";
	case wcParseNodeType::Expression_Equality:
		return "Expression_Equality";
	case wcParseNodeType::Expression_Factor:
		return "Expression_Factor";
	case wcParseNodeType::Expression_LogicAnd:
		return "Expression_LogicAnd";
	case wcParseNodeType::Expression_LogicOr:
		return "Expression_LogicOr";
	case wcParseNodeType::Expression_Primary:
		return "Expression_Primary";
	case wcParseNodeType::Expression_Term:
		return "Expression_Term";
	case wcParseNodeType::Expression_Call:
		return "Expression_Call";
	case wcParseNodeType::Expression_Unary:
		return "Expression_Unary";
	case wcParseNodeType::Expression_Operator:
		return "Expression_Operator";
	default:
		return "";
	}
}

wcParseNodeType wcTokenTypeToParseNodeType(wcTokenType Type)
{
	switch (Type)
	{
	case wcTokenType::Null:
		return wcParseNodeType::Head;

	default:
		return wcParseNodeType::Head;
	}
}

weec::parse::wcParseSymbolTable::wcParseSymbolTable() : NullSymbol(wcParseSymbolType::Null, wcFullIdent("", ""), wcToken())
{
	//global scope
	Add(wcParseSymbolType::Type, wcFullIdent(ParserConsts.Parser_GlobalScopeIdent, ParserConsts.Parser_GlobalScopeIdent), wcToken(), true);

	//built-in types
	Add(wcParseSymbolType::Type, wcFullIdent("void", ParserConsts.Parser_GlobalScopeIdent), wcToken());
	Add(wcParseSymbolType::Type, wcFullIdent("int", ParserConsts.Parser_GlobalScopeIdent), wcToken());
	Add(wcParseSymbolType::Type, wcFullIdent("uint", ParserConsts.Parser_GlobalScopeIdent), wcToken());
	Add(wcParseSymbolType::Type, wcFullIdent("double", ParserConsts.Parser_GlobalScopeIdent), wcToken());
	Add(wcParseSymbolType::Type, wcFullIdent("float", ParserConsts.Parser_GlobalScopeIdent), wcToken());
	Add(wcParseSymbolType::Type, wcFullIdent("bool", ParserConsts.Parser_GlobalScopeIdent), wcToken());
	Add(wcParseSymbolType::Type, wcFullIdent("string", ParserConsts.Parser_GlobalScopeIdent), wcToken());
}

wcParseSymbolTable& weec::parse::wcParseSymbolTable::operator=(const wcParseSymbolTable& Other)
{
	Container = Other.Container;
	this->CurrentScope = Other.CurrentScope;
	return *this;
}

bool weec::parse::wcParseSymbolTable::Add(wcParseSymbolType Type, wcIdent Ident, lex::wcToken IdentToken)
{
	wcFullIdent FullIdent = wcFullIdent(Ident, wcScope(GetCurrentScope().FullIdent.to_string()));
	wcParseSymbol Symbol = wcParseSymbol(Type, FullIdent, IdentToken);

	if (Exists(FullIdent))
		return false;

	Container.insert(make_pair(FullIdent.to_string(), Symbol));

	SetScope(FullIdent);

	return true;
}


bool weec::parse::wcParseSymbolTable::Add(wcParseFunctionSignature Sig, bool SetScopeToThisSymbol)
{
	if (!Exists(Sig.to_string()))
		return false;

	FunctionContainer.insert(make_pair(Sig.to_string(), Sig));
	ShortFunctionNames.push_back(Sig.to_string_no_arguments());

	if (SetScopeToThisSymbol)
		SetScope(wcFullIdent(Sig.to_string()));

	return true;
}

bool weec::parse::wcParseSymbolTable::Add(wcParseSymbol Sym, bool SetScopeToThisSymbol)
{

	if (Exists(Sym.FullIdent))
		return false;

	Container.insert(make_pair(Sym.FullIdent.to_string(), Sym));

	if(SetScopeToThisSymbol)
		SetScope(Sym.FullIdent);

	return true;
}


bool weec::parse::wcParseSymbolTable::Add(wcParseSymbolType Type, wcFullIdent FullIdent, wcToken IdentToken, bool PointScopeToThis)
{
	wcParseSymbol Symbol = wcParseSymbol(Type, FullIdent, IdentToken);

	if (Exists(FullIdent))
		return false;

	Container.insert(make_pair(FullIdent.to_string(), Symbol));

	if (PointScopeToThis)
		SetScope(FullIdent);

	return true;
}

bool weec::parse::wcParseSymbolTable::Exists(wcFullIdent FullIdent) const
{
	if (FullIdent.to_string().find("@"))
		if (Container.find(FullIdent.to_string().substr(0, FullIdent.to_string().find_first_of("@"))) != Container.end())
			return true;	

	//check function names
	for(auto ShortFunctionName : ShortFunctionNames)
		if(ShortFunctionName == FullIdent.to_string())
			return true;

	return Container.find(FullIdent.to_string()) != Container.end() ? true : false;
}

wcParseSymbol weec::parse::wcParseSymbolTable::Get(wcFullIdent FullIdent) const
{
	if (Container.find(FullIdent.to_string()) != Container.end())
		return Container.at(FullIdent.to_string());
	else
		return NullSymbol;
}

bool weec::parse::wcParseSymbolTable::SetScope(wcFullIdent FullIdent)
{
	if (!Exists(FullIdent))
		return false;

	CurrentScope = Get(FullIdent);

	return true;
}

bool weec::parse::wcParseSymbolTable::SetScope(wcParseSymbol& Symbol)
{
	if (!Exists(Symbol.FullIdent))
		return false;

	CurrentScope = Get(Symbol.FullIdent);

	return true;
}

wcParseSymbol  weec::parse::wcParseSymbolTable::GetCurrentScope() const
{
	return CurrentScope;
}

wcParseSymbol weec::parse::wcParseSymbolTable::ClassifyIdent(lex::wcToken Ident)
{
	wcFullIdent FullIdent;

	//check if ident is fully qualified
	if (wcTokenTypeAlizer().IsIdentQualified(Ident.StringToken.Data))
		FullIdent = wcFullIdent(Ident.StringToken.Data);
	else
		FullIdent = wcFullIdent(wcIdent(Ident.StringToken.Data), wcScope(GetCurrentScope().FullIdent.to_string()));

	if (Exists(FullIdent))
	{

	}

	return wcParseSymbol(wcParseSymbolType::Variable, FullIdent ,Ident);
}

weec::parse::wcExpressionParser::wcExpressionParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable) : Tokenizer(_Tokenizer), SymbolTable(_SymbolTable)
{

}


weec::parse::wcScope::wcScope()
{
}

weec::parse::wcScope::wcScope(const wcScope& Other)
{
	FullIdent = Other.FullIdent;
}

weec::parse::wcScope::wcScope(std::string _Data)
{
	FullIdent = _Data;
}

weec::parse::wcIdent::wcIdent()
{
}

weec::parse::wcIdent::wcIdent(const wcIdent& Other) : Ident(Other.Ident)
{
	Ident = Other.Ident;
}

weec::parse::wcIdent::wcIdent(std::string _Data)
{
	Ident = _Data;
}

weec::parse::wcFullIdent::wcFullIdent(std::string FullIdent)
{

	//$global::functionNameWithParams@$global::a,$global::b

	if (FullIdent.find("::") == std::string::npos)
	{
		//not qualified as expected, global scope
		ShortIdent = wcIdent(FullIdent);
		Scope = wcScope(ParserConsts.Parser_GlobalScopeIdent);
		return;
	}

	//argument specifier, must be a function
	string IdentWithoutArguments = FullIdent, ShortIdentWithArguments = "";
	if (IdentWithoutArguments.find_first_of("@") != std::string::npos)
	{
		IdentWithoutArguments = IdentWithoutArguments.substr(0, IdentWithoutArguments.find_first_of("@"));	//$global::functionNameWithParams
		ShortIdentWithArguments = FullIdent.substr(FullIdent.rfind("::", FullIdent.find_first_of("@")) + 2);
	}
	else
		ShortIdentWithArguments = FullIdent.substr(FullIdent.find_last_of("::")+1);


	//get scope
	Scope = wcScope(FullIdent.substr(0, IdentWithoutArguments.find_last_of("::") - 1));			//$global
	ShortIdent = wcIdent(ShortIdentWithArguments);
}

weec::parse::wcFullIdent::wcFullIdent(std::string _Ident, std::string _Scope)
{
	ShortIdent = wcIdent(_Ident);
	Scope = wcScope(_Scope);
}

weec::parse::wcFullIdent::wcFullIdent(wcIdent _Ident, wcScope _Scope)
{
	ShortIdent = wcIdent(_Ident);
	Scope = wcScope(_Scope);
}

weec::parse::wcFullIdent::wcFullIdent()
{
}

weec::parse::wcFullIdent::wcFullIdent(const wcFullIdent& Other)
{
	ShortIdent = wcIdent(Other.ShortIdent);
	Scope = wcScope(Other.Scope);
}

std::string weec::parse::wcFullIdent::to_string() const
{
	if (ShortIdent.to_string() == ParserConsts.Parser_GlobalScopeIdent)
		return ShortIdent.to_string();		//stops the global scope FullIdent being global::global
	return Scope.to_string() + ParserConsts.Parser_ScopeDelimiter.c_str() + ShortIdent.to_string();
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
}

weec::parse::wcParseSymbol::wcParseSymbol(wcParseSymbolType _Type, wcFullIdent _FullIdent, wcToken _Token) : FullIdent(_FullIdent)
{
	FullIdent = wcFullIdent(_FullIdent);
	Type = _Type;
	IdentToken = _Token;
}

weec::parse::wcParseSymbol::wcParseSymbol(wcParseSymbolType _Type, wcFullIdent _FullIdent, wcFullIdent _DataType, wcToken _Token) : FullIdent(_FullIdent), DataType(_DataType)
{
	FullIdent = wcFullIdent(_FullIdent);
	Type = _Type;
	DataType = _DataType;
	IdentToken = _Token;
	Const = false;
	Registered = false;

}

std::string weec::parse::wcParseFunctionSignature::to_string()
{
	return Ident.FullIdent.to_string();
}

std::string weec::parse::wcParseFunctionSignature::to_string_no_arguments()
{
	return to_string().substr(0, to_string().find_first_of("@"));
}
