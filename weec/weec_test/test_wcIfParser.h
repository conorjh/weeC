#ifndef WC_TEST_WCIFPARSER_H
#define WC_TEST_WCIFPARSER_H
#include <string>
#include <vector>
#include "parse.h"

namespace weec
{
	namespace test
	{

		int Test_wcIfParser_if_true_single_line()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;

			string IfStatement = "if(true)	true;";
			auto IfParserOutput = wcIfParser(*new wcParseState(*new wcTokenizer(IfStatement, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (IfParserOutput.Error.Code != wcParserErrorCode::None)	return 1;

			return 0;
		}

		int Test_wcIfParser_if_true_else_single_line()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;

			string IfStatement = "if(true)	true; else false;";
			auto IfParserOutput = wcIfParser(*new wcParseState(*new wcTokenizer(IfStatement, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (IfParserOutput.Error.Code != wcParserErrorCode::None)	return 1;

			return 0;
		}

		int Test_wcIfParser_if_true_else_newlines_and_braces()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;

			string IfStatement = "if(true)\n{\n true;\n }\n else \n{\n false;\n }";
			auto IfParserOutput = wcIfParser(*new wcParseState(*new wcTokenizer(IfStatement, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (IfParserOutput.Error.Code != wcParserErrorCode::None)	return 1;

			return 0;
		}
		int Test_wcIfParser_if_true_else_newlines()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;

			string IfStatement = "if(true)\n true;\n  else \n false;";
			auto IfParserOutput = wcIfParser(*new wcParseState(*new wcTokenizer(IfStatement, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (IfParserOutput.Error.Code != wcParserErrorCode::None)	return 1;

			return 0;
		}
	}
}

#endif