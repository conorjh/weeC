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
	std::string Parser_GlobalScopeIdent = "$global",
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
		 
		Output.AddAsChild(ParseStatement());
	}

	Output.SymbolTable = SymbolTable;

	return Output;
}

wcParseOutput weec::parse::wcParser::ParseDeclaration()
{
	wcParseOutput Output;

	Output.AddAsChild(wcParseNode(wcParseNodeType::Declaration), true);

	//Declaration_Type
	auto TypeToken = Tokenizer.GetToken();
	if (!TypeToken.IsBuiltinType() && TypeToken.Type != Identifier)
		return wcParseOutput(wcParserError(InvalidType, TypeToken));		//error - expected built in or user type
	Output.AddAsChild(wcParseNode(wcParseNodeType::Declaration_Type, TypeToken));

	//Declaration_Ident
	if (!Tokenizer.NextToken(Identifier))
		return wcParseOutput(wcParserError(UnexpectedToken, TypeToken));
	auto IdentToken = Tokenizer.GetToken();
	auto FullIdent = wcFullIdent(IdentToken.StringToken.Data, SymbolTable.GetCurrentScope().FullIdent.Scope.Get());
	if (SymbolTable.Exists(FullIdent))
		return wcParseOutput(wcParserError(IdentRedeclaration, IdentToken));
	Output.AddAsChild(wcParseNode(wcParseNodeType::Declaration_Ident, IdentToken));

	//add to symbol table
	auto SymbolType = SymbolTable.ClassifyIdent(IdentToken).Type;
	SymbolTable.Add(SymbolType, FullIdent);

	//semi colon/optional assignment
	if (!Tokenizer.NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));
	if (Tokenizer.GetToken().Type == wcTokenType::SemiColon)
	{
		Tokenizer.NextToken();
		return Output;
	}
	else if (Tokenizer.GetToken().Type != wcTokenType::AssignOperator)
		return Output;
	Tokenizer.NextToken();

	//optional assignment expression;
	auto InitExpression = wcExpressionParser(Tokenizer, SymbolTable).ParseExpression();
	if (InitExpression.Error.Code != None)
		return InitExpression;
	Output.AddAsChild(InitExpression);

	//semi colon
	if (Tokenizer.GetToken().Type != wcTokenType::SemiColon)
		return wcParseOutput(wcParserError(UnexpectedToken, Tokenizer.GetToken()));

	Tokenizer.NextToken();

	return Output;
}

wcParseOutput weec::parse::wcParser::ParseDeclaration(wcParseSymbol)
{
	
	return wcParseOutput();
}

wcParseOutput weec::parse::wcParser::ParseDeclaration_Variable()
{
	return wcParseOutput();
}

wcParseOutput weec::parse::wcParser::ParseDeclaration_Function()
{
	return wcParseOutput();
}

wcParseOutput weec::parse::wcParser::ParseDeclaration_Variable(wcParseSymbol)
{
	return wcParseOutput();
}

weec::parse::wcIdentParser::wcIdentParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable) : Tokenizer(_Tokenizer), SymbolTable(_SymbolTable)
{

}

wcParseSymbol weec::parse::wcIdentParser::ParseIdent()
{
	auto Ident1 = Tokenizer.GetToken();

	wcFullIdent FullIdent(wcIdent(Ident1.StringToken.Data), wcScope(SymbolTable.GetCurrentScope().FullIdent.Get()));

	if (SymbolTable.Exists(FullIdent))
	{

	}
	wcParseSymbol Symbol(SymbolTable.ClassifyIdent(Ident1).Type, FullIdent);

	Symbol.FullIdent = FullIdent;
	return Symbol;
}

