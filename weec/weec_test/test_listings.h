#ifndef WC_TEST_LISTINGS_H
#define WC_TEST_LISTINGS_H
#include <string>

namespace weec
{
	namespace test
	{
		namespace listing
		{
			//lexing
			const std::string list_lexer_arithmetic_operators = "+ - / * ^ % ++ -- = += -= *= /=";
			const std::string list_lexer_boolean_operators = "== != < > >= <= || &&";
			const std::string list_lexer_misc = "== != < > >= <= || &&";
			const std::string list_lexer_keywords =
				"true false string double int uint float bool char void object var namespace func if else while break continue return const print struct";

			const std::string list_lexer_whitespace1 = " ";					//space
			const std::string list_lexer_whitespace2 = "\t";				//tab
			const std::string list_lexer_whitespace3 = "\n";				//newline
			const std::string list_lexer_whitespace4 = "\t\n";				//tab newline
			const std::string list_lexer_whitespace5 = "\t\n \n";			//tab newline space newline
			const std::string list_lexer_whitespace6 = "    /t  /t /t ";	//3 space, tab, 2 space, tab, 1 space, tab, space

			const std::string list_lexer_comments1 = "//comment\n/* multi \n  line \n comment */";
			const std::string list_lexer_comments2 = "//comment\n/* multi \n  line \n comment */\n with extra line //then comment at the end";

			const std::string list_tokenizer101 = "3.14";
			const std::string list_tokenizer102 = "\"Hello World\"";
			const std::string list_tokenizer103 = "'Char literal'";
			const std::string list_tokenizer104 = "const int TestIdent =99 *44";
			const std::string list_tokenizer105 = "string Test_Ident = \"Hello World\"";
			const std::string list_tokenizer106 = "- + += * / + ( ) { } [ ] # \\ / % & ^ ? |";
			const std::string list_tokenizer107 = "int main(int argc, char* argv[])\n{\nreturn 0;\n}";
			const std::string list_tokenizer108 = "// comment  \n /* multi \n  line \n comment */";
			const std::string list_tokenizer109 = "// comment  \n /* multi \n  line \n comment */\n with extra line //then comment at the end";
			const std::string list_tokenizer110 = "int IdentTest; char IdentTest::WithNamespace";
			const std::string list_tokenizer111 = "x = 88 + (42 / 99)";
			const std::string list_tokenizer112 = "ident.withMember ident::withNamespace::withObject.withMember ns::method1() ns2::method2(a,b)";

			//declarations
			const std::string list_parser1 = "int a;";
			const std::string list_parser2 = "float a";
			const std::string list_parser3 = "int a = 2;";
			const std::string list_parser4 = "int a = (22 + 6);";
			const std::string list_parser5 = "int a; int b = a * 2;";
			const std::string list_parser6 = "int a = 2; int b = a * 2;";

			//declaration errors
			const std::string list_parser7 = " int a; int a;";	//error
			const std::string list_parser8 = " a * b;";			//error
			const std::string list_parser9 = "int a = 2;\n int b = a * 2;\n return a * b;";

			//if statements
			const std::string list_parser10 = "if(1 == 10-9)\n{\n return 999;\n}";
			const std::string list_parser11 = "if(1 != 10-9)\n{\n\t return 123;\n}\nelse\n{\n\t return 999;\n}";
			const std::string list_parser12 = "if(1 == 10-9)\n\treturn (1 == 10-9);\nreturn !(1 == 10-9);";
			const std::string list_parser13 = "if(1 == 10-9\n\tint a;";	//error
			const std::string list_parser14 = "if(1 == 10-9)\n{\n\tint a;\n\nelse\n{\n\tint b;\n} "; //error
			const std::string list_parser15 = "if(true)\n\t55+55;\nelse\n\t66+ +66;";
			const std::string list_parser16 = "if(true) if(true) if(true) 123;";

			//return statement
			const std::string list_parser17 = "return";	//error
			const std::string list_parser18 = "return;";
			const std::string list_parser19 = "return 2+2;";
			const std::string list_parser20 = "int a = 123; return a;";
			const std::string list_parser21 = "int a = 123; \nif(a < 100)\n\treturn a;\nelse\nreturn a * 2;";
			const std::string list_parser22 = "int t = 0;\nwhile(t<100)\n{\n\tt = t+1;\n}\nreturn t;";
			const std::string list_parser23 = "int a = 2;\n int b = 3;\n if (a < b)\n return 213123;	\n return 323444; ";
			const std::string list_parser24 = "int a = 1;\n\n//single line while loop \nwhile(a<100)\n\ta = a + 1;\nreturn a;";

