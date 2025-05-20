#ifndef WC_TEST_WCBLOCKPARSER_H
#define WC_TEST_WCBLOCKPARSER_H
#include <string>
#include <vector>
#include "parse.h"

namespace weec
{
	namespace test
	{
		int Test_wcBlockParser1()
		{
			using namespace  std;
			using namespace weec::lex;
			using namespace weec::parse;

			std::string Source1 = "{}", Source2 = "{\n{\n}\n}";

			auto Output1 = wcBlockParser(*new wcParseState(*new wcTokenizer(Source1), *new wcParseSymbolTable(), *new wcParseScopes())).Parse(true);
			auto Output2 = wcBlockParser(*new wcParseState(*new wcTokenizer(Source1), *new wcParseSymbolTable(), *new wcParseScopes())).Parse(true);

			return 0;
		}
	}
}

#endif