wcParseOutput weec::parse::wcParser::ParseStatement()
{
	wcParseOutput Output;
	auto ThisToken = Tokenizer.GetToken();

	Output.AddAsChild(wcParseNode(wcParseNodeType::Statement), true);

	wcParseOutput Buffer;
	switch (ThisToken.Type)
	{
	WC_SWITCHCASE_TOKENS_LITERAL
	WC_SWITCHCASE_TOKENS_OPERATORS_ALL
	case OpenParenthesis:
		Buffer = wcExpressionParser(Tokenizer, SymbolTable).ParseExpression();
		break;

	WC_SWITCHCASE_TOKENS_BUILTIN_TYPES
		Buffer = ParseDeclaration();
		break;

	case wcTokenType::IfKeyword:
		Buffer = ParseIf();
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
			Buffer = ParseDeclaration(Ident);
			break;

		case wcParseSymbolType::Variable:
		case wcParseSymbolType::Function:
			Buffer = wcExpressionParser(Tokenizer, SymbolTable).ParseExpression(Ident);
			if (Buffer.Error.Code != None)
				return Buffer;

			if (Tokenizer.GetToken().Type != SemiColon)
			{
				//error
				Output.Error = wcParserError(wcParserErrorCode::UnexpectedToken, Tokenizer.GetToken());
				return Output;
			}
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

wcParseOutput weec::parse::wcParser::ParseBlock()
{
	wcParseOutput Output;

	if(Tokenizer.GetToken().Type != OpenBrace)
		return wcParseOutput(wcParserError(UnexpectedToken, Tokenizer.GetToken()));

	if(!Tokenizer.NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));

	Output.AddAsChild(wcParseNode(wcParseNodeType::Block), true);

	while (Tokenizer.GetToken().Type != CloseBrace)
	{
		Output.AddAsChild(ParseStatement());

		if (Tokenizer.IsErrored() || Output.Error.Code != None)
			return Output;
	}

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
	auto IfExpression = wcExpressionParser(Tokenizer, SymbolTable).ParseExpression();
	if (IfExpression.Error.Code != None)
		return IfExpression;
	Output.AddAsChild(IfExpression);

	// )
	if (Tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
		return wcParseOutput(wcParserError(If_MissingClosingParenthesis, Tokenizer.GetToken()));
	if (!Tokenizer.NextToken())
		return wcParseOutput(wcParserError(UnexpectedEOF, Tokenizer.GetToken()));

	//block/statement
	Output.AddAsChild(ParseBlock());

	//optional else keyword
	if (!Tokenizer.NextToken())
		return Output;
	if (Tokenizer.GetToken().Type == wcTokenType::SemiColon)
	{
		Tokenizer.NextToken();
		return Output;
	}
	else if (Tokenizer.GetToken().Type != wcTokenType::AssignOperator)
		return Output;
	Tokenizer.NextToken();

	//optional else block/statement
	auto InitExpression = wcExpressionParser(Tokenizer, SymbolTable).ParseExpression();
	if (InitExpression.Error.Code != None)
		return InitExpression;
	Output.AddAsChild(InitExpression);

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
}

weec::parse::wcParseExpression::wcParseExpression(wcParseNodeType HeadType, lex::wcToken OperatorOrLiteral)
{
	auto ExpNode = AST.insert(AST.begin(), *new wcParseNode(Expression));

	AST.append_child(ExpNode, *new wcParseNode(HeadType, OperatorOrLiteral));

	Tokens.push_back(OperatorOrLiteral);
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
	if(LeftHand.Error.Code == None && RightHand.Error.Code != None)
		Error = RightHand.Error;
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
}


wcParseOutput weec::parse::wcExpressionParser::ParseExpression()
{
	if (Tokenizer.IsFinished())
		return wcParserError(wcParserErrorCode::Experession_Empty, Tokenizer.GetToken());

	auto eExpression = ParseExpression_Expression();
	wcParseOutput Output;
	Output.AST = eExpression.AST;
	Output.Error = eExpression.Error;
	return Output;
}
wcParseOutput weec::parse::wcExpressionParser::ParseExpression(wcParseSymbol Symbol)
{

	if (Tokenizer.IsFinished())
		return wcParserError(wcParserErrorCode::Experession_Empty, Tokenizer.GetToken());

	wcParseOutput Output;
	auto Expression = ParseExpression_Expression();
	if (Expression.Error.Code != wcParserErrorCode::None)
	{
		Output.Error = Expression.Error;
		return Output;
	}

	if (Tokenizer.GetToken().Type != wcTokenType::SemiColon)
	{
		Output.Error = wcParserError(wcParserErrorCode::Experession_UnexpectedToken, Tokenizer.GetToken());
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

	Expression1.Eval();

	return Expression1;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_SubExpression()
{
	auto Expression = ParseExpression_Equality();

	//if(!Tokenizer.IsFinished())

	return Expression;
}

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Equality()
{
	wcParseExpression Output = ParseExpression_Assignment();
	if (Output.Error.Code != None)
		return Output;

	auto Operator = Tokenizer.GetToken();
	while (Operator.Type == NotEqualOperator || Operator.Type == EqualOperator)
	{
		Tokenizer.NextToken();
		auto RightExp = ParseExpression_Assignment();
		Output = wcParseExpression(Expression_Equality, Output, Operator, RightExp);

		Operator = Tokenizer.GetToken();
	}

	return Output;
}

/*

 private Expr assignment() {
	Expr expr = equality();

	if (match(EQUAL)) {
	  Token equals = previous();
	  Expr value = assignment();

	  if (expr instanceof Expr.Variable) {
		Token name = ((Expr.Variable)expr).name;
		return new Expr.Assign(name, value);
	  }

	  error(equals, "Invalid assignment target.");
	}

	return expr;

*/

wcParseExpression weec::parse::wcExpressionParser::ParseExpression_Assignment()
{
	wcParseExpression Output = ParseExpression_LogicOr();
	if (Output.Error.Code != None)
		return Output;

	auto Operator = Tokenizer.GetToken();
	while (Operator.Type == AssignOperator)
	{
		Tokenizer.NextToken();
		auto RightExp = ParseExpression_LogicOr();
		Output = wcParseExpression(Expression_Assignment, Output, Operator, RightExp);

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
	wcParseExpression Output = ParseExpression_Comparison();
	if (Output.Error.Code != None)
		return Output;

	auto Operator = Tokenizer.GetToken();
	while (Operator.Type == wcTokenType::LogAndOperator)
	{
		Tokenizer.NextToken();
		auto RightExp = ParseExpression_Comparison();
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
			Output.Error = wcParserError(wcParserErrorCode::Experession_UnexpectedEOF, Tokenizer.GetToken());
			return Output;
		}

		auto RightExp = ParseExpression_Factor();
		//if(RightExp.Error)

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

	return ParseExpression_Primary();
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
			Output.Error = wcParserError(wcParserErrorCode::Experession_MissingClosingParenthesis, Tokenizer.GetToken());
			return Output;
		}
		Tokenizer.NextToken();
		return Output;

	default:
		//error
		Output.Error = wcParserError(wcParserErrorCode::Experession_UnexpectedToken, Tokenizer.GetToken());
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

void weec::parse::wcParseOutput::AddAsChild(wcParseOutput Child, bool PointToChild)
{
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
	auto HeadPtr = AST.begin(),t = AST.begin();

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
/*
None, FuckKnows,

UnexpectedToken,

InvalidType,

IdentRedeclaration, UndeclaredIdent,

UnexpectedEOF,

Experession_Empty,
Experession_UnexpectedToken,
Experession_MissingClosingParenthesis,
Experession_UnexpectedEOF
*/

std::string weec::parse::wcParserErrorCodeToString(wcParserErrorCode Code)
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
	case InvalidType:
		return "InvalidType";
	case IdentRedeclaration:
		return "IdentRedeclaration";
	case UndeclaredIdent:
		return "UndeclaredIdent";
	case UnexpectedEOF:
		return "UnexpectedEOF";
	case Experession_Empty:
		return "Experession_Empty";
	case Experession_UnexpectedToken:
		return "Experession_UnexpectedToken";
	case Experession_MissingClosingParenthesis:
		return "Experession_MissingClosingParenthesis";
	case Experession_UnexpectedEOF:
		return "Experession_UnexpectedEOF";
	default:
		return "";
	}
}


std::string weec::parse::wcParseNodeTypeToString(wcParseNodeType Type)
{
	switch (Type)
	{
	case wcParseNodeType::Head:
		return "Head";
	case wcParseNodeType::Empty:
		return "Empty";
	case wcParseNodeType::IfStatement:
		return "IfStatement";
	case wcParseNodeType::IfExpression:
		return "IfExpression";
	case wcParseNodeType::IfBlock:
		return "IfBlock";
	case wcParseNodeType::ElseBlock:
		return "ElseBlock";
	case wcParseNodeType::Declaration:
		return "Declaration";
	case wcParseNodeType::Declaration_Ident:
		return "Declaration_Ident";
	case wcParseNodeType::Declaration_Type:
		return "Declaration_Type";
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

weec::parse::wcParseSymbolTable::wcParseSymbolTable() : NullSymbol(wcParseSymbolType::Null, wcFullIdent("", ""))
{
	//global scope
	Add(wcParseSymbolType::Type, wcFullIdent(ParserConsts.Parser_GlobalScopeIdent, ParserConsts.Parser_GlobalScopeIdent), true);

	//built-in types
	Add(wcParseSymbolType::Type, wcFullIdent("void", ParserConsts.Parser_GlobalScopeIdent));
	Add(wcParseSymbolType::Type, wcFullIdent("int", ParserConsts.Parser_GlobalScopeIdent));
	Add(wcParseSymbolType::Type, wcFullIdent("uint", ParserConsts.Parser_GlobalScopeIdent));
	Add(wcParseSymbolType::Type, wcFullIdent("float", ParserConsts.Parser_GlobalScopeIdent));
	Add(wcParseSymbolType::Type, wcFullIdent("string", ParserConsts.Parser_GlobalScopeIdent));
}

wcParseSymbolTable& weec::parse::wcParseSymbolTable::operator=(const wcParseSymbolTable& Other)
{
	Container = Other.Container;
	this->CurrentScopeFullIdent = Other.CurrentScopeFullIdent;
	return *this;
}

bool weec::parse::wcParseSymbolTable::Add(wcParseSymbolType Type, wcIdent Ident)
{
	wcFullIdent FullIdent = wcFullIdent(Ident, wcScope(GetCurrentScope().FullIdent.Get()));
	wcParseSymbol Symbol = wcParseSymbol(Type, FullIdent);

	if (Exists(FullIdent))
		return false;

	Container.insert(make_pair(FullIdent.Get(), Symbol));

	SetScope(FullIdent);

	return true;
}

bool weec::parse::wcParseSymbolTable::Add(wcParseSymbolType Type, wcFullIdent FullIdent, bool PointScopeToThis)
{
	wcParseSymbol Symbol = wcParseSymbol(Type, FullIdent);

	if (Exists(FullIdent))
		return false;

	Container.insert(make_pair(FullIdent.Get(), Symbol));

	if (PointScopeToThis)
		SetScope(FullIdent);

	return true;
}

bool weec::parse::wcParseSymbolTable::Exists(wcFullIdent FullIdent) const
{
	if (Container.find(FullIdent.Get()) != Container.end())
		return true;
	else
		return false;
}

wcParseSymbol weec::parse::wcParseSymbolTable::Get(wcFullIdent FullIdent) const
{
	if (Container.find(FullIdent.Get()) != Container.end())
		return Container.at(FullIdent.Get());
	else
		return NullSymbol;
}

bool weec::parse::wcParseSymbolTable::SetScope(wcFullIdent FullIdent)
{
	if (!Exists(FullIdent))
		return false;

	CurrentScopeFullIdent = Get(FullIdent);

	return true;
}

bool weec::parse::wcParseSymbolTable::SetScope(wcParseSymbol& Symbol)
{
	if (!Exists(Symbol.FullIdent))
		return false;
	CurrentScopeFullIdent = Get(Symbol.FullIdent);
	return true;
}

wcParseSymbol  weec::parse::wcParseSymbolTable::GetCurrentScope() const
{
	return CurrentScopeFullIdent;
}

wcParseSymbol weec::parse::wcParseSymbolTable::ClassifyIdent(lex::wcToken Ident)
{
	wcFullIdent FullIdent;

	//check if ident is fully qualified
	if (wcTokenTypeAlizer().IsIdentQualified(Ident.StringToken.Data))
		FullIdent = wcFullIdent(Ident.StringToken.Data);
	else
		FullIdent = wcFullIdent(wcIdent(Ident.StringToken.Data), wcScope(GetCurrentScope().FullIdent.Get()));

	if (Exists(FullIdent))
	{

	}

	return wcParseSymbol(wcParseSymbolType::Variable, FullIdent);
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
	if (FullIdent.find("::") == std::string::npos && FullIdent.find("."))
	{
		//not qualified as expected, global scope
		Ident = wcIdent(FullIdent);
		Scope = wcScope(ParserConsts.Parser_GlobalScopeIdent);
		return;
	}

	//get scope
	Scope = wcScope(FullIdent.substr(FullIdent.find_last_of("::")));

}

weec::parse::wcFullIdent::wcFullIdent(std::string _Ident, std::string _Scope)
{
	Ident = wcIdent(_Ident);
	Scope = wcScope(_Scope);
}

weec::parse::wcFullIdent::wcFullIdent(wcIdent _Ident, wcScope _Scope)
{
	Ident = wcIdent(_Ident);
	Scope = wcScope(_Scope);
}

weec::parse::wcFullIdent::wcFullIdent()
{
}

weec::parse::wcFullIdent::wcFullIdent(const wcFullIdent& Other)
{
	Ident = wcIdent(Other.Ident);
	Scope = wcScope(Other.Scope);
}

std::string weec::parse::wcFullIdent::Get() const
{
	if (Ident.Get() == ParserConsts.Parser_GlobalScopeIdent)
		return Ident.Get();		//stops the global scope FullIdent being global::global
	return Scope.Get() + ParserConsts.Parser_ScopeDelimiter.c_str() + Ident.Get();
}

weec::parse::wcParseSymbol::wcParseSymbol() : FullIdent()
{
	Type = wcParseSymbolType::Null;
}

weec::parse::wcParseSymbol::wcParseSymbol(const wcParseSymbol& Other)
{
	Type = Other.Type;
	FullIdent = Other.FullIdent;
}

weec::parse::wcParseSymbol::wcParseSymbol(wcParseSymbolType _Type, wcFullIdent _FullIdent) : FullIdent(_FullIdent)
{
	FullIdent = wcFullIdent(_FullIdent);
	Type = _Type;
}
