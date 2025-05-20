#include "test.h"
#include "lex.h"
#include "parse.h"
#include "interpreter.h"
#include "tree_util.hh"
#include <string>
#include <vector>
#include <iostream>
#include <chrono>

using namespace std;

using namespace std;
using namespace weec;
using namespace weec::lex;
using namespace weec::parse;
using namespace weec::interpreter;
using weec::lex::wcToken;

namespace weec
{
	namespace lex
	{

	}
}

void printTree(tree<wcParseNode> pTree)
{
	auto it = pTree.begin();

	while (it != pTree.end())
	{
		for (int t = 0; t < pTree.depth(it); t++)
			cout << " - ";
		cout << it->Token.StringToken.Data << " (" << to_string(it->Type) << ")" << endl;
		it++;
	}
}

int main(int argc, char* argv[])
{
	weec::test::lex::Test_AllLex();
	weec::test::lex::Test_AllParse();
	cin;
}

int weec::test::lex::Test_AllLex()
{
	cout << "Running lex tests..." << endl;
	for (int t = 0; t < allLexTests.size(); ++t)
	{
		int testResult = allLexTests[t]();
		if (testResult)
			cout << "Test #" << t + 1 << " failed: " << testResult << endl;
		else
			cout << "Test #" << t + 1 << " passed" << endl;
	}

	return 0;
}

int weec::test::lex::Test_AllParse()
{
	cout << "Running parse tests..." << endl;
	for (int t = 0; t < allParseTests.size(); ++t)
	{
		int testResult = allParseTests[t]();
		if (testResult)
			cout << "\tTest #" << t + 1 << " failed: " << testResult << endl;
		else
			cout << "\tTest #" << t + 1 << " passed" << endl;
	}

	return 0;
}

int weec::test::lex::Test_wcParseScope()
{
	wcParseScope Scope("Test");
	if (Scope.Name != "Test")		return 1;
	if (Scope.Container.size())		return 2;

	wcParseScope Scope2("Namespace::Test", { "Symbol1", "Symbol2", "Symbol3" });
	if (Scope2.Name != "Namespace::Test")	return 1;
	if (!Scope2.Exists("Symbol2"))			return 2;
	if (!Scope2.Exists("Symbol2"))			return 3;
	if (!Scope2.Exists("Symbol3"))			return 4;
	if (Scope2.Container.size() != 3)		return 5;

	return 0;
}

int weec::test::lex::Test_wcParseScopes()
{
	wcParseScopes Scopes;
	if (Scopes.Size() != 1)										return 1;
	if (Scopes.Top().Exists("void") != true)					return 2;
	if (Scopes.Top().Exists("int") != true)						return 2;
	if (Scopes.Top().Exists("uint") != true)					return 3;
	if (Scopes.Top().Exists("float") != true)					return 4;
	if (Scopes.Top().Exists("double") != true)					return 5;
	if (Scopes.Top().Exists("bool") != true)					return 6;
	if (Scopes.Top().Exists("string") != true)					return 7;
	if (Scopes.Top().Count() != ParserConsts.BasicTypeCount)	return 8;
	if (Scopes.Size() != 1)				return 2;

	Scopes.Push(wcParseScope("test"));
	if (Scopes.Size() != 2)										return 9;
	if (Scopes.Top().Count() != 0 )	return 10;
	if (Scopes.Top().Name != "test")							return 11;

	Scopes.AddSymbol("testSymbol");
	if (Scopes.Size() != 2)											return 12;
	if (Scopes.Top().Count() != 1)	return 13;
	if (!Scopes.Top().Exists("testSymbol"))							return 14;


		

	return 0;
}




string AnyToString(any In)
{
	if (strcmp(In.type().name(), "int") == 0)
		return to_string(any_cast<int>(In));
	else if (strcmp(In.type().name(), "unsigned int") == 0)
		return to_string(any_cast<unsigned int>(In));
	else if (strcmp(In.type().name(), "float") == 0)
		return to_string(any_cast<float>(In));
	else if (strcmp(In.type().name(), "double") == 0)
		return to_string(any_cast<double>(In));
	else if (strcmp(In.type().name(), "bool") == 0)
		return to_string(any_cast<bool>(In));
	return "";
}

