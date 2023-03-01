#ifndef WC_LEX
#define WC_LEX
#include <string>
#include <vector>

#define WC_SWITCHCASE_TOKENS_OPERATORS_ALL	case wcTokenType::MinusOperator: case wcTokenType::PlusOperator: case wcTokenType::DivideOperator: case wcTokenType::MultiplyOperator: \
		case wcTokenType::ModulusOperator: case wcTokenType::ExponentOperator: case wcTokenType::IncrementOperator: case wcTokenType::DecrementOperator: \
		case wcTokenType::AssignOperator: case wcTokenType::PlusAssignOperator: case wcTokenType::MinusAssignOperator: case wcTokenType::MultAssignOperator: \
		case wcTokenType::DivAssignOperator: case wcTokenType::GreaterOperator: case wcTokenType::LessOperator: case wcTokenType::EqualOperator:\
		case wcTokenType::NotEqualOperator: case wcTokenType::GreaterEqualOperator: case wcTokenType::LessEqualOperator: case wcTokenType::LogNotOperator: \
		case wcTokenType::LogOrOperator: case wcTokenType::LogAndOperator: 
#define WC_SWITCHCASE_TOKENS_OPERATORS_ARITHMETIC case wcTokenType::MinusOperator: case wcTokenType::PlusOperator: case wcTokenType::DivideOperator:\
		case wcTokenType::MultiplyOperator: case wcTokenType::ModulusOperator: case wcTokenType::ExponentOperator: case wcTokenType::IncrementOperator:\
		case wcTokenType::DecrementOperator: 
#define WC_SWITCHCASE_TOKENS_OPERATORS_KEYWORD case wcTokenType::TrueKeyword: case wcTokenType::FalseKeyword: case wcTokenType::StringKeyword: case wcTokenType::IntKeyword: \
		case wcTokenType::FloatKeyword: case wcTokenType::BoolKeyword: case wcTokenType::CharKeyword: case wcTokenType::ObjectKeyword: case wcTokenType::VarKeyword: \
		case wcTokenType::NamespaceKeyword: case wcTokenType::FunctionKeyword: case wcTokenType::IfKeyword: case wcTokenType::ElseKeyword: case wcTokenType::ElseIfKeyword:\
		case wcTokenType::WhileKeyword: case wcTokenType::BreakKeyword: case wcTokenType::ContinueKeyword: case wcTokenType::ReturnKeyword: case wcTokenType::ConstKeyword: 
#define WC_SWITCHCASE_TOKENS_LITERAL case wcTokenType::IntLiteral: case wcTokenType::StringLiteral: case wcTokenType::FloatLiteral: case wcTokenType::CharLiteral: 
#define WC_SWITCHCASE_TOKENS_BUILTIN_TYPES case wcTokenType::IntKeyword: case wcTokenType::StringKeyword: case wcTokenType::FloatKeyword: case wcTokenType::CharKeyword: case wcTokenType::UIntKeyword: 

namespace weec
{
	namespace lex
	{
		enum class wcStringTokenType
		{
			Null, Whitespace, NewLine, Number, FloatNumber, Alpha, Punctuation, Special
		};

		struct wcStringToken
		{
			wcStringToken();
			wcStringToken(std::string _data, wcStringTokenType _type, int _line, int _column);
			wcStringToken& operator=(wcStringToken);
			bool operator==(const wcStringToken&);

			wcStringTokenType Type;
			std::string Data;
			unsigned int Line, Column;
		};

		enum class wcTokenType
		{
			Null,

			//value types
			IntLiteral, StringLiteral, FloatLiteral, CharLiteral,

			//operators
			MinusOperator, PlusOperator, DivideOperator, MultiplyOperator, ModulusOperator, ExponentOperator, IncrementOperator, DecrementOperator,
			AssignOperator, PlusAssignOperator, MinusAssignOperator, MultAssignOperator, DivAssignOperator,
			GreaterOperator, LessOperator, EqualOperator, NotEqualOperator, GreaterEqualOperator, LessEqualOperator,
			LogNotOperator, LogOrOperator, LogAndOperator,

