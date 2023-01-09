#ifndef WC_PARSE
#define WC_PARSE
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <string>
#include <vector>
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

		class wcParseNode
		{
		public:
			wcParseNode();
			wcParseNode(const wcParseNode&);
			wcParseNode(wcParseNodeType _Type);
			wcParseNode(wcParseNodeType _Type, lex::wcToken _Token);
			wcParseNode(lex::wcToken _Token);

			bool operator==(const wcParseNode&), operator==(wcParseNode);
			wcParseNode& operator=(const wcParseNode&);

			lex::wcToken Token;
			wcParseNodeType Type;
		};

		enum class wcParseSymbolType
		{
			Null, Type, Variable, Function, Namespace
		};

		struct wcIdent
		{
			wcIdent();
			wcIdent(const wcIdent&), wcIdent(std::string);

			std::string to_string() const { return Ident.c_str(); }
			std::string Ident;
		};

		struct wcScope
		{
			wcScope();
			wcScope(const wcScope&), wcScope(std::string);

			std::string to_string() const { return FullIdent.c_str(); }
			std::string FullIdent;
		};


		struct wcFullIdent
		{
			wcFullIdent();
			wcFullIdent(const wcFullIdent&), wcFullIdent(wcIdent, wcScope), wcFullIdent(std::string), wcFullIdent(std::string, std::string);
			bool operator==(const wcFullIdent& p) const
			{
				return to_string() == p.to_string();
			}

			wcFullIdent& operator=(const wcFullIdent& Other)
			{
				ShortIdent = Other.ShortIdent;
				Scope = Other.Scope;
				return *this;
			}

			std::string to_string() const;

			wcIdent ShortIdent;
			wcScope Scope;
		};

		class wcParseSymbol
		{
		public:
			wcParseSymbol();
			wcParseSymbol(const wcParseSymbol&);
			wcParseSymbol(wcParseSymbolType _Type, wcFullIdent FullIdent, lex::wcToken IdentToken);
			wcParseSymbol(wcParseSymbolType _Type, wcFullIdent FullIdent, wcFullIdent DataType, lex::wcToken IdentToken);

			wcParseSymbolType Type;
			wcFullIdent DataType;
			wcFullIdent FullIdent;
			lex::wcToken IdentToken;
			unsigned int Arguments;
			bool Registered, Const;
		};

		struct wcParseFunctionSignature
		{
			std::string to_string();
			std::string to_string_no_arguments();

			wcParseSymbol Ident;
			std::vector<wcParseSymbol> ArgumentTypes;
		};

		class wcParseSymbolTable
		{
			std::unordered_map<std::string, wcParseSymbol> Container;
			std::unordered_map<std::string, wcParseFunctionSignature> FunctionContainer;
			std::vector<std::string> ShortFunctionNames; 
			
			wcParseSymbol CurrentScope;

		public:
			wcParseSymbolTable();

			wcParseSymbolTable& operator=(const wcParseSymbolTable&);

			bool Add(wcParseSymbolType, wcIdent Ident, lex::wcToken IdentToken),
				Add(wcParseSymbolType, wcFullIdent Ident, lex::wcToken IdentToken, bool SetScopeToThisSymbol = false),
				Add(wcParseSymbol Sym, bool SetScopeToThisSymbol = false),
				Add(wcParseFunctionSignature Sig, bool SetScopeToThisSymbol = false),

				Exists(wcFullIdent FullIdent) const;

			wcParseSymbol Get(wcFullIdent FullIdent) const;

			bool SetScope(wcFullIdent FullIdent), SetScope(wcParseSymbol&);
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

			void AddAsChild(wcParseOutput, bool PointToChild = false);
			void AddAsChild(wcParseNode, bool PointToChild = false);

			void Up() { NodeIndex = AST.parent(NodeIndex); }

			tree<wcParseNode>::pre_order_iterator GetHead() { return UnofficialHead; }
			tree<wcParseNode>::pre_order_iterator GetSubHead();

			wcParseOutput operator+(wcParseOutput), operator+=(wcParseOutput), operator=(wcParseOutput);

			tree<wcParseNode> AST;
			wcParseSymbolTable SymbolTable;
			wcParserError Error;
		};

		enum class wcParseExpressionType
		{
			Literal, Variable, Unary, Call, Binary, Logical, Grouping, Assignment
		};

		class wcParseExpression
		{
			tree<wcParseNode>::pre_order_iterator GetExpressionNodeBegin(), GetExpressionNodeEnd(), GetExpressionRootNodeBegin(), GetExpressionRootNodeEnd();

		public:
			wcParseExpressionType Type;
			wcParserError Error;
			std::vector<lex::wcToken> Tokens;
			tree<wcParseNode> AST;

			wcParseExpression();
			wcParseExpression(wcParserError _Error);
			wcParseExpression(wcParseNodeType HeadType, wcParseExpression LeftHand, lex::wcToken Operator, wcParseExpression RightHand);		//binary / logical / assignment
			wcParseExpression(wcParseNodeType HeadType, lex::wcToken Operator, wcParseExpression RightHand);									//unary
			wcParseExpression(wcParseNodeType HeadType, wcParseExpression Callee, std::vector<wcParseExpression> Arguments);					//call
			wcParseExpression(wcParseNodeType HeadType, lex::wcToken OperatorOrLiteral);														//literal/operator
			//wcParseExpression(wcParseExpression& OtherExpression);																			//grouping

			void Eval();
			bool IsErrored();
		};

		class wcExpressionParser
		{
			wcParseSymbolTable& SymbolTable;
			lex::wcTokenizer& Tokenizer;
			wcParserError Error;

			wcParseExpression ParseExpression_Expression(), ParseExpression_SubExpression(), 
				ParseExpression_Equality(), ParseExpression_Assignment(), ParseExpression_LogicOr(),
				ParseExpression_LogicAnd(), ParseExpression_Comparison(),
				ParseExpression_Term(), ParseExpression_Factor(),
				ParseExpression_Unary(), ParseExpression_Call(), ParseExpression_CallArguments(wcParseExpression Callee), ParseExpression_Primary();
		public:
			wcExpressionParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable);

			wcParseOutput ParseExpression(), ParseExpression(wcParseSymbol);
		};

		class wcIdentParser
		{
			wcParseSymbolTable& SymbolTable;
			lex::wcTokenizer& Tokenizer;
		public:
			wcIdentParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable);

			wcParseSymbol ParseIdent();
		};

		class wcParser
		{
			lex::wcTokenizer& Tokenizer;
			wcParseSymbolTable SymbolTable;

			wcParseOutput ParseStatement(bool AllowDeclarations);
			wcParseOutput ParseBlock(bool AllowDeclarations);
			wcParseOutput ParseIf();
			wcParseOutput ParseSemiColon();
			wcParseOutput ParseReturn();
			wcParseOutput ParseWhile();
			wcParseOutput ParseDeclaration(), ParseDeclarationArguments(wcFullIdent, wcFullIdent, lex::wcToken, wcFullIdent& Output), ParseDeclarationBody();

		public:
			wcParser(lex::wcTokenizer& _Tokenizer);

			wcParseOutput Parse();
		};

	}

}


#endif