bool printIsExpected(wcInterpreter Interp, any ExpectedResult)
{
	if (Interp.Error.Code != wcInterpreterErrorCode::None)
		cout << "Interp error: " << to_string((int)Interp.Error.Code) << " " << to_string(Interp.Error.Code) << endl;

	bool IsExpected = false;
	if (strcmp(Interp.Return.type().name(), "int") == 0)
	{
		if (ExpectedResult.has_value())
		{
			IsExpected = any_cast<int>(Interp.Return) == any_cast<int>(ExpectedResult);
			cout << "Expected: " << any_cast<int>(ExpectedResult) << "   ";
		}
		cout << "Out: " << any_cast<int>(Interp.Return) << endl;
	}
	else if (strcmp(Interp.Return.type().name(), "unsigned int") == 0)
	{
		if (ExpectedResult.has_value())
		{
			IsExpected = any_cast<unsigned int>(Interp.Return) == any_cast<int>(ExpectedResult);
			cout << "Expected: " << any_cast<unsigned int>(ExpectedResult) << "   ";
		}
		cout << "Out: " << any_cast<unsigned int>(Interp.Return) << endl;
	}
	else if (strcmp(Interp.Return.type().name(), "float") == 0)
	{
		if (ExpectedResult.has_value())
		{
			IsExpected = any_cast<float>(Interp.Return) == any_cast<float>(ExpectedResult);
			cout << "Expected: " << any_cast<float>(ExpectedResult) << "   ";
		}
		cout << "Out: " << any_cast<float>(Interp.Return) << endl;
	}
	else if (strcmp(Interp.Return.type().name(), "double") == 0)
	{
		if (ExpectedResult.has_value())
		{
			IsExpected = any_cast<double>(Interp.Return) == any_cast<double>(ExpectedResult);
			cout << "Expected: " << any_cast<double>(ExpectedResult) << "   ";
		}
		cout << "Out: " << any_cast<double>(Interp.Return) << endl;
	}
	else if (strcmp(Interp.Return.type().name(), "bool") == 0)
	{
		if (ExpectedResult.has_value())
		{
			//IsExpected = any_cast<bool>(Interp.Return) == any_cast<bool>(ExpectedResult);
//			cout << "Expected: " << any_cast<bool>(ExpectedResult) << "   ";
		}
		//cout << "Out: " << any_cast<bool>(Interp.Return) << endl;
	}
	cout << "EAX: " << AnyToString(Interp.EAX) << endl;
	auto t = Interp.Return.type().name();
	cout << "Type: " << Interp.Return.type().name() << endl;
	return IsExpected;
}

int Test_ParserTemplate(string Listing)
{
	wcTokenizer tokenizer(Listing); //tokenizer.NextToken();

	auto parseStart = chrono::system_clock::now();
	auto Parsed = wcParser(tokenizer).Parse();
	auto parseEnd = chrono::system_clock::now();

	cout << endl << endl << Listing << endl;

	printTree(Parsed.AST);

	cout << "Parse time: " << chrono::duration_cast<chrono::milliseconds>(parseEnd - parseStart).count() << "ms" << endl;
	cout << endl << "Error code: " << (int)Parsed.Error.Code << "  " << to_string(Parsed.Error.Code) << endl;
	if (Parsed.Error.Code != wcParserErrorCode::None)
	{
		return (int)Parsed.Error.Code;
	}

	auto execStart = chrono::system_clock::now();
	auto Interp = wcInterpreter(Parsed);
	auto Result = Interp.Exec();
	auto execEnd = chrono::system_clock::now();
	
	cout << "Exec time: " << chrono::duration_cast<chrono::milliseconds>(execEnd - execStart).count() << "ms" << endl;
	return (Parsed.Error.Code == wcParserErrorCode::None && printIsExpected(Interp, any())) ? 0 : 1;
}


int Test_ExpressionParserTemplate(string Listing, any ExpectedResult)
{
	wcTokenizer tokenizer(Listing); //tokenizer.NextToken();

	auto Expr = wcParser(tokenizer).Parse();

	cout << endl << endl << Listing << endl;

	printTree(Expr.AST);

	if (Expr.Error.Code != wcParserErrorCode::None)
	{
		cout << endl << "Error code: " << (int)Expr.Error.Code << "  " << to_string(Expr.Error.Code) << endl;
		return (int)Expr.Error.Code;
	}
	auto Interp = wcInterpreter(Expr);
	auto Result = Interp.Exec();

	return (Expr.Error.Code == wcParserErrorCode::None && printIsExpected(Interp, ExpectedResult)) ? 0 : 1;
}

