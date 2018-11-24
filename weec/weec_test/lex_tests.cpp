#include "tests.h"
#include "lex.h"
#include "error.h"

using namespace wc;
using namespace wc::lex;
using namespace wc::error;
using namespace std;

namespace wctest
{
	namespace lex
	{
		int l_compare(vector<wcToken> p_correctVec, vector<wcToken> p_testVec);
	}
}

//compare two vectors of wcTokens, return 1 if theyre equivalent, 0 if not
int wctest::lex::l_compare(vector<wcToken> p_correctVec, vector<wcToken> p_testVec)
{
	for (int t = 0; t < p_testVec.size(); ++t)
		if (p_correctVec[t] != p_testVec[t])
			return 0;
	return 1;
}

//create a lexer, lex a simple string ,dispose of the lexer
int wctest::lex::l_basic_1()
{
	wcLexer lexer;

	vector<wcToken> lexOutput = lexer.lex("\"Hello World\"");

	vector<wcToken> correctOutput = { wcToken(tt_strlit, "Hello World") };

	if (lexer.getError() != ec_null)
		return !(l_compare(correctOutput, lexOutput));
	return lexer.getError().code;
}

//lex some integers
int wctest::lex::l_basic_2()
{
	wcLexer lexer;

	vector<wcToken> lexOutput = lexer.lex("1 55 000001 128");

	vector<wcToken> correctOutput = { wcToken(tt_intlit, "1"),wcToken(tt_intlit, "55"),wcToken(tt_intlit, "000001"),wcToken(tt_intlit, "128") };

	if (lexer.getError() != ec_null)
		return !(l_compare(correctOutput, lexOutput));
	return lexer.getError().code;
}

//lex some floats
int wctest::lex::l_basic_3()
{
	wcLexer lexer;

	vector<wcToken> lexOutput = lexer.lex("3.14 500.500 0.000000001 128.0");

	vector<wcToken> correctOutput = 
	{ wcToken(tt_fltlit, "3.14"),wcToken(tt_fltlit, "500.500"),wcToken(tt_fltlit, "0.000000001"),wcToken(tt_fltlit, "128.0") };

	if (lexer.getError() != ec_null)
		return !(l_compare(correctOutput, lexOutput));
	return lexer.getError().code;
}

//lex with errors
int wctest::lex::l_basic_4()
{
	wcLexer lexer;

	vector<wcToken> lexOutput = lexer.lex("3.1.4");
	if (lexer.getError() != ec_lex_unexpectedtoken)
		return lexer.getError().code;
		
	return 0;
}

//lex comments and numbers 
int wctest::lex::l_basic_5()
{
	wcLexer lexer;
	string sourceCode = "// comment here"
		"// another one here"
		"3.14 999 // comment"
		"/* multi line comment"
		"    "
		"123132 numbers not counted here"
		"/*";	

	vector<wcToken> correctOutput =
	{ wcToken(tt_fltlit, "3.14"),wcToken(tt_intlit, "999")};

	vector<wcToken> lexOutput = lexer.lex(sourceCode);

	if (lexer.getError() != wc::error::ec_null)
		return !(l_compare(correctOutput, lexOutput));
	return lexer.getError().code;
}

//lex random identifiers
int wctest::lex::l_basic_6()
{
	wcLexer lexer;

	vector<wcToken> lexOutput = lexer.lex("cats dogs bogs");

	vector<wcToken> correctOutput = { wcToken(tt_ident, "cats"),wcToken(tt_ident, "dogs"),wcToken(tt_ident, "bogs") };

	if (lexer.getError() != wc::error::ec_null)
		return !(l_compare(correctOutput, lexOutput));
	return lexer.getError().code;
}

int wctest::lex::l_basic_7()
{
	wcLexer lexer;
	string sourceCode = "int a = 0;"
		"int b = a + 1;";

	vector<wcToken> correctOutput =
	{ 
		wcToken(tt_int, "int"),wcToken(tt_ident, "daveBrubeck"),wcToken(tt_assign, "="),
		wcToken(tt_intlit, "0"), wcToken(tt_scolon, ";"), wcToken(tt_key_return, "return"), 
		wcToken(tt_ident, "daveBrubeck"), wcToken(tt_scolon, ";") 
	};

	vector<wcToken> lexOutput = lexer.lex(sourceCode);

	if (lexer.getError() != ec_null)
		return !(l_compare(correctOutput, lexOutput));
	return lexer.getError().code;
}