			//reserved words
			TrueKeyword, FalseKeyword,
			StringKeyword, DoubleKeyword, IntKeyword, UIntKeyword, FloatKeyword, BoolKeyword, CharKeyword, ObjectKeyword, VarKeyword,
			NamespaceKeyword, FunctionKeyword, IfKeyword, ElseKeyword, 
			WhileKeyword, BreakKeyword, ContinueKeyword, ReturnKeyword, ConstKeyword,

			//misc
			Comment, MultiLineCommentStart, MultiLineCommentEnd, Identifier,
			OpenParenthesis, CloseParenthesis, OpenBracket, CloseBracket, OpenBrace, CloseBrace,
			SemiColon, Colon, DoubleColon, Comma, Period, SingleQuote, DoubleQuote,
			Dollar, Amper, BackSlash, QMark, Pipe, Underscore, Tilde, Hash,
			Tab, Whitespace, NewLine, Eos, Eof
		};

		std::string wcTokenTypeToString(wcTokenType);
		
		//matches string literals to wcTokenTypes
		struct wcTokenDefinition
		{
			wcTokenDefinition();	//tt_null
			wcTokenDefinition(wcTokenType _type, std::string);
			wcTokenDefinition(wcTokenType _type, std::string, bool);
			wcTokenDefinition(wcTokenType _type, std::string, bool, bool);
			wcTokenDefinition(wcTokenType _type, std::vector<std::string> _identifiers);
			wcTokenDefinition(wcTokenType _type, std::vector<std::string> _identifiers, bool _isDelimiter);
			wcTokenDefinition(wcTokenType _type, std::vector<std::string> _identifiers, bool _isDelimiter, bool _isPunctuation);
			wcTokenDefinition(const wcTokenDefinition&);

			bool isSingleCharacterToken() const;
			bool isNull() const;

			const wcTokenType Type;
			const bool delimiter, punctuation;
			const unsigned int precedence;
			const std::vector<std::string> identifiers;
		};

		const unsigned int WC_VAR_DEFINITIONCOUNT = 64;

		//definitions used for creating wcToken
		struct wcTokenDefinitionsBank
		{
			wcTokenDefinitionsBank();
			wcTokenDefinitionsBank(const std::vector<wcTokenDefinition>&);

			bool exists(std::string),
				exists(char), exists(wcTokenType);

			const wcTokenDefinition find(std::string) const,
				find(char) const, find(wcTokenType) const;

		private:
			const wcTokenDefinition* cache;
			void populateDelimiterTypes();
			std::vector<wcTokenType> delimiterTypes;
		};

		struct wcToken
		{
			wcToken();
			wcToken(wcStringToken _string);

			wcToken& operator=(const wcToken&);
			bool operator==(const wcToken&), operator==(wcToken&);

			bool IsOperator() const, IsBuiltinType() const;

			wcTokenType Type;
			wcStringToken StringToken;
		};
		bool operator==(const wcToken&, wcToken&);
		bool operator==(wcToken&, const wcToken&);

		struct wcStringTokenTypeAlizer
		{
			wcStringTokenType Get(char) const;
			wcStringTokenType Get(std::string) const;
		};

		class wcLineFeeder
		{
			friend class wcStringTokenizer;

			std::string& Source, Buffer;
			int Index, Line;
		public:
			wcLineFeeder(std::string& _source);
			wcLineFeeder& operator=(wcLineFeeder);

			int GetLineNum() { return Line; }
			std::string GetLine();
			bool NextLine();
		};

		class wcStringTokenizer
		{
			std::string Line;
			wcStringToken TokenBuffer;
			wcLineFeeder Feeder;
			unsigned int Index;
			std::string deliminators[WC_VAR_DEFINITIONCOUNT];
			bool Finished;