int weec::test::lex::Test_wcExpressionParser_1()
{
	return Test_ExpressionParserTemplate(listing::list_expression1, 5);
}

int weec::test::lex::Test_wcExpressionParser_60()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_59()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_58()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_57()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_56()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_55()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_54()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_53()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_52()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_51()
{
	return Test_ExpressionParserTemplate(listing::list_expression51, 0);
}

int weec::test::lex::Test_wcExpressionParser_2()
{
	return Test_ExpressionParserTemplate(listing::list_expression2, 6);
}

int weec::test::lex::Test_wcParser_30()
{
	return Test_ParserTemplate(listing::list_parser30);
}

int weec::test::lex::Test_wcParser_29()
{
	return Test_ParserTemplate(listing::list_parser29);
}

int weec::test::lex::Test_wcParser_28()
{
	return Test_ParserTemplate(listing::list_parser28); 
}

int weec::test::lex::Test_wcParser_27()
{
	return Test_ParserTemplate(listing::list_parser27);
}

int weec::test::lex::Test_wcParser_26()
{
	return Test_ParserTemplate(listing::list_parser26);
}

int weec::test::lex::Test_wcParser_25()
{
	return Test_ParserTemplate(listing::list_parser25);
}

int weec::test::lex::Test_wcParser_24()
{
	return Test_ParserTemplate(listing::list_parser24);
}

int weec::test::lex::Test_wcParser_23()
{
	return Test_ParserTemplate(listing::list_parser23);
}

int weec::test::lex::Test_wcParser_22()
{
	return Test_ParserTemplate(listing::list_parser22);
}

int weec::test::lex::Test_wcParser_21()
{
	return Test_ParserTemplate(listing::list_parser21);
}
int weec::test::lex::Test_wcParser_20()
{
	return Test_ParserTemplate(listing::list_parser20);
}

int weec::test::lex::Test_wcParser_19()
{
	return Test_ParserTemplate(listing::list_parser19);
}

int weec::test::lex::Test_wcParser_18()
{
	return Test_ParserTemplate(listing::list_parser18);
}

int weec::test::lex::Test_wcParser_17()
{
	return Test_ParserTemplate(listing::list_parser17);
}

int weec::test::lex::Test_wcParser_16()
{
	return Test_ParserTemplate(listing::list_parser16);
}

int weec::test::lex::Test_wcParser_15()
{
	return Test_ParserTemplate(listing::list_parser15);
}

int weec::test::lex::Test_wcParser_14()
{
	return Test_ParserTemplate(listing::list_parser14);
}

int weec::test::lex::Test_wcParser_13()
{
	return Test_ParserTemplate(listing::list_parser13);
}

int weec::test::lex::Test_wcParser_12()
{
	return Test_ParserTemplate(listing::list_parser12);
}

int weec::test::lex::Test_wcParser_11()
{
	return Test_ParserTemplate(listing::list_parser11);
}

int weec::test::lex::Test_wcParser_10()
{
	return Test_ParserTemplate(listing::list_parser10);
}

int weec::test::lex::Test_wcParser_9()
{
	return Test_ParserTemplate(listing::list_parser9);
}

int weec::test::lex::Test_wcParser_8()
{
	return Test_ParserTemplate(listing::list_parser8);
}

int weec::test::lex::Test_wcParser_7()
{
	return Test_ParserTemplate(listing::list_parser7);
}

int weec::test::lex::Test_wcParser_6()
{
	return Test_ParserTemplate(listing::list_parser6);
}

int weec::test::lex::Test_wcParser_5()
{
	return Test_ParserTemplate(listing::list_parser5);
}

int weec::test::lex::Test_wcParser_4()
{
	return Test_ParserTemplate(listing::list_parser4);
}

int weec::test::lex::Test_wcParser_3()
{
	return Test_ParserTemplate(listing::list_parser3);
}

int weec::test::lex::Test_wcParser_2()
{
	return Test_ParserTemplate(listing::list_parser2);
}

