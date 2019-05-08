#include "tests.h"
#include "parse.h"
#include "simple.h"
#include <iostream>

using namespace wc;
using namespace wc::lex;
using namespace wc::parse;
using namespace std;

namespace wctest
{
	namespace parse
	{
		int p_compare(wcAST p_correctAST, wcAST p_testAST);
		int standardParseTest(const char *);
		int standardExpParseTest(const char *);
		void printAST(wcAST);
	}
}

//compare two ASTS, 0 if theyre different
int wctest::parse::p_compare(wcAST p_correctAST, wcAST p_testAST)
{

	return 1;
}

int wctest::parse::standardParseTest(const char * p_source)
{
	return wcParser().parse(wcLexer().lex(wcLexInputStream(p_source))).error.code;
}

int wctest::parse::standardExpParseTest(const char * p_source)
{
	auto lexOutput = wcLexer().lex(wcLexInputStream(p_source));
	auto parseOutput = wcParser().parse(lexOutput);

	//print the AST
	printAST(parseOutput.ast);

	return parseOutput.error.code;
}

void wctest::parse::printAST(wcAST ast)
{
	tree<wcParseNode>::iterator iter = ast.parseTree.begin();

	string tabbing = "";
	while (iter != ast.parseTree.end())
	{
		for (int t = 0; t < ast.parseTree.depth(iter); t = t + 2)
			tabbing += " ";

		cout << tabbing << wc::parse::pnTypeStrings.find(iter.node->data.type)->second << endl;

		iter++;
	}
}


//create a lexer, lex a simple string ,dispose of the lexer
int wctest::parse::p_basic_1()
{
	return standardExpParseTest("int a;");
}

int wctest::parse::p_basic_2()
{
	return standardExpParseTest("int fooBar = 2;");
}

int wctest::parse::p_basic_3()
{
	return standardParseTest("2 + 55 * 88 / 99;");
}

int wctest::parse::p_basic_4()
{
	return standardParseTest("int a = 1;"
		"a + 55;");
}

int wctest::parse::p_basic_5()
{
	return standardParseTest("int a = 22 + 22;"
		"a + 55 * (600 / 2 );");
}

int wctest::parse::p_basic_6()
{
	return standardParseTest("int a = 22 + 22;"
		"a = 22 + a;");
}

int wctest::parse::p_basic_7()
{
	return standardParseTest("namespace test"
		"{"
		"	int a;"
		"}");
}

int wctest::parse::p_basic_8()
{
	return standardParseTest("namespace outer"
		"{"
		"	namespace inner"
		"	{"
		"		int innerVar = 22;"
		"	}"
		"	int outerVar = 33;"
		"}");
}

int wctest::parse::p_basic_9()
{
	return standardParseTest(
		"namespace outer"
		"{"
		"	namespace inner"
		"	{"
		"		int innerVar = 22;"
		"		int innerMethod()"
		"		{"
		"			int innerMethodVar = 44;"
		"			return innerMethodVar;"
		"		}"
		"	}"
		"	int outerVar = 33;"
		"}");
}

//create a lexer, lex a simple string ,dispose of the lexer
int wctest::parse::p_exp_1()
{
	return standardExpParseTest("int a;");
}

int wctest::parse::p_exp_2()
{
	return standardExpParseTest("(22+33)");
}

int wctest::parse::p_exp_3()
{
	return standardExpParseTest("22+(33);");
}

int wctest::parse::p_exp_4()
{
	return standardExpParseTest("22*33;");
}

int wctest::parse::p_exp_5()
{
	return standardExpParseTest("22 * (33 + 44);");
}

int wctest::parse::p_exp_6()
{
	return standardExpParseTest("22 * (33 + (44));");
}

int wctest::parse::p_exp_7()
{
	return standardExpParseTest("22 * (33 + -(44));");
}

int wctest::parse::p_exp_8()
{
	return standardExpParseTest("(22 * (33 + -(44))) * 55;");
}

int wctest::parse::p_exp_9()
{
	return standardExpParseTest("22 || (33 < 44)");
}