#ifndef WC_TOKEN_DEFINITIONS
#define WC_TOKEN_DEFINITIONS
#include "lex.h"

namespace weec
{
	namespace lex
	{
		const wcTokenDefinition weec::lex::definitions[WC_VAR_DEFINITIONCOUNT] =
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
			wcTokenDefinition(wcTokenType::CharKeyword, "char"),
			wcTokenDefinition(wcTokenType::StringKeyword, "string"),
			wcTokenDefinition(wcTokenType::FloatKeyword, "float"),
			wcTokenDefinition(wcTokenType::ConstKeyword, "const"),
			wcTokenDefinition(wcTokenType::BoolKeyword, "bool"),
			wcTokenDefinition(wcTokenType::TrueKeyword, "true"),
			wcTokenDefinition(wcTokenType::FalseKeyword, "false"),
			wcTokenDefinition(wcTokenType::IfKeyword, "if"),
			wcTokenDefinition(wcTokenType::ElseKeyword, "else"),
			wcTokenDefinition(wcTokenType::ElseIfKeyword, "elseif"),
			wcTokenDefinition(wcTokenType::ReturnKeyword, "return"),
			wcTokenDefinition(wcTokenType::WhileKeyword, "while"),
			wcTokenDefinition(wcTokenType::BreakKeyword, "break"),
			wcTokenDefinition(wcTokenType::ContinueKeyword, "continue"),
			wcTokenDefinition(wcTokenType::NamespaceKeyword, "namespace"),

			wcTokenDefinition(wcTokenType::IntLiteral, { "0","1","2","3","4","5","6","7","8","9" })
		};
	}
}

#endif