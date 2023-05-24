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
		struct wcParserConsts
		{
			std::string GlobalIdentifier = "$g",
				ScopeDelimiter = "::",
				GlobalIdentPrefix = GlobalIdentifier + ScopeDelimiter;
		};

		static wcParserConsts ParserConsts;

		enum class wcParseNodeType
		{
			Empty, Head,

			Statement, Block,

			WhileStatement,

			IfStatement, If_Expression, If_TrueBlock, If_ElseBlock,

			ReturnStatement, Return_Expression,

			Declaration, Declaration_Type, Declaration_Ident, Parameters, Parameter, Parameter_Type, Parameter_Ident, DeclaratationBody,

			//Temporary, hackish
			Identifier,

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

			CouldntResolveType, AmbiguousType, CouldntResolveIdentifier, AmbiguousIdentifier,

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

		enum class wcIdentalyzerAnalyzeResultCode
		{
			InvalidIdentifier, ShortIdentifier, QualifiedIdentifier, ShortFunction, QualifiedFunction
		};

		class wcIdentifier; class wcIdentifierScope;

		class wcIdentalyzer
		{
		public:
			wcIdentalyzerAnalyzeResultCode Analyze(std::string);

			bool Create(std::string IdentifierString, wcIdentifierScope& ScopeOutput, wcIdentifier& IdentifierOutput);

			std::string GetNamespaceFromQualifiedIdentifier(std::string IdentifierString),
				GetIdentifierFromQualifiedIdentifier(std::string IdentifierString),
				StripArgumentsFromFunctionIdentifier(std::string FunctionIdentifierString);

			bool ContainsGlobal(std::string Identifier) const
			{
				return Identifier.find(ParserConsts.GlobalIdentifier) != std::string::npos;
			}

			bool ContainsNamespace(std::string Identifier) const
			{
				return Identifier.find(ParserConsts.ScopeDelimiter) != std::string::npos;
			}

			bool IsQualified(std::string), IsFunction(std::string), IsValid(std::string), IsQualifiedWithGlobal(std::string);
		};

		class wcBaseIdentifier
		{

		};

		class wcIdentifier
		{
		public:
			wcIdentifier() {}
			wcIdentifier(const wcIdentifier&), wcIdentifier(std::string);

			bool operator==(const wcIdentifier& p) const
			{
				//strip arguments from either side
				auto l = to_string().find("(") != to_string().npos 
					? to_string().substr(0, to_string().find_first_of("(")) 
					: to_string();

				auto r = p.to_string().find("(") != p.to_string().npos 
					? p.to_string().substr(0, p.to_string().find_first_of("(")) 
					: p.to_string();

				return l == r;
			}

			bool operator==(const std::string& p) const
			{
				//strip arguments from either side
				auto l = to_string().find("(") != to_string().npos 
					? to_string().substr(0, to_string().find_first_of("(")) 
					: to_string();

				auto r = p.find("(") != p.npos 
					? p.substr(0, p.find_first_of("(")) 
					: p;

				return l == r;
			}

			bool IsFunction() const
			{
				return wcIdentalyzer().IsFunction(Identifier);
			}

			bool IsQualified() const
			{
				return wcIdentalyzer().IsQualified(Identifier);
			}

			std::string to_string() const
			{
				return Identifier;
			}

			std::string to_unqualified_string() const
			{
				//remove $g:: and any other namespaces
				if (wcIdentalyzer().ContainsNamespace(Identifier))
					return Identifier.substr(Identifier.find_last_of(ParserConsts.ScopeDelimiter) + 1, Identifier.size() - (Identifier.find_last_of(ParserConsts.ScopeDelimiter) + 1));				

				if (wcIdentalyzer().ContainsGlobal(Identifier))
					return "";	//no namespace delimiter, but a present global ident... must be just "$g". the unqualified version of this is an empty string

				//no namespace, no global, this identifier already is unqualified
				return Identifier;
			}

			std::string to_string_no_global() const
			{
				if (!wcIdentalyzer().ContainsGlobal(Identifier))
					return Identifier;	//global wasnt present
				else
					if (wcIdentalyzer().ContainsNamespace(Identifier))
					{
						auto l = Identifier.substr(Identifier.find_first_of(ParserConsts.GlobalIdentPrefix) + 4, Identifier.size() - (Identifier.find_first_of(ParserConsts.GlobalIdentPrefix) + 4));	//todo remove debug line 
						return l;
					}
					else
						return "";  //must be just "$g"
			}

			std::string to_string_no_arguments() const 
			{ 				
				return IsFunction()
					? wcIdentalyzer().StripArgumentsFromFunctionIdentifier(to_string())
					: to_string();
			}

			std::string to_string_unqualified_no_arguments() const 
			{ 
				return IsFunction()
					? wcIdentalyzer().StripArgumentsFromFunctionIdentifier(to_unqualified_string())
					: to_unqualified_string();
			}

			std::string to_string_no_arguments_no_global() const 
			{
				return IsFunction()
					? wcIdentalyzer().StripArgumentsFromFunctionIdentifier(to_string_no_global())
					: to_string_no_global();
			}

			unsigned int Size() const
			{
				if (wcIdentalyzer().ContainsGlobal(Identifier))
					if (wcIdentalyzer().ContainsNamespace(Identifier))
					{
						auto l = Identifier.substr(Identifier.find_first_of(ParserConsts.ScopeDelimiter) + 2, Identifier.size() - (Identifier.find_first_of(ParserConsts.ScopeDelimiter) + 2));	//todo remove debug line 
						return l.size();
					}
					else
						return 0;	//must be just "$g"
				else
					return Identifier.size();
			}

		private:
			std::string Identifier;
		};

		class wcIdentifierScope
		{
			wcIdentifier Identifier;	//will always be fully qualified with global

		public:
			wcIdentifierScope()
			{
				Identifier = ParserConsts.GlobalIdentifier;
			}

			wcIdentifierScope(const wcIdentifierScope& Other)
			{
				Identifier = Other.Identifier;
			}

			wcIdentifierScope(std::string Data)
			{
				//ensure global scope is appended 
				if (!Data.starts_with(ParserConsts.GlobalIdentPrefix) && Data != ParserConsts.GlobalIdentifier)
					Data = ParserConsts.GlobalIdentPrefix + Data;

				Identifier = Data;
			}

			bool operator==(const wcIdentifierScope& p) const
			{
				return to_string() == p.to_string();
			}

			bool operator==(const std::string& p) const
			{
				if (wcIdentalyzer().IsQualified(p))
					return to_string() == p;
				return to_unqualified_string() == p;
			}

			bool IsQualified() const
			{
				//we guarantee the identifier to be fully qualified
				return true;
			}

			unsigned int Size() const
			{
				return Identifier.Size();
			}

			std::string to_string() const { return Identifier.to_string(); }

			std::string to_unqualified_string() const {	return Identifier.to_unqualified_string();	}

			std::string to_string_no_global() const	{	return Identifier.to_string_no_global();	}

		};

		class wcParseExpression; struct wcParseSymbol; struct wcParseParameter;

		class wcFullIdentifier
		{

		public:
			wcFullIdentifier() {}
			wcFullIdentifier(std::string, std::vector<wcParseExpression>);
			wcFullIdentifier(std::string, std::vector<wcParseParameter>);
			wcFullIdentifier(std::string, std::vector<wcParseSymbol>);
			wcFullIdentifier(const wcFullIdentifier& Other) 
			{
				ShortIdentifier = Other.ShortIdentifier;
				ScopeIdentifier = Other.ScopeIdentifier;
			}

			wcFullIdentifier(wcIdentifierScope _Scope, wcIdentifier _Ident)
			{
				ShortIdentifier = _Ident;
				ScopeIdentifier = _Scope;
			}

			wcFullIdentifier(const char * RawIdentifier)
			{
				wcIdentalyzer().Create(RawIdentifier, ScopeIdentifier, ShortIdentifier);
			}

			wcFullIdentifier(std::string RawIdentifier)
			{
				wcIdentalyzer().Create(RawIdentifier, ScopeIdentifier, ShortIdentifier);
			}

			bool operator==(const wcFullIdentifier& p) const
			{
				return to_string() == p.to_string();
			}

			bool operator==(const std::string& p) const
			{
				return wcIdentalyzer().IsQualifiedWithGlobal(p)
					? to_string() == p
					: to_string_no_global() == p;
			}

			bool operator==(const char * p) const
			{
				return operator==(std::string(p));
			}

			wcFullIdentifier& operator=(const wcFullIdentifier& Other)
			{
				ShortIdentifier = Other.ShortIdentifier;
				ScopeIdentifier = Other.ScopeIdentifier;
				return *this;
			}

			bool IsFunction() const
			{
				return ShortIdentifier.IsFunction();
			}

			unsigned int Size() const
			{
				return to_string_no_global().size();
			}

			std::string to_string() const
			{
				return ShortIdentifier.Size()
					? ScopeIdentifier.to_string() + ParserConsts.ScopeDelimiter + ShortIdentifier.to_string()
					: ScopeIdentifier.to_string();
			}

			std::string to_unqualified_string() const
			{
				return ShortIdentifier.to_unqualified_string();
			}

			std::string to_string_no_global() const
			{
				return ScopeIdentifier.Size()
					? ScopeIdentifier.to_string_no_global() + ParserConsts.ScopeDelimiter + ShortIdentifier.to_string()
					: ShortIdentifier.to_string();
			}

			std::string to_string_no_arguments() const
			{
				return IsFunction()
					? wcIdentalyzer().StripArgumentsFromFunctionIdentifier(to_string())
					: to_string();
			}

			std::string to_string_unqualified_no_arguments() const
			{
				return IsFunction()
					? wcIdentalyzer().StripArgumentsFromFunctionIdentifier(to_unqualified_string())
					: to_unqualified_string();
			}

			std::string to_string_no_arguments_no_global() const
			{
				return IsFunction()
					? wcIdentalyzer().StripArgumentsFromFunctionIdentifier(to_string_no_global())
					: to_string_no_global();
			}

			wcIdentifier ShortIdentifier;
			wcIdentifierScope ScopeIdentifier;
		};

		struct wcParseParameter
		{
			lex::wcToken TypeToken, IdentifierToken;
			wcFullIdentifier TypeIdentifier, Identifier;
		};

		struct wcFunctionIdentifier
		{
			wcFunctionIdentifier() {}
			wcFunctionIdentifier(wcFullIdentifier _Identifier, std::vector<wcParseParameter> Parameters)
			{
				Identifier = wcFullIdentifier(_Identifier.to_string(), Parameters);
				for (auto Parameter : Parameters)
					ParameterTypes.push_back(Parameter.TypeIdentifier);
			}

			bool operator==(const wcFunctionIdentifier& p) const
			{
				return to_string() == p.to_string();
			}

			bool operator==(const wcFullIdentifier& p) const
			{
				return to_string_no_parameters() == p.to_string();
			}

			wcFunctionIdentifier& operator=(const wcFunctionIdentifier& Other)
			{
				Identifier = Other.Identifier;
				ParameterTypes = Other.ParameterTypes;
				return *this;
			}

			wcFullIdentifier no_arguments() const
			{
				return wcFullIdentifier(Identifier.ShortIdentifier.to_string());
			}

			std::string to_string() const
			{
				return Identifier.to_string();
			}

			std::string to_string_no_parameters() const
			{
				return no_arguments().to_string();
			}

			unsigned int ArgumentCount() const
			{
				return ParameterTypes.size();
			}

			wcFullIdentifier Identifier;
			std::vector<wcFullIdentifier> ParameterTypes;
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

		struct wcParseFunctionSignature
		{
			wcParseFunctionSignature(wcFullIdentifier _DataType, lex::wcToken _IdentToken, std::vector<wcParseParameter> _Parameters, wcFunctionIdentifier _FuncFullIdent)
			{
				DataType = _DataType;
				Parameters = _Parameters;
				IdentToken = _IdentToken;
				FunctionIdentifier = _FuncFullIdent;
			}

			std::string to_string() const
			{
				return DataType.to_string() + " " + FunctionIdentifier.to_string();
			}

			std::string to_string_no_parameters() const
			{
				return DataType.to_string() + " " + FunctionIdentifier.to_string_no_parameters();
			}

			wcFullIdentifier DataType;
			lex::wcToken IdentToken;
			std::vector<wcParseParameter> Parameters;
			wcFunctionIdentifier FunctionIdentifier;
		};

		struct wcParseSymbol
		{
			wcParseSymbol();
			wcParseSymbol(const wcParseSymbol&);
			wcParseSymbol(wcParseSymbolType _Type, wcFullIdentifier FullIdent, lex::wcToken IdentToken);
			wcParseSymbol(wcParseSymbolType _Type, wcFullIdentifier FullIdent, wcFullIdentifier DataType, lex::wcToken IdentToken);
			wcParseSymbol(wcParseFunctionSignature FuntionSignature);

			wcParseSymbolType Type;
			wcFullIdentifier DataType;
			wcFullIdentifier FullIdent;

			lex::wcToken IdentToken;
			unsigned int Arguments;
			bool Registered, Const, HasOverloads;
		};

		struct wcParseScope
		{
			wcParseScope(std::string _Name)
			{
				Name = _Name;
			}

			wcFullIdentifier Name;

			std::vector<wcFullIdentifier> Symbols;
		};

		enum class wcIdentifierResolveResult
		{
			Ambiguous = -1,
			Unresolved = 0,
			Resolved = 1
		};

		class wcIdentifierResolver
		{
			std::stack<wcParseScope>& StackFrames;
		public:
			wcIdentifierResolver(std::stack<wcParseScope>& _StackFrames) :
				StackFrames(_StackFrames) {}

			wcIdentifierResolveResult Resolve(wcIdentifier In, wcFullIdentifier& Out, bool ForceResolve = true);
		};


		class wcParseSymbolTable 
		{
			std::unordered_map<std::string, wcParseSymbol> Container;
			std::unordered_map<std::string, wcParseFunctionSignature> FunctionContainer;

		public:
			wcParseSymbolTable();

			wcParseSymbolTable& operator=(const wcParseSymbolTable&);

			bool Add(wcParseSymbol Sym, bool SetScopeToThisSymbol = false),
				Add(wcParseFunctionSignature Sig, std::vector<wcParseParameter> Parameters, bool SetScopeToThisSymbol = false);

			bool Exists(wcFullIdentifier FullIdent) const;

			wcParseSymbol Get(wcFullIdentifier FullIdent) const;

			unsigned int Count() const
			{
				return Container.size();
			}

			const  wcParseSymbol NullSymbol;
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
			std::stack<wcParseScope>& Scopes;

		public:
			wcSubParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable, std::stack<wcParseScope>& _Scopes)
				: Tokenizer(_Tokenizer), SymbolTable(_SymbolTable), Scopes(_Scopes) {}

			lex::wcToken GetToken()
			{
				return Tokenizer.GetToken();
			}

			bool NextToken()
			{
				return Tokenizer.NextToken();
			}

			bool ExpectToken(lex::wcTokenType TokenType)
			{
				return Tokenizer.GetToken().Type == TokenType;
			}

			bool ExpectNextToken(lex::wcTokenType TokenType)
			{
				return Tokenizer.NextToken(TokenType);
			}
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
			wcExpressionParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable, std::stack<wcParseScope>& _Scopes)
				: wcSubParser(_Tokenizer, _SymbolTable, _Scopes) {}

			wcParseOutput ParseExpression(), ParseExpression(wcParseSymbol);
		};

		class wcIdentParser : wcSubParser
		{
		public:
			wcIdentParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable, std::stack<wcParseScope>& _Scopes);

			wcParseOutput Parse(wcIdentifier&, wcFullIdentifier& ResolvedFullIdentifier, bool Consume = true, bool ExpectDeclared = false);
		};

		class wcDeclarationParser : wcSubParser
		{
			wcParseOutput ParseType(wcFullIdentifier& DeclarationType, wcParseNodeType NodeType),
				ParseIdent(wcFullIdentifier& DeclarationFullIdentifier, lex::wcToken& IdentAsSeen),
				ParseParameters(std::vector<wcParseParameter>& ParametersOut),
				ParseParameter(wcParseParameter& ParameterOutput);

		public:
			wcDeclarationParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable, std::stack<wcParseScope>& _Scopes)
				: wcSubParser(_Tokenizer, _SymbolTable, _Scopes) {}

			wcParseOutput Parse();
		};

		class wcStatementParser : wcSubParser
		{

		public:
			wcStatementParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable, std::stack<wcParseScope>& _Scopes)
				: wcSubParser(_Tokenizer, _SymbolTable, _Scopes) {}

			wcParseOutput Parse(bool AllowDeclarations);
		};

		class wcBlockParser : wcSubParser
		{

		public:
			wcBlockParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable, std::stack<wcParseScope>& _Scopes)
				: wcSubParser(_Tokenizer, _SymbolTable, _Scopes) {}

			wcParseOutput Parse(bool AllowDeclarations);
		};

		class wcIfParser : wcSubParser
		{
		public:
			wcIfParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable, std::stack<wcParseScope>& _Scopes)
				: wcSubParser(_Tokenizer, _SymbolTable, _Scopes) {}

			wcParseOutput Parse();
		};

		class wcSemiColonParser : wcSubParser
		{
		public:
			wcSemiColonParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable, std::stack<wcParseScope>& _Scopes)
				: wcSubParser(_Tokenizer, _SymbolTable, _Scopes) {}

			wcParseOutput Parse();
		};

		class wcReturnParser : wcSubParser
		{
		public:
			wcReturnParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable, std::stack<wcParseScope>& _Scopes)
				: wcSubParser(_Tokenizer, _SymbolTable, _Scopes) {}

			wcParseOutput Parse();
		};

		class wcWhileParser : wcSubParser
		{
		public:
			wcWhileParser(lex::wcTokenizer& _Tokenizer, wcParseSymbolTable& _SymbolTable, std::stack<wcParseScope>& _Scopes)
				: wcSubParser(_Tokenizer, _SymbolTable, _Scopes) {}

			wcParseOutput Parse();
		};

		class wcParser
		{
			friend class wcDeclarationParser;
			friend class wcIdentParser;

			lex::wcTokenizer& Tokenizer;
			wcParseSymbolTable SymbolTable;
			std::stack<wcParseScope> Scopes;

		public:
			wcParser(lex::wcTokenizer& _Tokenizer);

			wcParseOutput Parse();
		};

	}

}


#endif