int weec::test::lex::Test_wcParser_1()
{
	return Test_ParserTemplate(listing::list_parser1);
}

int weec::test::lex::Test_wcParser_33()
{
	return Test_ParserTemplate(listing::list_parser33);
}

int weec::test::lex::Test_wcParser_32()
{
	return Test_ParserTemplate(listing::list_parser32);
}

int weec::test::lex::Test_Fibonacci()
{
	return Test_ParserTemplate(listing::list_fibonacci);
}

int weec::test::lex::Test_wcParser_31()
{
	return Test_ParserTemplate(listing::list_parser31);
}

int weec::test::lex::Test_wcExpressionParser_3()
{
	return Test_ExpressionParserTemplate(listing::list_expression3, 89);
}

int weec::test::lex::Test_wcExpressionParser_4()
{
	return Test_ExpressionParserTemplate(listing::list_expression4, 4);
}

int weec::test::lex::Test_wcExpressionParser_5()
{
	return Test_ExpressionParserTemplate(listing::list_expression5, 133);
}

int weec::test::lex::Test_wcExpressionParser_6()
{
	return Test_ExpressionParserTemplate(listing::list_expression6, 87);
}

int weec::test::lex::Test_wcExpressionParser_7()
{
	return Test_ExpressionParserTemplate(listing::list_expression7, 48);
}

int weec::test::lex::Test_wcExpressionParser_8()
{
	return Test_ExpressionParserTemplate(listing::list_expression8, 5);
}

int weec::test::lex::Test_wcExpressionParser_9()
{
	return Test_ExpressionParserTemplate(listing::list_expression9, (float)2.5);
}

int weec::test::lex::Test_wcExpressionParser_10()
{
	return Test_ExpressionParserTemplate(listing::list_expression10, (float)(4 * 2.5 + 8.5 + 1.5 / 3.0));
}

int weec::test::lex::Test_wcExpressionParser_11()
{
	return Test_ExpressionParserTemplate(listing::list_expression11, (float)(5.0005 + 0.0095));
}

int weec::test::lex::Test_wcExpressionParser_12()
{
	return Test_ExpressionParserTemplate(listing::list_expression12, 69);
}

int weec::test::lex::Test_wcExpressionParser_13()
{
	return Test_ExpressionParserTemplate(listing::list_expression13, -5);
}

int weec::test::lex::Test_wcExpressionParser_14()
{
	return Test_ExpressionParserTemplate(listing::list_expression14, 35);
}

int weec::test::lex::Test_wcExpressionParser_15()
{
	return Test_ExpressionParserTemplate(listing::list_expression15, 2);
}
int weec::test::lex::Test_wcExpressionParser_16()
{
	return Test_ExpressionParserTemplate(listing::list_expression16, 10);
}
int weec::test::lex::Test_wcExpressionParser_17()
{
	return Test_ExpressionParserTemplate(listing::list_expression17, 6);
}
int weec::test::lex::Test_wcExpressionParser_18()
{
	return Test_ExpressionParserTemplate(listing::list_expression18, 10);
}
int weec::test::lex::Test_wcExpressionParser_19()
{
	return Test_ExpressionParserTemplate(listing::list_expression19, 6);
}

int weec::test::lex::Test_wcExpressionParser_20()
{
	return Test_ExpressionParserTemplate(listing::list_expression20, (2 * 3 - 4 * 5 + 6 / 3));
}

int weec::test::lex::Test_wcExpressionParser_21()
{
	return Test_ExpressionParserTemplate(listing::list_expression21, (2 * 3 * 4 / 8 - 5 / 2 * 4 + 6 + 0 / 3));
}

int weec::test::lex::Test_wcExpressionParser_22()
{
	return Test_ExpressionParserTemplate(listing::list_expression22, (10 / 4));
}

int weec::test::lex::Test_wcExpressionParser_23()
{
	return Test_ExpressionParserTemplate(listing::list_expression23, (5 / 3));
}

int weec::test::lex::Test_wcExpressionParser_24()
{
	return Test_ExpressionParserTemplate(listing::list_expression24, (3 + 8 / 5 - 1 - 2 * 5));
}

