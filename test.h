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
		void printLexer(lex::bcLexer*);
	}
}