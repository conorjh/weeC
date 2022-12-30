#ifndef WC_TEST_H
#define WC_TEST_H
#include <string>
#include <vector>

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
			std::string list_tokenizer5 = "string TestIdent = \"Hello World\"";
			std::string list_tokenizer6 = "- + += * / + ( ) { } [ ] # \\ / % & ^ ? |";
			std::string list_tokenizer7 = "int main(int argc, char* argv[])\n{\nreturn 0;\n}";
			std::string list_tokenizer8 = "// comment  \n /* multi \n  line \n comment */";
			std::string list_tokenizer9 = "// comment  \n /* multi \n  line \n comment */\n with extra line //then comment at the end";

			std::string list_tokenizer10 = "int IdentTest; char IdentTest::WithNamespace";
			std::string list_tokenizer11 = "x = 88 + (42 / 99)";
			std::string list_tokenizer12 = "ident.withMember ident::withNamespace::withObject.withMember";

			std::string list_parser1 = "int a;";
			std::string list_parser2 = "float a";
			std::string list_parser3 = "int a = 2;";
			std::string list_parser4 = "int a = (22 + 6);";
			std::string list_parser5 = "int a; int b = a * 2;";
			std::string list_parser6 = "int a = 2; int b = a * 2;";
			std::string list_parser7 = " int a; int a;";	//error
			std::string list_parser8 = " a * b;";			//error
			std::string list_parser9 = "2.5";
			std::string list_parser10 = "if(1 == 10-9)\n{\nint a;\n}";
			std::string list_parser11 = "if(1 == 10-9)\n{\n\tint a;\n}\nelse\n{\n\tint b;\n}";
			std::string list_parser12 = "if(1 == 10-9)\n\tint a;";
			std::string list_parser13 = "if(1 == 10-9\n\tint a;";	//error
			std::string list_parser14 = "if(1 == 10-9)\n{\n\tint a;\n\nelse\n{\n\tint b;\n} "; //error
			std::string list_parser15 = "if(1)\n\t55+55;\nelse\n\t66+ +66;";
			std::string list_parser16 = "if(1) if(1) if(1) 123;";
			std::string list_parser17 = "return";	//error
			std::string list_parser18 = "return;";
			std::string list_parser19 = "return 2+2;";
			std::string list_parser20 = "int a = 123; return a;";
			std::string list_parser21 = "int a = 123; \nif(a < 100)\n\treturn a;\nelse\nreturn a * 2;";
			std::string list_parser22 = "10/4";
			std::string list_parser23 = "5/3";
			std::string list_parser24 = "3 + 8/5 -1 -2*5";
			std::string list_parser25 = "  6 + c";
			std::string list_parser26 = "  7 & 2";
			std::string list_parser27 = "  %";
			std::string list_parser28 = " 5 + + 6";
			std::string list_parser29 = " -5 + 2";
			std::string list_parser30 = "5/1";


			std::string list_expression1 = "2 + 3;";
			std::string list_expression2 = "2 * 3;";
			std::string list_expression3 = "89;";
			std::string list_expression4 = "   12        -  8   ;";
			std::string list_expression5 = "142        -9   ;";
			std::string list_expression6 = "72+  15;";
			std::string list_expression7 = " 12*  4;";
			std::string list_expression8 = " 50/10;";
			std::string list_expression9 = "2.5;";
			std::string list_expression10 = "4*2.5 + 8.5+1.5 / 3.0;";
			std::string list_expression11 = "5.0005 + 0.0095;";
			std::string list_expression12 = "67+2;";
			std::string list_expression13 = " 2-7;";
			std::string list_expression14 = "5*7 ;";
			std::string list_expression15 = "8/4;";
			std::string list_expression16 = "2 -4 +6 -1 -1- 0 +8;";
			std::string list_expression17 = "1 -1   + 2   - 2   +  4 - 4 +    6;";
			std::string list_expression18 = "2 -4 +6 -1 -1- 0 +8;";
			std::string list_expression19 = "1 -1   + 2   - 2   +  4 - 4 +    6;";
			std::string list_expression20 = " 2*3 - 4*5 + 6/3 ;";
			std::string list_expression21 = "2*3*4/8 -   5/2*4 +  6 + 0/3   ;";
			std::string list_expression22 = "10/4;";
			std::string list_expression23 = "5/3;";
			std::string list_expression24 = "3 + 8/5 -1 -2*5;";
			std::string list_expression25 = "  6 + c;";
			std::string list_expression26 = "  7 & 2;";
			std::string list_expression27 = "  %;";
			std::string list_expression28 = " 5 + + 6;";
			std::string list_expression29 = " -5 + 2;";
			std::string list_expression30 = "5/1;";
			std::string list_expression31 = " 2 - 1 + 14/1 + 7;";
			std::string list_expression32 = "(2);";
			std::string list_expression33 = "(5 + 2*3 - 1 + 7 * 8);";
			std::string list_expression34 = "(67 + 2 * 3 - 67 + 2/1 - 7);";
			std::string list_expression35 = "(2) + (17*2-30) * (5)+2 - (8/2)*4;";
			std::string list_expression36 = "(5*7/5) + (23) - 5 * (98-4)/(6*7-42);";
			std::string list_expression37 = "(((((5)))));";
			std::string list_expression38 = "(( ((2)) + 4))*((5));";
			std::string list_expression39 = "2 + (5 * 2;";
			std::string list_expression40 = "(((((4))));";
			std::string list_expression41 = "((2)) * ((3;";
			std::string list_expression42 = "((9)) * ((1);";
			std::string list_expression43 = "1 == +(-1) == 1.00;";
			std::string list_expression44 = "+(-1) * 10;";
			std::string list_expression45 = "!2;";
			std::string list_expression46 = "22 && (11 + 11);";
			std::string list_expression47 = "0 || (33 + 33);";
			std::string list_expression48 = "+(-2) * 10;";
			std::string list_expression49 = "!!!2;";
			std::string list_expression50 = "!!!!(123);";

			std::string list_stringtokenizer4 = "Hello\nWorld\nLonger sentence\nFinishing with newline\n";
		}

		namespace lex
		{

			int Test_Tokenizer1(), Test_Tokenizer2(), Test_Tokenizer3(), Test_Tokenizer4(), Test_Tokenizer5(),
				Test_Tokenizer6(), Test_Tokenizer7(), Test_Tokenizer8(), Test_Tokenizer9(), Test_Tokenizer10(),
				Test_Tokenizer11(), Test_Tokenizer12();

			int Test_StringTokenizer1();

			int Test_wcParser_1(), Test_wcParser_2(), Test_wcParser_3(), Test_wcParser_4(), Test_wcParser_5(),
				Test_wcParser_6(), Test_wcParser_7(), Test_wcParser_8(), Test_wcParser_9(), Test_wcParser_10(),
				Test_wcParser_11(), Test_wcParser_12(), Test_wcParser_13(), Test_wcParser_14(), Test_wcParser_15(),
				Test_wcParser_16(), Test_wcParser_17(), Test_wcParser_18(), Test_wcParser_19(), Test_wcParser_20(),
				Test_wcParser_21(), Test_wcParser_22(), Test_wcParser_23(), Test_wcParser_24(), Test_wcParser_25(),
				Test_wcParser_26(), Test_wcParser_27(), Test_wcParser_28(), Test_wcParser_29(), Test_wcParser_30();

			int Test_wcExpressionParser_1(), Test_wcExpressionParser_2(), Test_wcExpressionParser_3(), Test_wcExpressionParser_4(), Test_wcExpressionParser_5(),
				Test_wcExpressionParser_6(), Test_wcExpressionParser_7(), Test_wcExpressionParser_8(), Test_wcExpressionParser_9(), Test_wcExpressionParser_10(),
				Test_wcExpressionParser_11(), Test_wcExpressionParser_12(), Test_wcExpressionParser_13(), Test_wcExpressionParser_14(), Test_wcExpressionParser_15(),
				Test_wcExpressionParser_16(), Test_wcExpressionParser_17(), Test_wcExpressionParser_18(), Test_wcExpressionParser_19(), Test_wcExpressionParser_20(),
				Test_wcExpressionParser_21(), Test_wcExpressionParser_22(), Test_wcExpressionParser_23(), Test_wcExpressionParser_24(), Test_wcExpressionParser_25(),
				Test_wcExpressionParser_26(), Test_wcExpressionParser_27(), Test_wcExpressionParser_28(), Test_wcExpressionParser_29(), Test_wcExpressionParser_30(),
				Test_wcExpressionParser_31(), Test_wcExpressionParser_32(), Test_wcExpressionParser_33(), Test_wcExpressionParser_34(), Test_wcExpressionParser_35(),
				Test_wcExpressionParser_36(), Test_wcExpressionParser_37(), Test_wcExpressionParser_38(), Test_wcExpressionParser_39(), Test_wcExpressionParser_40(),
				Test_wcExpressionParser_41(), Test_wcExpressionParser_42(), Test_wcExpressionParser_43(), Test_wcExpressionParser_44(), Test_wcExpressionParser_45(),
				Test_wcExpressionParser_46(), Test_wcExpressionParser_47(), Test_wcExpressionParser_48(), Test_wcExpressionParser_49(), Test_wcExpressionParser_50();

			//int Test_wcParseNode_1(), Test_wcParseNode_2(), Test_wcParseNode_3(), Test_wcParseNode_4(), Test_wcParseNode_5(), Test_wcParseNode_6();

			std::vector<int(*)()> allLexTests =
			{
				Test_Tokenizer1  ,Test_Tokenizer2, Test_Tokenizer3, Test_Tokenizer4, Test_Tokenizer5,
				Test_Tokenizer6, Test_Tokenizer7, Test_Tokenizer8, Test_Tokenizer9, Test_Tokenizer10, Test_Tokenizer11, Test_Tokenizer12,

				Test_StringTokenizer1

			};
			int Test_AllLex();

			std::vector<int(*)()> allParseTests =
			{
				//Test_wcParseNode_1, Test_wcParseNode_2, Test_wcParseNode_3, Test_wcParseNode_4, Test_wcParseNode_5, Test_wcParseNode_6,

				Test_wcParser_1, Test_wcParser_2, Test_wcParser_3, Test_wcParser_4, Test_wcParser_5,
				Test_wcParser_6, Test_wcParser_7, Test_wcParser_8, Test_wcParser_9, Test_wcParser_10,
				Test_wcParser_11, Test_wcParser_12, Test_wcParser_13, Test_wcParser_14, Test_wcParser_15,
				Test_wcParser_16, Test_wcParser_17, Test_wcParser_18, Test_wcParser_19, Test_wcParser_20,
				Test_wcParser_21, Test_wcParser_22, Test_wcParser_23, Test_wcParser_24, Test_wcParser_25,
				Test_wcParser_26, Test_wcParser_27, Test_wcParser_28, Test_wcParser_29, Test_wcParser_30,

				Test_wcExpressionParser_1, Test_wcExpressionParser_2, Test_wcExpressionParser_3, Test_wcExpressionParser_4, Test_wcExpressionParser_5,
				Test_wcExpressionParser_6, Test_wcExpressionParser_7, Test_wcExpressionParser_8, Test_wcExpressionParser_9, Test_wcExpressionParser_10,
				Test_wcExpressionParser_11, Test_wcExpressionParser_12, Test_wcExpressionParser_13, Test_wcExpressionParser_14, Test_wcExpressionParser_15,
				Test_wcExpressionParser_16, Test_wcExpressionParser_17, Test_wcExpressionParser_18, Test_wcExpressionParser_19, Test_wcExpressionParser_20,
				Test_wcExpressionParser_21, Test_wcExpressionParser_22, Test_wcExpressionParser_23, Test_wcExpressionParser_24, Test_wcExpressionParser_25,
				Test_wcExpressionParser_26, Test_wcExpressionParser_27, Test_wcExpressionParser_28, Test_wcExpressionParser_29, Test_wcExpressionParser_30,
				Test_wcExpressionParser_31, Test_wcExpressionParser_32, Test_wcExpressionParser_33, Test_wcExpressionParser_34, Test_wcExpressionParser_35,
				Test_wcExpressionParser_36, Test_wcExpressionParser_37, Test_wcExpressionParser_38, Test_wcExpressionParser_39, Test_wcExpressionParser_40,
				Test_wcExpressionParser_41, Test_wcExpressionParser_42, Test_wcExpressionParser_43, Test_wcExpressionParser_44, Test_wcExpressionParser_45,
				Test_wcExpressionParser_46, Test_wcExpressionParser_47, Test_wcExpressionParser_48, Test_wcExpressionParser_49, Test_wcExpressionParser_50
			};
			int Test_AllParse();
			
		}

		namespace parse
		{


		}
	}
}

#endif