#ifndef WC_PARSE
#define WC_PARSE
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include "tree.hh"
#include "lex.h"

namespace weec
{
	namespace parse
	{
		enum class wcParseNodeType
		{
			Empty, Head,

			Statement, Block,

			WhileStatement,

			IfStatement, If_Expression, If_TrueBlock, If_ElseBlock,

			ReturnStatement, Return_Expression,

			Declaration, Declaration_Type, Declaration_Ident, DeclarationArguments, DeclarationArgument, DeclarationArgument_Type, DeclarationArgument_Ident, DeclaratationBody,

			Expression, Expression_Equality, Expression_Assignment, Expression_LogicOr, Expression_LogicAnd, Expression_Comparison, Expression_Term,
			Expression_Factor, Expression_Call, Expression_Unary, Expression_Primary, Expression_Operator,

			Type, Variable, Array, Function
		};
		std::string to_string(wcParseNodeType);
		wcParseNodeType wcTokenTypeToParseNodeType(lex::wcTokenType);

		enum class wcParserErrorCode
		{
			None, FuckKnows,

			UnexpectedToken,

			InvalidType,

			If_MissingClosingParenthesis, MissingClosingBrace,

			While_MissingClosingParenthesis,

			IdentRedeclaration, UndeclaredIdent, DeclarationsProhibited,

			UnexpectedEOF,

			Expression_Empty,
			Expression_UnexpectedToken,
			Expression_MissingClosingParenthesis,
			Expression_UnexpectedEOF,
			Expression_NotAnLValue,
			Expression_FunctionCallMissingOpenParenthesis,

			FunctionCall_MaxArgumentsExceeded
		};
		std::string to_string(wcParserErrorCode);

		struct wcParserError
		{
			wcParserError() { Code = wcParserErrorCode::None; }
			wcParserError(wcParserErrorCode _Code, lex::wcToken _Token) { Code = _Code; Token = _Token; }
			wcParserErrorCode Code;
			lex::wcToken Token;
		};
		
		enum class wcParseSymbolType
		{
			Null, Type, Variable, Function, Namespace
		};

		struct wcIdentifier
		{
			wcIdentifier();
			wcIdentifier(const wcIdentifier&), wcIdentifier(std::string);

			bool operator==(const wcIdentifier& p) const
			{
				return to_string() == p.to_string();
			}

			std::string to_string() const { return Identifier.c_str(); }

		private:
			std::string Identifier;
		};

		struct wcIdentifierScope
		{
			wcIdentifierScope();
			wcIdentifierScope(const wcIdentifierScope&), wcIdentifierScope(std::string);

			std::string to_string() const { return FullIdent.to_string(); }
			wcIdentifier FullIdent;
		};

		struct wcParseScope
		{
			wcParseScope();

			std::string Name;
			std::vector<wcFullIdentifier> Symbols
		};

		class wcParseExpression; struct wcParseSymbol;

		struct wcFullIdentifier
		{
			wcFullIdentifier();
			wcFullIdentifier(const wcFullIdentifier&);
			wcFullIdentifier(wcIdentifier, wcIdentifierScope);
			wcFullIdentifier(std::string);
			wcFullIdentifier(std::string, std::vector<wcParseExpression>);
			wcFullIdentifier(std::string, std::vector<wcParseSymbol>);
			wcFullIdentifier(std::string, std::string);

			bool operator==(const wcFullIdentifier& p) const
			{
				return to_string() == p.to_string();
			}

			wcFullIdentifier& operator=(const wcFullIdentifier& Other)
			{
				ShortIdent = Other.ShortIdent;
				Scope = Other.Scope;
				return *this;
			}

			std::string to_string() const;

			wcIdentifier ShortIdent;
			wcIdentifierScope Scope;
		};

		struct wcArgumentPrototype
		{
			wcFullIdentifier Type, Identifer;
		};

		struct wcFunctionIdentifier
		{
			wcFunctionIdentifier() {}
			wcFunctionIdentifier(std::string);
			wcFunctionIdentifier(wcFullIdentifier _Identifier, std::vector<wcArgumentPrototype> Arguments);
			wcFunctionIdentifier(wcFullIdentifier _Identifier, std::vector<wcFullIdentifier> ArgumentTypes);
			wcFunctionIdentifier(wcFullIdentifier, std::vector<wcParseSymbol> Arguments);

			bool operator==(const wcFunctionIdentifier& p) const
			{
				return to_string() == p.to_string();
			}

			bool operator==(const wcFullIdentifier& p) const
			{
				return to_string_no_arguments() == p.to_string();
			}

			wcFunctionIdentifier& operator=(const wcFunctionIdentifier& Other)
			{
				Identifier = Other.Identifier;
				IdentifierWithoutArguments = Other.IdentifierWithoutArguments;
				ArgumentTypes = Other.ArgumentTypes;
				return *this;
			}

			std::string to_string() const
			{
				return Identifier.to_string();
			}