int weec::test::lex::Test_wcExpressionParser_25()
{
	return Test_ExpressionParserTemplate(listing::list_expression25, 0);
}
int weec::test::lex::Test_wcExpressionParser_26()
{
	return Test_ExpressionParserTemplate(listing::list_expression26, 0);
}
int weec::test::lex::Test_wcExpressionParser_27()
{
	return Test_ExpressionParserTemplate(listing::list_expression27, 0);
}
int weec::test::lex::Test_wcExpressionParser_28()
{
	return Test_ExpressionParserTemplate(listing::list_expression28, (5 + +6));
}
int weec::test::lex::Test_wcExpressionParser_29()
{
	return Test_ExpressionParserTemplate(listing::list_expression29, (-5 + 2));
}

int weec::test::lex::Test_wcExpressionParser_30()
{
	return Test_ExpressionParserTemplate(listing::list_expression30, 5 / 1);
}

int weec::test::lex::Test_wcExpressionParser_31()
{
	return Test_ExpressionParserTemplate(listing::list_expression31, 2 - 1 + 14 / 1 + 7);
}

int weec::test::lex::Test_wcExpressionParser_32()
{
	return Test_ExpressionParserTemplate(listing::list_expression32, (2));
}

int weec::test::lex::Test_wcExpressionParser_33()
{
	return Test_ExpressionParserTemplate(listing::list_expression33, (5 + 2 * 3 - 1 + 7 * 8));
}

int weec::test::lex::Test_wcExpressionParser_34()
{
	return Test_ExpressionParserTemplate(listing::list_expression34, (67 + 2 * 3 - 67 + 2 / 1 - 7));
}

int weec::test::lex::Test_wcExpressionParser_35()
{
	return Test_ExpressionParserTemplate(listing::list_expression35, ((2) + (17 * 2 - 30) * (5) + 2 - (8 / 2) * 4));
}
int weec::test::lex::Test_wcExpressionParser_36()
{
	return Test_ExpressionParserTemplate(listing::list_expression36, ((5 * 7 / 5) + (23) - 5 * (98 - 4) / (6 * 7 - 42)));
}
int weec::test::lex::Test_wcExpressionParser_37()
{
	return Test_ExpressionParserTemplate(listing::list_expression37, (((((5))))));
}
int weec::test::lex::Test_wcExpressionParser_38()
{
	return Test_ExpressionParserTemplate(listing::list_expression38, ((((2)) + 4)) * ((5)));
}
int weec::test::lex::Test_wcExpressionParser_39()
{
	return Test_ExpressionParserTemplate(listing::list_expression39, 0);
}
int weec::test::lex::Test_wcExpressionParser_40()
{
	return Test_ExpressionParserTemplate(listing::list_expression40, 0);
}
int weec::test::lex::Test_wcExpressionParser_41()
{
	return Test_ExpressionParserTemplate(listing::list_expression41, 0);
}
int weec::test::lex::Test_wcExpressionParser_42()
{
	return Test_ExpressionParserTemplate(listing::list_expression42, 0);
}
int weec::test::lex::Test_wcExpressionParser_43()
{
	return Test_ExpressionParserTemplate(listing::list_expression43, (1 == +(-1) == 1.00));
}
int weec::test::lex::Test_wcExpressionParser_44()
{
	return Test_ExpressionParserTemplate(listing::list_expression44, +(-1) * 10);
}
int weec::test::lex::Test_wcExpressionParser_45()
{
	return Test_ExpressionParserTemplate(listing::list_expression45, !2);
}
int weec::test::lex::Test_wcExpressionParser_46()
{
	return Test_ExpressionParserTemplate(listing::list_expression46, 22 && (11 + 11));
}
int weec::test::lex::Test_wcExpressionParser_47()
{
	return Test_ExpressionParserTemplate(listing::list_expression47, 0 || (33 + 33));
}
int weec::test::lex::Test_wcExpressionParser_48()
{
	return Test_ExpressionParserTemplate(listing::list_expression48, +(-2) * 10);
}
int weec::test::lex::Test_wcExpressionParser_49()
{
	return Test_ExpressionParserTemplate(listing::list_expression49, !!!2);
}
int weec::test::lex::Test_wcExpressionParser_50()
{
	return Test_ExpressionParserTemplate(listing::list_expression50, !!!!(123));
}

