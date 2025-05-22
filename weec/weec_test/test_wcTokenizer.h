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
			if (tokenizer1.NextToken() || tokenizer1.GetToken().Type != wcTokenType::Whitespace)
				return 1;

			std::string input2(list_lexer_whitespace2);
			wcTokenizer tokenizer2(input2);
			if (tokenizer2.NextToken() || tokenizer2.GetToken().Type != wcTokenType::Whitespace)
				return 2;

			std::string input3(list_lexer_whitespace3);
			wcTokenizer tokenizer3(input3);
			if (!tokenizer3.NextToken() || tokenizer3.GetToken().Type != wcTokenType::Whitespace)
				return 3;

			std::string input4(list_lexer_whitespace4);
			wcTokenizer tokenizer4(input4);
			if (tokenizer4.NextToken() || tokenizer4.GetToken().Type != wcTokenType::Whitespace)
				return 4;

			std::string input5(list_lexer_whitespace5);
			wcTokenizer tokenizer5(input5);
			if (tokenizer5.NextToken() || tokenizer5.GetToken().Type != wcTokenType::Whitespace)
				return 5;

			std::string input6(list_lexer_whitespace6);
			wcTokenizer tokenizer6(input6);
			if (tokenizer6.NextToken() || tokenizer6.GetToken().Type != wcTokenType::Whitespace)
				return 6;

			return 0;
		}

		inline int Test_Tokenizer_102()
		{
			using namespace weec::lex;

			std::string input = listing::list_tokenizer102;
			wcTokenizer tokenizer(input);

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::StringLiteral)
				return 1;

			return 0;
		}

		inline int Test_Tokenizer_103()
		{
			using namespace weec::lex;

			std::string input = listing::list_tokenizer103;
			wcTokenizer tokenizer(input);

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CharLiteral)
				return 1;

			return 0;
		}

		inline int Test_Tokenizer_104()
		{
			using namespace weec::lex;

			std::string input = listing::list_tokenizer104;
			wcTokenizer tokenizer(input);

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::ConstKeyword && tokenizer.GetToken().StringToken.Data == "const")
				return 1;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
				return 2;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier || tokenizer.GetToken().StringToken.Data != "TestIdent")
				return 3;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::AssignOperator)
				return 4;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
				return 5;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MultiplyOperator)
				return 6;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
				return 7;

			return 0;
		}

		inline int Test_Tokenizer_105()
		{
			using namespace weec::lex;

			std::string input = listing::list_tokenizer105;
			wcTokenizer tokenizer(input);

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::StringKeyword || tokenizer.GetToken().StringToken.Data != "string")
				return 1;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier || tokenizer.GetToken().StringToken.Data != "Test_Ident")
				return 2;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::AssignOperator)
				return 3;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::StringLiteral)
				return 4;

			return 0;
		}

		inline int Test_Tokenizer_106()
		{
			using namespace weec::lex;

			std::string input = listing::list_tokenizer106;
			wcTokenizer tokenizer(input);

			//- + += * / + ( ) { } [ ] # \\ / % & ^ ? |

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MinusOperator)
				return 1;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusOperator)
				return 2;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusAssignOperator)
				return 3;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MultiplyOperator)
				return 4;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::DivideOperator)
				return 5;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusOperator)
				return 6;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
				return 7;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
				return 8;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBrace)
				return 9;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBrace)
				return 10;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBracket)
				return 11;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBracket)
				return 12;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Hash)
				return 13;

			return 0;
		}

		inline int Test_Tokenizer_107()
		{
			using namespace weec::lex;

			std::string input = listing::list_tokenizer107;
			wcTokenizer tokenizer(input);

			//int main(int argc, char* argv[])
			//{
			//	return 0;
			//}

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
				return 1;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
				return 2;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
				return 3;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
				return 4;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
				return 5;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Comma)
				return 6;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CharKeyword)
				return 7;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MultiplyOperator)
				return 8;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
				return 9;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBracket)
				return 10;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBracket)
				return 11;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
				return 12;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBrace)
				return 13;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::ReturnKeyword)
				return 14;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
				return 15;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::SemiColon)
				return 16;

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBrace)
				return 17;

			return 0;
		}

		inline int Test_Tokenizer_108()
		{
			using namespace weec::lex;

			std::string input = listing::list_tokenizer108;
			wcTokenizer tokenizer(input);

			//  // comment  
			//  /* multi 
			//  line 
			//  comment */
			//	with extra line //then comment at the end

			if (tokenizer.NextToken())
				return 1;

			return 0;
		}

		inline int Test_Tokenizer_109()
		{
			using namespace weec::lex;

			std::string input = listing::list_tokenizer109;
			wcTokenizer tokenizer(input);

			//  // comment  
			//  /* multi 
			//  line 
			//  comment */
			//	with extra line //then comment at the end


			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
				return 1;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
				return 2;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
				return 3;

			if (tokenizer.NextToken())
				return 4;

			return 0;
		}

		inline int Test_Tokenizer_110()
		{
			using namespace weec::lex;

			std::string input = listing::list_tokenizer110;
			wcTokenizer tokenizer(input);

			//  int IdentTest; char IdentTest::WithNamespace

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
				return 1;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
				return 2;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::SemiColon)
				return 3;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CharKeyword)
				return 4;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier || tokenizer.GetToken().StringToken.Data != "IdentTest::WithNamespace")
				return 5;

			return 0;
		}

		inline int Test_Tokenizer_112()
		{
			using namespace weec::lex;

			std::string input = listing::list_tokenizer112;
			wcTokenizer tokenizer(input);

			// ident.withMember ident::withNamespace::withObject.withMember ns::method1() ns2::method2(a,b)

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier && tokenizer.GetToken().StringToken.Data == "ident.withMember")
				return 1;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier && tokenizer.GetToken().StringToken.Data == "ident::withNamespace::withObject.withMember")
				return 2;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier && tokenizer.GetToken().StringToken.Data == "ns::method1")
				return 3;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
				return 4;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
				return 5;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier && tokenizer.GetToken().StringToken.Data == "ns::method2")
				return 6;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
				return 7;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier && tokenizer.GetToken().StringToken.Data == "a")
				return 8;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Comma)
				return 7;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier && tokenizer.GetToken().StringToken.Data == "b")
				return 8;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
				return 9;

			return 0;
		}

		inline int Test_Tokenizer_111()
		{
			using namespace weec::lex;

			std::string input = listing::list_tokenizer111;
			wcTokenizer tokenizer(input);

			//x = 88 + (42 / 99)

			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
				return 1;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::AssignOperator)
				return 2;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
				return 3;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusOperator)
				return 4;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
				return 5;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
				return 6;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::DivideOperator)
				return 7;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
				return 8;
			if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
				return 9;

			return 0;
		}


	}
}

#endif