			std::string to_string_no_arguments() const
			{
				return IdentifierWithoutArguments.to_string();
			}

			unsigned int ArgumentCount()
			{
				return ArgumentTypes.size();
			}

			wcFullIdentifier Identifier, IdentifierWithoutArguments;
			std::vector<wcFullIdentifier> ArgumentTypes;
		};

		class wcParseNode
		{
		public:
			wcParseNode();
			wcParseNode(const wcParseNode&);
			wcParseNode(wcParseNodeType _Type);
			wcParseNode(wcParseNodeType _Type, lex::wcToken _Token);
			wcParseNode(wcParseNodeType _Type, lex::wcToken _Token, wcFullIdentifier Identifier);
			wcParseNode(lex::wcToken _Token);

			bool operator==(const wcParseNode&), operator==(wcParseNode);
			wcParseNode& operator=(const wcParseNode&);

			lex::wcToken Token;
			std::string Memo;
			wcParseNodeType Type;
		};

		struct wcParseSymbol
		{
			wcParseSymbol();
			wcParseSymbol(const wcParseSymbol&);
			wcParseSymbol(wcParseSymbolType _Type, wcFullIdentifier FullIdent, lex::wcToken IdentToken);
			wcParseSymbol(wcParseSymbolType _Type, wcFullIdentifier FullIdent, wcFullIdentifier DataType, lex::wcToken IdentToken);
			wcParseSymbol(wcArgumentPrototype ArgDeclaration, lex::wcToken IdentToken);

			wcParseSymbolType Type;
			wcFullIdentifier DataType;
			wcFullIdentifier FullIdent;

			lex::wcToken IdentToken;
			unsigned int Arguments;
			bool Registered, Const, HasOverloads;
		};

		struct wcParseFunctionSignature
		{
			std::string to_string();
			std::string to_string_no_arguments();

			wcFullIdentifier DataType;
			lex::wcToken IdentToken;
			wcFunctionIdentifier FuncFullIdent;
		};

		struct wcParseStackFrame
		{
			wcParseStackFrame();
			wcParseStackFrame(wcFullIdentifier);

			std::string Name;
			wcFunctionIdentifier FuncIdent;
			tree<parse::wcParseNode>::iterator ReturnAddress;
			std::vector<wcFullIdentifier> Symbols;
			//std::vector<std::any> Arguments;
		};

		class wcParseSymbolTable
		{
			std::unordered_map<std::string, wcParseSymbol> Container;
			std::unordered_map<std::string, wcParseFunctionSignature> FunctionContainer;

			wcParseSymbol CurrentScope;

		public:
			std::stack<wcParseStackFrame> StackFrames;
			wcParseSymbolTable();

			wcParseSymbolTable& operator=(const wcParseSymbolTable&);

			bool 
				Add(wcParseSymbolType, wcFullIdentifier Ident, lex::wcToken IdentToken, bool SetScopeToThisSymbol = false),
				Add(wcParseSymbol Sym, bool SetScopeToThisSymbol = false),
				Add(wcParseFunctionSignature Sig, bool SetScopeToThisSymbol = false),

				Exists(wcFullIdentifier FullIdent) const;

			wcParseSymbol Get(wcFullIdentifier FullIdent) const;

			bool SetScope(wcFullIdentifier FullIdent), SetScope(wcParseSymbol&);
			wcParseSymbol GetCurrentScope() const;

			const  wcParseSymbol NullSymbol;
			wcParseSymbol ClassifyIdent(lex::wcToken);
		};

		class wcParseOutput
		{
			tree<wcParseNode>::pre_order_iterator NodeIndex, UnofficialHead;

		public:
			wcParseOutput();
			wcParseOutput(wcParserError);
			wcParseOutput(wcParseNode, bool PointToChild = false);

			wcParseOutput& AddAsChild(wcParseOutput, bool PointToChild = false);
			wcParseOutput& AddAsChild(wcParseNode, bool PointToChild = false);

			void Up() { NodeIndex = AST.parent(NodeIndex); }

			tree<wcParseNode>::pre_order_iterator GetHead() { return UnofficialHead; }
			tree<wcParseNode>::pre_order_iterator GetSubHead();

			wcParseOutput operator+(wcParseOutput), operator+=(wcParseOutput), operator=(wcParseOutput);

			tree<wcParseNode> AST;
			wcParseSymbolTable SymbolTable;
			wcParserError Error;
		};

		class wcIdentifierResolver
		{
			std::stack<wcParseStackFrame>& StackFrames;
		public:
			wcIdentifierResolver(std::stack<wcParseStackFrame>& _StackFrames) :
				StackFrames(_StackFrames) {}

			bool Resolve(wcIdentifier In, wcFullIdentifier& Out);
		};

		enum class wcParseExpressionType
		{
			Literal, Variable, Unary, Call, Binary, Logical, Grouping, Assignment
		};

		class wcParseExpression
		{
			tree<wcParseNode>::pre_order_iterator GetExpressionNodeBegin(), GetExpressionNodeEnd(), GetExpressionRootNodeBegin(), GetExpressionRootNodeEnd();

