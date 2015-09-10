#include "lexer.h"
#include "parser.h"
#include <string>
namespace bc
{
	namespace test
	{
		int test_lexer();
		int test_parser();
		
		std::string getTypeAsString(lex::bcTokenType _t);
		std::string getTypeAsString(parse::bcParseNodeType _t);
		void printLexer(lex::bcLexer*);
		void printParser(parse::bcParser*);
	}
}