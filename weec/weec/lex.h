#ifndef WC_LEX
#define WC_LEX
#include <string>
#include <vector>

#define WC_SWITCHCASE_TOKENS_OPERATORS_ALL	case wcTokenType::MinusOperator: case wcTokenType::PlusOperator: case wcTokenType::DivideOperator: case wcTokenType::MultiplyOperator: \
		case wcTokenType::ModulusOperator: case wcTokenType::ExponentOperator: case wcTokenType::IncrementOperator: case wcTokenType::DecrementOperator: \
		case wcTokenType::AssignOperator: case wcTokenType::PlusAssignOperator: case wcTokenType::MinusAssignOperator: case wcTokenType::MultAssignOperator: \
		case wcTokenType::DivAssignOperator: case wcTokenType::GreaterOperator: case wcTokenType::LessOperator: case wcTokenType::EqualOperator:\
		case wcTokenType::NotEqualOperator: case wcTokenType::GreaterEqualOperator: case wcTokenType::LessEqualOperator: case wcTokenType::LogNotOperator: \
		case wcTokenType::LogOrOperator: case wcTokenType::LogAndOperator:  case wcTokenType::TernaryOperator: 
#define WC_SWITCHCASE_TOKENS_OPERATORS_ARITHMETIC case wcTokenType::MinusOperator: case wcTokenType::PlusOperator: case wcTokenType::DivideOperator:\
		case wcTokenType::MultiplyOperator: case wcTokenType::ModulusOperator: case wcTokenType::ExponentOperator: case wcTokenType::IncrementOperator:\
		case wcTokenType::DecrementOperator: case wcTokenType::AssignOperator: case wcTokenType::PlusAssignOperator: case wcTokenType::MinusAssignOperator: \
		case wcTokenType::MultAssignOperator: case wcTokenType::DivAssignOperator:
#define WC_SWITCHCASE_TOKENS_KEYWORDS case wcTokenType::TrueKeyword: case wcTokenType::FalseKeyword: case wcTokenType::StringKeyword: case wcTokenType::IntKeyword: \
		case wcTokenType::FloatKeyword: case wcTokenType::BoolKeyword: case wcTokenType::CharKeyword: case wcTokenType::ObjectKeyword: case wcTokenType::VarKeyword: \
		case wcTokenType::NamespaceKeyword: case wcTokenType::FunctionKeyword: case wcTokenType::IfKeyword: case wcTokenType::ElseKeyword: case wcTokenType::ElseIfKeyword:\
		case wcTokenType::WhileKeyword: case wcTokenType::BreakKeyword: case wcTokenType::ContinueKeyword: case wcTokenType::ReturnKeyword: case wcTokenType::ConstKeyword:  case wcTokenType::StructKeyword: 