		public:
			wcFullIdentifier DataType;
			wcParseExpressionType Type;
			wcParserError Error;
			std::vector<lex::wcToken> Tokens;
			tree<wcParseNode> AST;

			wcParseExpression();
			wcParseExpression(wcParserError _Error);
			wcParseExpression(wcParseNodeType HeadType, wcParseExpression LeftHand, lex::wcToken Operator, wcParseExpression RightHand);		//binary / logical / assignment
			wcParseExpression(wcParseNodeType HeadType, lex::wcToken Operator, wcParseExpression RightHand);									//unary
			wcParseExpression(wcParseNodeType HeadType, wcFullIdentifier FullIdent, wcParseExpression Callee, std::vector<wcParseExpression> Arguments);					//call
			wcParseExpression(wcParseNodeType HeadType, lex::wcToken OperatorOrLiteral);														//literal/operator
			wcParseExpression(wcParseNodeType HeadType, lex::wcToken OperatorOrLiteral, wcFullIdentifier DataType);														//variable
			wcParseExpression(wcParseNodeType HeadType, wcFullIdentifier FullIdent, wcFullIdentifier DataType, lex::wcToken OperatorOrLiteral);									//variable
			//wcParseExpression(wcParseExpression& OtherExpression);																			//grouping

			wcParseExpression operator=(wcParseExpression);

			bool IsErrored();
		};

		class wcSubParser
		{
		protected:
			lex::wcTokenizer& Tokenizer;
			wcParseSymbolTable& SymbolTable;

		public:
			wcSubParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable)
				: Tokenizer(_Tokenizer), SymbolTable(_SymbolTable) {}

			bool ExpectToken(lex::wcTokenType);
			bool ExpectNextToken(lex::wcTokenType);
		};

		class wcExpressionParser : wcSubParser
		{
			wcParserError Error;

			wcParseExpression ParseExpression_Expression(), ParseExpression_SubExpression(),
				ParseExpression_Equality(), ParseExpression_Assignment(), ParseExpression_LogicOr(),
				ParseExpression_LogicAnd(), ParseExpression_Comparison(),
				ParseExpression_Term(), ParseExpression_Factor(),
				ParseExpression_Unary(), ParseExpression_Call(), ParseExpression_CallArguments(wcParseExpression Callee), ParseExpression_Primary();
		public:
			wcExpressionParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable)
				: wcSubParser(_Tokenizer, _SymbolTable) {}

			wcParseOutput ParseExpression(), ParseExpression(wcParseSymbol);
		};

		class wcIdentParser : wcSubParser
		{
		public:
			wcIdentParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable);

			wcParseSymbol ParseIdent();
		};

		class wcDeclarationParser : wcSubParser
		{

			wcParseOutput ParseType(wcFullIdentifier& DeclarationType), ParseIdent(wcIdentifier& DeclarationIdent, lex::wcToken& IdentAsSeen),
				ParseArguments(wcFullIdentifier DeclarationIdent, wcFullIdentifier DeclarationType, lex::wcToken IdentToken, wcFunctionIdentifier& ReturnTrueFunctionIdent),
				ParseArgument(wcFunctionIdentifier& ReturnTrueFunctionIdent);

		public:
			wcDeclarationParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable)
				: wcSubParser(_Tokenizer, _SymbolTable) {}

			wcParseOutput Parse();
		};

		class wcStatementParser : wcSubParser
		{

		public:
			wcStatementParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable)
				: wcSubParser(_Tokenizer, _SymbolTable) {}

			wcParseOutput Parse(bool AllowDeclarations);
		};

		class wcBlockParser : wcSubParser
		{

		public:
			wcBlockParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable)
				: wcSubParser(_Tokenizer, _SymbolTable) {}

			wcParseOutput Parse(bool AllowDeclarations);
		};

		class wcIfParser : wcSubParser
		{
		public:
			wcIfParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable)
				: wcSubParser(_Tokenizer, _SymbolTable) {}

			wcParseOutput Parse();
		};

		class wcSemiColonParser : wcSubParser
		{
		public:
			wcSemiColonParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable)
				: wcSubParser(_Tokenizer, _SymbolTable) {}

			wcParseOutput Parse();
		};

		class wcReturnParser : wcSubParser
		{
		public:
			wcReturnParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable)
				: wcSubParser(_Tokenizer, _SymbolTable) {}

			wcParseOutput Parse();
		};

		class wcWhileParser : wcSubParser
		{
		public:
			wcWhileParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable)
				: wcSubParser(_Tokenizer, _SymbolTable) {}

			wcParseOutput Parse();
		};

		class wcParser
		{
			lex::wcTokenizer& Tokenizer;
			wcParseSymbolTable SymbolTable;

		public:
			wcParser(lex::wcTokenizer& _Tokenizer);

			wcParseOutput Parse();
		};

	}

}


#endif
