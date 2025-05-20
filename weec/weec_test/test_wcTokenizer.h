#ifndef WC_TEST_WCTOKENIZER_H
#define WC_TEST_WCTOKENIZER_H
#include <string>
#include <vector>
#include "lex.h"

namespace weec
{
	namespace test
	{
		namespace listing
		{
			std::string list_tokenizer1 = "3.14";
			std::string list_tokenizer2 = "\"Hello World\"";
			std::string list_tokenizer3 = "'Char literal'";
			std::string list_tokenizer4 = "const int TestIdent =99 *44";
			std::string list_tokenizer5 = "string Test_Ident = \"Hello World\"";
			std::string list_tokenizer6 = "- + += * / + ( ) { } [ ] # \\ / % & ^ ? |";
			std::string list_tokenizer7 = "int main(int argc, char* argv[])\n{\nreturn 0;\n}";
			std::string list_tokenizer8 = "// comment  \n /* multi \n  line \n comment */";
			std::string list_tokenizer9 = "// comment  \n /* multi \n  line \n comment */\n with extra line //then comment at the end";
			std::string list_tokenizer10 = "int IdentTest; char IdentTest::WithNamespace";
			std::string list_tokenizer11 = "x = 88 + (42 / 99)";
			std::string list_tokenizer12 = "ident.withMember ident::withNamespace::withObject.withMember ns::method1() ns2::method2(a,b)";
		}

		namespace lex
		{
			int weec::test::Test_Tokenizer1()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer1;
				wcTokenizer tokenizer(input);

				if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::FloatLiteral)
					return 1;

				return 0;
			}

			int weec::test::Test_Tokenizer2()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer2;
				wcTokenizer tokenizer(input);

				if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::StringLiteral)
					return 1;

				return 0;
			}

			int weec::test::Test_Tokenizer3()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer3;
				wcTokenizer tokenizer(input);

				if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CharLiteral)
					return 1;

				return 0;
			}

			int weec::test::Test_Tokenizer4()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer4;
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

			int weec::test::Test_Tokenizer5()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer5;
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

			int weec::test::Test_Tokenizer6()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer6;
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

			int weec::test::Test_Tokenizer7()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer7;
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

			int weec::test::Test_Tokenizer8()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer8;
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

			int weec::test::Test_Tokenizer9()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer9;
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

			int weec::test::Test_Tokenizer10()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer10;
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

			int weec::test::Test_Tokenizer12()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer12;
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

			int weec::test::Test_Tokenizer11()
			{
				using namespace weec::lex;

				std::string input = listing::list_tokenizer11;
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
}

#endif