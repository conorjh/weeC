#ifndef WC_TEST_WCTOKENIZER_H
#define WC_TEST_WCTOKENIZER_H
#include <string>
#include "test_listings.h"
#include "lex.h"

//not sure why but functions in this file must be inlined or we 
//get LNK1169 one or more multiply defined symbols found on VS22

namespace weec
{
	namespace test
	{
		inline int Test_Tokenizer_whitespace1()
		{
			using namespace weec::lex;
			using namespace weec::test::listing;

			std::string input1(list_lexer_whitespace1);
			wcTokenizer tokenizer1(input1);
			if (tokenizer1.NextToken() || !tokenizer1.GetToken().IsWhitespace())
				return 1;

			std::string input2(list_lexer_whitespace2);
			wcTokenizer tokenizer2(input2);
			if (tokenizer2.NextToken() || !tokenizer2.GetToken().IsWhitespace())
				return 2;

			std::string input3(list_lexer_whitespace3);
			wcTokenizer tokenizer3(input3);
			if (tokenizer3.NextToken() || !tokenizer3.GetToken().IsWhitespace())
				return 3;

			std::string input4(list_lexer_whitespace4);
			wcTokenizer tokenizer4(input4);
			if (tokenizer4.NextToken() || !tokenizer4.GetToken().IsWhitespace())
				return 4;

			std::string input5(list_lexer_whitespace5);
			wcTokenizer tokenizer5(input5);
			if (tokenizer5.NextToken() || !tokenizer5.GetToken().IsWhitespace())
				return 5;

			std::string input6(list_lexer_whitespace6);
			wcTokenizer tokenizer6(input6);
			if (tokenizer6.NextToken() || !tokenizer6.GetToken().IsWhitespace())
				return 6;

			std::string input7(list_lexer_whitespace7);
			wcTokenizer tokenizer7(input7);
			if (tokenizer7.NextToken() || !tokenizer7.GetToken().IsWhitespace())
				return 7;

			std::string input8(list_lexer_whitespace8);
			wcTokenizer tokenizer8(input8);
			if (tokenizer8.NextToken() || !tokenizer8.GetToken().IsWhitespace())
				return 8;

			std::string input9(list_lexer_whitespace9);
			wcTokenizer tokenizer9(input9);
			if (tokenizer9.NextToken() || !tokenizer9.GetToken().IsWhitespace())
				return 9;
			return 0;
		}

		inline int Test_Tokenizer_comments()
		{
			using namespace weec::lex;
			using namespace weec::test::listing;

			std::string input1(list_lexer_comments1);
			wcTokenizer tokenizer1(input1);
			if (tokenizer1.NextToken())
				return 1;

			std::string input2(list_lexer_comments2);
			wcTokenizer tokenizer2(input2);
			if (tokenizer2.NextToken())
				return 2;

			std::string input3(list_lexer_comments3);
			wcTokenizer tokenizer3(input3);
			if (tokenizer3.NextToken())
				return 3;

			std::string input4(list_lexer_comments4);
			wcTokenizer tokenizer4(input4);
			if (!tokenizer4.NextToken() || tokenizer4.GetToken().Type != wcTokenType::Identifier 
				|| tokenizer4.GetToken().to_string() != "ident")
				return 4;

			std::string input5(list_lexer_comments5);
			wcTokenizer tokenizer5(input5);
			if (!tokenizer5.NextToken() || tokenizer5.GetToken().Type != wcTokenType::Identifier
				|| tokenizer5.GetToken().to_string() != "ident")
				return 5;

			return 0;
		}


		inline int Test_Tokenizer_multiline_comments()
		{
			using namespace weec::lex;
			using namespace weec::test::listing;

			std::string input1(list_lexer_multiline_comment1);
			wcTokenizer tokenizer1(input1);
			if (tokenizer1.NextToken())
				return 1;

			std::string input2(list_lexer_multiline_comment2);
			wcTokenizer tokenizer2(input2);
			if (tokenizer2.NextToken())
				return 2;

			std::string input3(list_lexer_multiline_comment3);
			wcTokenizer tokenizer3(input3);
			if (tokenizer3.NextToken())
				return 3;

			return 0;
		}

		inline int Test_Tokenizer_keywords()
		{
			using namespace weec::lex;
			using namespace weec::test::listing;

			std::string input1(list_lexer_keywords);
			wcTokenizer tokenizer1(input1);
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::TrueKeyword)			return 1;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::FalseKeyword)			return 2;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::StringKeyword)		return 3;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::DoubleKeyword)		return 4;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::IntKeyword)			return 4;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::UIntKeyword)			return 4;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::FloatKeyword)			return 5;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::BoolKeyword)			return 5;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::CharKeyword)			return 6;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::VoidKeyword)			return 7;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::ObjectKeyword)		return 8;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::VarKeyword)			return 9;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::NamespaceKeyword)		return 10;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::FunctionKeyword)		return 11;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::IfKeyword)			return 12;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::ElseKeyword)			return 13;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::WhileKeyword)			return 14;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::BreakKeyword)			return 15;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::ContinueKeyword)		return 16;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::ReturnKeyword)		return 17;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::InlineKeyword)		return 18;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::ConstKeyword)			return 19;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::PrintKeyword)			return 20;
			if (!tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::StructKeyword)		return 21;

			return 0;
		}

	}
}

#endif