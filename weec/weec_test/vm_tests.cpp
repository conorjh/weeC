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
		int standardVMTest(const char*,int expectedResult);
	}
}

int wctest::vm::standardVMTest(const char * p_source)
{
	wcLexer lexer;
	wcParser parser;
	wcSimpleBytecodeGen gen;

	wcSimpleExecContext testOutput = gen.genSimple(parser.parse(lexer.lex(p_source)));

	if (parser.getError().code)
		return parser.getError().code;

	wcSimpleVM vm;
	int handle = vm.load(testOutput);
	vm.exec(handle);
	testOutput = vm.getContext(handle);
	if (testOutput.registers.eax == 0)
		return 0;
	return 1;
}

int wctest::vm::standardVMTest(const char * p_source, int expectedResult)
{
	wcLexer lexer;
	wcParser parser;
	wcSimpleBytecodeGen gen;
	wcSimpleExecContext testOutput = gen.genSimple(parser.parse(lexer.lex(p_source)));

	if (parser.getError().code)
		return parser.getError().code;

	wcSimpleVM vm;
	int andle = vm.load(testOutput);
	vm.exec(andle);
	testOutput = vm.getContext(andle);
	if (testOutput.registers.eax == expectedResult)
		return 0;
	return 1;
}

int wctest::vm::vm_basic_1()
{
	return standardVMTest("2 + 55 * 88 / 99;", 2 + 55 * 88 / 99);
}

int wctest::vm::vm_basic_2()
{
	return standardVMTest("int a = 22 + 22;");
}

int wctest::vm::vm_basic_3()
{
	return standardVMTest("int a = 22 + 22;"
		"a = 22 + a;");
}