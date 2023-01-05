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
			
			IfStatement, If_Expression, If_TrueBlock, If_ElseBlock,

			ReturnStatement, Return_Expression,

			Declaration, Declaration_Type, Declaration_Ident,

			Expression, Expression_Equality, Expression_Assignment, Expression_LogicOr, Expression_LogicAnd, Expression_Comparison, Expression_Term,
			Expression_Factor, Expression_Unary, Expression_Primary, Expression_Operator,

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

			Experession_Empty,
			Experession_UnexpectedToken,
			Experession_MissingClosingParenthesis,
			Experession_UnexpectedEOF
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

			std::string Get() const { return Ident.c_str(); }
			std::string Ident;
		};

		struct wcScope
		{
			wcScope();
			wcScope(const wcScope&), wcScope(std::string);

			std::string Get() const { return FullIdent.c_str(); }
			std::string FullIdent;
		};


		struct wcFullIdent
		{
			wcFullIdent();
			wcFullIdent(const wcFullIdent&), wcFullIdent(wcIdent, wcScope), wcFullIdent(std::string), wcFullIdent(std::string, std::string);
			bool operator==(const wcFullIdent& p) const
			{
				return Get() == p.Get();
			}
			std::string Get() const;
			wcIdent Ident;
			wcScope Scope;
		};

		class wcParseSymbol
		{
		public:
			wcParseSymbol();
			wcParseSymbol(const wcParseSymbol&);
			wcParseSymbol(wcParseSymbolType _Type, wcFullIdent FullIdent);

			wcParseSymbolType Type;
			wcFullIdent FullIdent;
			bool Registered, Const;
		};

		class wcParseSymbolTable
		{
			std::unordered_map<std::string, wcParseSymbol> Container;
			wcParseSymbol CurrentScopeFullIdent;

		public:
			wcParseSymbolTable();

			wcParseSymbolTable& operator=(const wcParseSymbolTable&);

			bool Add(wcParseSymbolType, wcIdent Ident),
				Add(wcParseSymbolType, wcFullIdent Ident, bool SetScopeToThisSymbol = false),

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

		class wcParseExpression
		{
			tree<wcParseNode>::pre_order_iterator GetExpressionNodeBegin(), GetExpressionNodeEnd(), GetExpressionRootNodeBegin(), GetExpressionRootNodeEnd();

		public:
			wcParserError Error;
			std::vector<lex::wcToken> Tokens;
			tree<wcParseNode> AST;

			wcParseExpression();
			wcParseExpression(wcParseNodeType HeadType, wcParseExpression LeftHand, lex::wcToken Operator, wcParseExpression RightHand);	//binary
			wcParseExpression(wcParseNodeType HeadType, lex::wcToken Operator, wcParseExpression RightHand);								//unary
			wcParseExpression(wcParseNodeType HeadType, lex::wcToken OperatorOrLiteral);						//literal/operator
			//wcParseExpression(wcParseExpression& OtherExpression);												//grouping

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
				ParseExpression_Unary(), ParseExpression_Primary();
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
			wcParseOutput ParseDeclaration(), ParseDeclaration_Function(), ParseDeclaration_Variable();
			wcParseOutput ParseDeclaration(wcParseSymbol), ParseDeclaration_Function(wcParseSymbol), ParseDeclaration_Variable(wcParseSymbol);

		public:
			wcParser(lex::wcTokenizer& _Tokenizer);

			wcParseOutput Parse();
		};

	}

}


#endif
