#pragma once
#include <string>
#include "lexer.h"
#include "parser.h"
#include "bytecode.h"
#include "vm.h"

namespace bc
{
	namespace test
	{
		int test_lexer();
		int test_parser();
		
		std::string getTypeAsString(lex::bcTokenType _t);
		std::string getTypeAsString(parse::bcParseNodeType _t);
		std::string getTypeAsString(parse::bcSymbolType _t);
		std::string getTypeAsString(vm::bcOpCode _t);
		std::string getTypeAsString(vm::bcValType _t);
		std::string getTypeAsString(bcErrorCode _t);
		void printLexer(lex::bcLexer*);
		void printParser(parse::bcParser*);
		void printGen(vm::bcByteCodeGen*);
	}
}