			bool IsDelimiter(std::string);
			bool IncIndex();
			bool Match(wcStringTokenType), Match(char), Match(std::string);

		public:
			wcStringTokenizer(std::string& _source);
			wcStringTokenizer& operator=(wcStringTokenizer&);

			wcStringToken GetStringToken() const;
			bool NextStringToken(), NextStringToken(wcStringTokenType),
				NextStringToken(char), NextStringToken(std::string), IsFinished();

			bool Lookahead(wcStringToken& output), Lookahead(wcStringToken& output, int lookaheadCount),
				LookaheadAndMatch(wcStringToken& output, int lookaheadCount, char matchChar),
				LookaheadAndMatch(wcStringToken& output, int lookaheadCount, std::string matchString),
				LookaheadAndMatch(wcStringToken& output, int lookaheadCount, wcStringTokenType matchType);
		};

		class wcTokenTypeAlizer
		{
			wcTokenDefinitionsBank definitionsBank;
			bool IsPartValidIdent(std::string);

		public:
			wcTokenType Get(std::string);
			bool IsValidIdent(std::string), IsIdentQualified(std::string);
		};

		enum class wcTokenizerErrorCode
		{
			None, FuckKnows, NewLineInStringLiteral, UnclosedStringLiteral, NewLineInCharLiteral, UnclosedCharLiteral, MalformedIdentifier, UnknownToken, UnexpectedToken
		};

		struct wcTokenizerError
		{
			wcTokenizerError();
			wcTokenizerError(wcTokenizerErrorCode _Code, wcStringToken _Data);
			wcTokenizerError(wcTokenizerErrorCode _Code, wcStringToken _Data, std::string _Msg);

			wcTokenizerErrorCode Code;
			wcStringToken Data;
			std::string Msg;
		};

		class wcTokenizer
		{
			wcStringTokenizer stringTokenizer;
			wcStringToken lookaheadBuffer;
			wcToken TokenBuffer;
			wcTokenTypeAlizer tokenTypeAlizer;

			bool NextToken_StringLiteral(), NextToken_CharLiteral(),
				NextToken_Number(), NextToken_Alpha(), NextToken_Ident(),
				NextToken_Punctuation(), NextToken_Special(), NextToken_SingleLineComment(), NextToken_MultiLineComment();

		public:
			wcTokenizerError Error;
			wcTokenizer(std::string& _source);
			wcTokenizer& operator=(wcTokenizer&);

			wcToken GetToken() const;
			bool NextToken(), NextToken(wcTokenType Type), IsErrored(), IsFinished();
		};