			//function declarations
			const std::string list_parser25 = "int functionName()";
			const std::string list_parser26 = "int functionNameWithBody()\n{\n\treturn 123;\n}";
			const std::string list_parser27 = "int functionNameWithParams(int a, int b)\n{\n\treturn a * b;\n}";
			const std::string list_parser28 = "int functionName()\n{\n\treturn 2345;;\n}\n\nreturn functionName();";
			const std::string list_parser29 = "int functionNameWithParams(int a, int b)\n{\n\treturn a + b;\n}\nint functionNameWithParams2(int a, int b)\n{\n\treturn a + b;\n}\nreturn functionNameWithParams(2,3) + functionNameWithParams2(11,11);";
			const std::string list_parser30 = "int functionNameWithParams(int a, int b)\n{\n\treturn a + b;\n}\n\nfunctionNameWithParams(2,3);";
			const std::string list_parser31 = "int functionNameWithParamsAndRecursion(int a, int b)\n{\n\tif(a+b < 100)\n\t\treturn functionNameWithParamsAndRecursion(a + b, b);\n\telse\nreturn a + b;\n}\n\nreturn functionNameWithParamsAndRecursion(2,3);";
			const std::string list_parser32 =
				"int f1(int a, int b)\n"
				"{\n"
				"	return a + b;\n"
				"}\n"
				"int f1(int a)\n"
				"{\n"
				"	return a;\n"
				"}\n"
				"\n"
				"//overloads\n"
				"return f1(6) + f1(6,6);\n";
			const std::string list_parser33 =
				"struct s1\n"
				"{\n"
				"\tint member1;\n"
				"\n"
				"\tvoid set(int value)\n"
				"\t{\n"
				"\t\tmember1 = value;\n"
				"\t}\n"
				"\tint get()\n"
				"\t{\n"
				"\t\treturn member1;\n"
				"\t}\n"
				"};\n"
				"\n"
				"s1 testStruct;\n"
				"testStruct.member1 = 10;\n"
				"print testStruct.member1;\n"
				"testStruct.set(11);\n"
				"print testStruct.get();\n"
				"return testStruct.member1;\n";


			const std::string list_fibonacci =
				"int fibonacci(int num)\n"
				"{\n"
				"	if (num == 0)\n"
				"		return 0;\n"
				"\n"
				"	else if (num == 1)\n"
				"		return 1; \n"
				"\n"
				"	else\n"
				"		return fibonacci(num - 1) + fibonacci(num - 2);\n"
				"}\n"
				"\n"
				"int num = 20; // variable to store how many elements to be displayed in the series\n"
				"int i = 0;\n"
				"int ret = 0;\n"
				"while (i <= num)\n"
				"{\n"
				"	ret = fibonacci(i);\n"
				"	print i = i + 1;\n"
				"}\n"
				"return ret;";


			const std::string list_expression1 = "2 + 3;";
			const std::string list_expression2 = "2 * 3;";
			const std::string list_expression3 = "89;";
			const std::string list_expression4 = "   12        -  8   ;";
			const std::string list_expression5 = "142        -9   ;";
			const std::string list_expression6 = "72+  15;";
			const std::string list_expression7 = " 12*  4;";
			const std::string list_expression8 = " 50/10;";
			const std::string list_expression9 = "2.5;";
			const std::string list_expression10 = "4*2.5 + 8.5+1.5 / 3.0;";
			const std::string list_expression11 = "5.0005 + 0.0095;";
			const std::string list_expression12 = "67+2;";
			const std::string list_expression13 = " 2-7;";
			const std::string list_expression14 = "5*7 ;";
			const std::string list_expression15 = "8/4;";
			const std::string list_expression16 = "2 -4 +6 -1 -1- 0 +8;";
			const std::string list_expression17 = "1 -1   + 2   - 2   +  4 - 4 +    6;";
			const std::string list_expression18 = "2 -4 +6 -1 -1- 0 +8;";
			const std::string list_expression19 = "1 -1   + 2   - 2   +  4 - 4 +    6;";
			const std::string list_expression20 = " 2*3 - 4*5 + 6/3 ;";
			const std::string list_expression21 = "2*3*4/8 -   5/2*4 +  6 + 0/3   ;";
			const std::string list_expression22 = "10/4;";
			const std::string list_expression23 = "5/3;";
			const std::string list_expression24 = "3 + 8/5 -1 -2*5;";
			const std::string list_expression25 = "  6 + c;";
			const std::string list_expression26 = "  7 & 2;";
			const std::string list_expression27 = "  %;";
			const std::string list_expression28 = " 5 + + 6;";
			const std::string list_expression29 = " -5 + 2;";
			const std::string list_expression30 = "5/1;";
			const std::string list_expression31 = " 2 - 1 + 14/1 + 7;";
			const std::string list_expression32 = "(2);";
			const std::string list_expression33 = "(5 + 2*3 - 1 + 7 * 8);";
			const std::string list_expression34 = "(67 + 2 * 3 - 67 + 2/1 - 7);";
			const std::string list_expression35 = "(2) + (17*2-30) * (5)+2 - (8/2)*4;";
			const std::string list_expression36 = "(5*7/5) + (23) - 5 * (98-4)/(6*7-42);";
			const std::string list_expression37 = "(((((5)))));";
			const std::string list_expression38 = "(( ((2)) + 4))*((5));";
			const std::string list_expression39 = "2 + (5 * 2;";
			const std::string list_expression40 = "(((((4))));";
			const std::string list_expression41 = "((2)) * ((3;";
			const std::string list_expression42 = "((9)) * ((1);";
			const std::string list_expression43 = "1 == +(-1) == 1.00;";
			const std::string list_expression44 = "+(-1) * 10;";
			const std::string list_expression45 = "!2;";
			const std::string list_expression46 = "22 && (11 + 11);";
			const std::string list_expression47 = "0 || (33 + 33);";
			const std::string list_expression48 = "+(-2) * 10;";
			const std::string list_expression49 = "!!!2;";
			const std::string list_expression50 = "!!!!(123);";
			const std::string list_expression51 = "return 10 < 10 \n\t? 123 < 123 \n\t\t? 123 \n\t\t: 124 \n\t: 456 == 456 \n\t\t? 456 \n\t\t: 457;";
		}
	}
}

#endif