#include "codegen.h"
#include "simple.h"
#include "tests.h"

using namespace wc;
using namespace wc::parse;
using namespace wc::lex;
using namespace wc::codegen;
using namespace wc::bytecode;
using namespace wc::vm;

namespace wctest
{
	namespace vm
	{
		int standardVMTest(const char*);
	}
}

int wctest::vm::standardVMTest(const char * p_source)
{
	wcLexer lexer;
	wcParser parser;
	wcSimpleBytecodeGen gen;

	//wcSimpleExecContext* testOutput = static_cast<wcSimpleExecContext*>(&gen.gen(parser.parse(lexer.lex(p_source))));
	wcSimpleExecContext testOutput = gen.genSimple(parser.parse(lexer.lex(p_source)));

	if (parser.getError().code)
		return parser.getError().code;

	wcSimpleVM vm;
	vm.exec(vm.load(testOutput));

	return 0;
}

int wctest::vm::vm_basic_1()
{
	return standardVMTest("2 + 55 * 88 / 99;");
}