		const wcTokenDefinition definitions[WC_VAR_DEFINITIONCOUNT] =
		{
			wcTokenDefinition(wcTokenType::Null, ""),
			wcTokenDefinition(wcTokenType::Whitespace, " ", true, true),
			wcTokenDefinition(wcTokenType::NewLine, "\n", true),
			wcTokenDefinition(wcTokenType::Tab, "\t", true),
			wcTokenDefinition(wcTokenType::Period, ".", true, true),
			wcTokenDefinition(wcTokenType::Comma, ",", true),
			wcTokenDefinition(wcTokenType::SemiColon, ";", true),
			wcTokenDefinition(wcTokenType::Colon, ":", true),
			wcTokenDefinition(wcTokenType::DoubleColon, "::", true),
			wcTokenDefinition(wcTokenType::Tilde, "`", true),
			wcTokenDefinition(wcTokenType::Underscore, "_", true),
			wcTokenDefinition(wcTokenType::Amper, "&", true),
			wcTokenDefinition(wcTokenType::SingleQuote, "'", true),
			wcTokenDefinition(wcTokenType::DoubleQuote, "\"", true),
			wcTokenDefinition(wcTokenType::OpenParenthesis, "(", true),
			wcTokenDefinition(wcTokenType::CloseParenthesis, ")", true),
			wcTokenDefinition(wcTokenType::OpenBrace, "{", true),
			wcTokenDefinition(wcTokenType::CloseBrace, "}", true),
			wcTokenDefinition(wcTokenType::OpenBracket, "[", true),
			wcTokenDefinition(wcTokenType::CloseBracket, "]", true),
			wcTokenDefinition(wcTokenType::Hash, "#", true),

			//arithmetic
			wcTokenDefinition(wcTokenType::AssignOperator, "=", true),
			wcTokenDefinition(wcTokenType::PlusOperator, "+", true),
			wcTokenDefinition(wcTokenType::MinusOperator, "-", true),
			wcTokenDefinition(wcTokenType::DivideOperator, "/", true),
			wcTokenDefinition(wcTokenType::MultiplyOperator, "*", true),
			wcTokenDefinition(wcTokenType::PlusAssignOperator, "+=", true),
			wcTokenDefinition(wcTokenType::MinusAssignOperator, "-=", true),
			wcTokenDefinition(wcTokenType::DivAssignOperator, "/=", true),
			wcTokenDefinition(wcTokenType::MultAssignOperator, "*=", true),
			wcTokenDefinition(wcTokenType::ModulusOperator, "%", true),
			wcTokenDefinition(wcTokenType::ExponentOperator, "^", true),
			wcTokenDefinition(wcTokenType::IncrementOperator, "++", true),
			wcTokenDefinition(wcTokenType::DecrementOperator, "--", true),

			//logical/bool
			wcTokenDefinition(wcTokenType::LogNotOperator, "!", true),
			wcTokenDefinition(wcTokenType::LogOrOperator, "||", true),
			wcTokenDefinition(wcTokenType::LogAndOperator, "&&", true),
			wcTokenDefinition(wcTokenType::GreaterOperator, ">", true),
			wcTokenDefinition(wcTokenType::GreaterEqualOperator, ">=", true),
			wcTokenDefinition(wcTokenType::LessOperator , "<", true),
			wcTokenDefinition(wcTokenType::LessEqualOperator, "<=", true),
			wcTokenDefinition(wcTokenType::EqualOperator, "==", true),
			wcTokenDefinition(wcTokenType::NotEqualOperator, "!=", true),
			wcTokenDefinition(wcTokenType::Comment, "//"),
			wcTokenDefinition(wcTokenType::MultiLineCommentStart, "/*"),
			wcTokenDefinition(wcTokenType::MultiLineCommentEnd,  "*/"),

			//keywords
			wcTokenDefinition(wcTokenType::IntKeyword, "int"),
			wcTokenDefinition(wcTokenType::UIntKeyword, "uint"),
			wcTokenDefinition(wcTokenType::CharKeyword, "char"),
			wcTokenDefinition(wcTokenType::StringKeyword, "string"),
			wcTokenDefinition(wcTokenType::DoubleKeyword, "double"),
			wcTokenDefinition(wcTokenType::FloatKeyword, "float"),
			wcTokenDefinition(wcTokenType::ConstKeyword, "const"),
			wcTokenDefinition(wcTokenType::BoolKeyword, "bool"),
			wcTokenDefinition(wcTokenType::TrueKeyword, "true"),
			wcTokenDefinition(wcTokenType::FalseKeyword, "false"),
			wcTokenDefinition(wcTokenType::IfKeyword, "if"),
			wcTokenDefinition(wcTokenType::ElseKeyword, "else"),
			wcTokenDefinition(wcTokenType::ReturnKeyword, "return"),
			wcTokenDefinition(wcTokenType::WhileKeyword, "while"),
			wcTokenDefinition(wcTokenType::BreakKeyword, "break"),
			wcTokenDefinition(wcTokenType::ContinueKeyword, "continue"),
			wcTokenDefinition(wcTokenType::NamespaceKeyword, "namespace"),

			wcTokenDefinition(wcTokenType::IntLiteral, { "0","1","2","3","4","5","6","7","8","9" })
		};

		namespace misc
		{
			extern wcStringToken NullToken;
		}
	}
}

#endif