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
	if (testOutput.registers.ret != 0)
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
	if (testOutput.registers.ret == expectedResult)
		return 0;
	return 1;
}

int wctest::vm::vm_basic_1()
{
	return standardVMTest("123 * 345 + 678;", 123 * 345 + 678);
}

int wctest::vm::vm_basic_2()
{
	return standardVMTest("int a = 22 + 22;",44);
}

int wctest::vm::vm_basic_3()
{
	int a = 22 + 22;
	a = 22 + a;
	return standardVMTest("int a = 22 + 22;"
		"a = 22 + a;",a);
}

int wctest::vm::vm_basic_4()
{
	return standardVMTest("int a = 123;"
		"int b = 345;"
		"int c = a + b;", 123 + 345);
}

int wctest::vm::vm_basic_5()
{
	int a = 123; 
	int b = 345;
	int c = a + b;
	c = c * a + b;
	int d = a + b + c;
	return standardVMTest("int a = 123;"
		"int b = 345;"
		"int c = a + b;"
		"c = c * a + b;"
		"int d = a + b + c;",d);
}

int wctest::vm::vm_basic_6()
{
	int a = 111;
	(a * 222 + 333);
	return standardVMTest("int a = 111;"
		"a * 222 + 333; ", a * 222 + 333);
}

int wctest::vm::vm_basic_7()
{
	int a = 111;
	(a * 222 + 333);
	return standardVMTest("int a = 111;"
		"a = a * 222 + 333; ", a * 222 + 333);
}

int wctest::vm::vm_basic_8()
{
	int c = 468 * 123 + 345;
	return standardVMTest("int c = 468 * 123 + 345;", c);
}

int wctest::vm::vm_basic_9()
{
	int a = 123;
	int b = 345;
	int c = a + b;
	int d = c * a + b;
	int e = a + b + c;
	return standardVMTest("int a = 123;"
		"int b = 345;"
		"int c = a + b;"
		"int d = c * a + b;"
		"int e = a + b + c;", e);

}