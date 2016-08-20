#ifndef WEEC_TEST_H
#define WEEC_TEST_H

#include <string>
#include "..\weec\lexer.h"
#include "..\weec\parser.h"
#include "..\weec\bytecode.h"
#include "..\weec\vm.h"

namespace wc
{
	namespace test
	{
		int test_lexer();
		int test_parser();
		int test_parser_if();

		
		std::string getTypeAsString(lex::wcTokenType _t);
		std::string getTypeAsString(parse::wcParseNodeType _t);
		std::string getTypeAsString(parse::wcSymbolType _t);
		std::string getTypeAsString(vm::wcOpCode _t);
		std::string getTypeAsString(vm::wcValType _t);
		std::string getTypeAsString(wcErrorCode _t);
		void printLexer(lex::wcLexer*);
		void printParser(parse::wcParser*);
		void printGen(vm::wcByteCodeGen*);
	}
}

#endif