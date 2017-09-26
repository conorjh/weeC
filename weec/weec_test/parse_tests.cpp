#include "tests.h"
#include "parse.h"

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
	}
}

//compare two ASTS, 0 if theyre different
int wctest::parse::p_compare(wcAST p_correctAST, wcAST p_testAST)
{

	return 1;
}
//compare two ASTS, 0 if theyre different
int wctest::parse::standardParseTest(const char * p_source)
{
	wcLexer lexer;
	wcParser parser;

	wcAST testOutput = parser.parse(lexer.lex(p_source));
	wcAST correctOutput;

	if (parser.getError().code)
		return parser.getError().code;
	return 0;
}

//create a lexer, lex a simple string ,dispose of the lexer
int wctest::parse::p_basic_1()
{
	return standardParseTest("int a;");
}
int wctest::parse::p_basic_2()
{
	return standardParseTest("int fooBar = 2;");
}
int wctest::parse::p_basic_3()
{
	return standardParseTest("2 + / 55 * 88 / 99;");
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
		"a = 22 + + a;");
}