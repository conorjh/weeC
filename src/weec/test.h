#include "lexer.h"
#include "parser.h"
#include <string>
namespace wc
{
	namespace test
	{
		int test_lexer();
		int test_parser();
		
		std::string getTypeAsString(lex::wcTokenType _t);
		std::string getTypeAsString(parse::wcParseNodeType _t);
		void printLexer(lex::wcLexer*);
		void printParser(parse::wcParser*);
	}
}