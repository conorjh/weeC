#include "codegen.h"
#include "simple.h"
#include "tests.h"

using namespace wc;
using namespace wc::parse;
using namespace wc::lex;
using namespace wc::codegen;
using namespace wc::bytecode;

namespace wctest
{
	namespace gen
	{
		int standardGenTest(const char*);
	}
}

int wctest::gen::standardGenTest(const char * p_source)
{
	wcLexer lexer;
	wcParser parser;
	wcSimpleBytecodeGen gen;

	//wcSimpleExecContext* testOutput = static_cast<wcSimpleExecContext*>(&gen.gen(parser.parse(lexer.lex(p_source))));
	wcSimpleExecContext testOutput = gen.genSimple(parser.parse(lexer.lex(p_source)));
		
	if (parser.getError().code)
		return parser.getError().code;
	
	return 0;
}

int wctest::gen::g_basic_1()
{
	return standardGenTest("2 + 55 * 88 / 99;");
}