#define WC_SWITCHCASE_TOKENS_LITERAL case wcTokenType::IntLiteral: case wcTokenType::StringLiteral: case wcTokenType::FloatLiteral: case wcTokenType::CharLiteral: case wcTokenType::TrueKeyword: case wcTokenType::FalseKeyword: 
#define WC_SWITCHCASE_TOKENS_BUILTIN_TYPES case wcTokenType::IntKeyword: case wcTokenType::StringKeyword: case wcTokenType::FloatKeyword: case wcTokenType::CharKeyword: case wcTokenType::UIntKeyword: case wcTokenType::VoidKeyword: 

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
			wcStringToken(std::string& _data, wcStringTokenType _type, int _line, int _column);
			wcStringToken(std::string _data, wcStringTokenType _type, int _line, int _column);
			wcStringToken& operator=(wcStringToken);
			bool operator==(const wcStringToken&), operator==(wcStringToken&);

			wcStringTokenType Type;
			std::string Data;
			unsigned int Line, Column;
		};

		enum class wcTokenType
		{
			NullToken,

			//value types
			IntLiteral, StringLiteral, FloatLiteral, CharLiteral,

			//operators
			MinusOperator, PlusOperator, DivideOperator, MultiplyOperator, ModulusOperator, ExponentOperator, IncrementOperator, DecrementOperator,
			AssignOperator, PlusAssignOperator, MinusAssignOperator, MultAssignOperator, DivAssignOperator,
			GreaterOperator, LessOperator, EqualOperator, NotEqualOperator, GreaterEqualOperator, LessEqualOperator,
			LogNotOperator, LogOrOperator, LogAndOperator,

			//reserved words
			TrueKeyword, FalseKeyword,
			StringKeyword, DoubleKeyword, IntKeyword, UIntKeyword, FloatKeyword, BoolKeyword, CharKeyword, VoidKeyword, ObjectKeyword, VarKeyword,
			NamespaceKeyword, FunctionKeyword, IfKeyword, ElseKeyword, 
			WhileKeyword, BreakKeyword, ContinueKeyword, ReturnKeyword, InlineKeyword, ConstKeyword, PrintKeyword, StructKeyword,

			//misc
			Comment, MultiLineCommentStart, MultiLineCommentEnd, Identifier,
			OpenParenthesis, CloseParenthesis, OpenBracket, CloseBracket, OpenBrace, CloseBrace,
			SemiColon, Colon, DoubleColon, Comma, Period, SingleQuote, DoubleQuote,
			Dollar, Amper, BackSlash, TernaryOperator, Pipe, Underscore, Tilde, Hash,
			Whitespace, NewLine, Eos, Eof
		};

		std::string to_string(wcTokenType);
		
		struct wcToken
		{
			wcToken();
			wcToken(const wcStringToken& _string);

			wcToken& operator=(const wcToken&);
			bool operator==(const wcToken&), operator==(wcToken&);

			bool IsOperator() const, IsBuiltinType() const, IsWhitespace() const;

			std::string to_string() const
			{
				return StringToken.Data;
			}

			wcTokenType Type;
			wcStringToken StringToken;
		};
		bool operator==(const wcToken&, wcToken&);
		bool operator==(wcToken&, const wcToken&);

		struct wcStringTokenTypeAlizer
		{
			wcStringTokenType Get(char&) const;
			wcStringTokenType Get(std::string&) const;
		};

		class wcLineFeeder
		{
			friend class wcStringTokenizer;

			std::string& Source;
			std::string Buffer;
			int Index, Line;
		public:
			wcLineFeeder(std::string&  _source);
			wcLineFeeder(const wcLineFeeder& _otherCopy);
			wcLineFeeder& operator=(wcLineFeeder&);

			int GetLineNum() { return Line; }
			std::string GetLine();
			bool NextLine();
		};

		class wcStringTokenizer
		{
			std::string Line;
			wcStringToken StringTokenBuffer;
			wcLineFeeder Feeder;
			unsigned int Index;
			bool Finished;

			bool IncIndex();
			bool Match(wcStringTokenType), Match(char), Match(std::string);

		public:
			wcStringTokenizer(std::string& const Source);
			wcStringTokenizer(const wcStringTokenizer& Source);
			wcStringTokenizer& operator=(wcStringTokenizer&);

			wcStringToken GetStringToken() const;
			bool NextStringToken(), NextStringToken(wcStringTokenType),
				NextStringToken(char), NextStringToken(std::string), IsFinished() const;

			bool Lookahead(wcStringToken& output), Lookahead(wcStringToken& output, int lookaheadCount),
				LookaheadAndMatch(wcStringToken& output, int lookaheadCount, char matchChar),
				LookaheadAndMatch(wcStringToken& output, int lookaheadCount, std::string matchString),
				LookaheadAndMatch(wcStringToken& output, int lookaheadCount, wcStringTokenType matchType);
		};

		class wcTokenTypeAlizer
		{
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
			wcTokenizer(std::string& Source, bool _NextToken = false);
			wcTokenizer& operator=(wcTokenizer&);

			wcToken GetToken() const;
			bool NextToken(), NextToken(wcTokenType Type), IsErrored() const, IsFinished() const;
		};

		namespace misc
		{
			extern wcStringToken NullToken;
		}
	}
}

#endif