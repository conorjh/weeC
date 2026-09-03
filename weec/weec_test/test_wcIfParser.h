#ifndef WC_TEST_WCIFPARSER_H
#define WC_TEST_WCIFPARSER_H
#include <string>
#include "parse.h"

namespace weec
{
	namespace test
	{
		int Test_wcIfParser_if_true_single_line()
		{
			using namespace weec::lex;
			using namespace weec::parse;

			std::string IfStatement = "if(true)\ttrue;";
			auto IfParserOutput = wcIfParser(*new wcParseState(*new wcTokenizer(IfStatement, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (IfParserOutput.Error.Code != wcParserErrorCode::None)	return 1;
			if (IfParserOutput.AST.begin() == IfParserOutput.AST.end())	return 2;

			return 0;
		}

		int Test_wcIfParser_if_true_else_single_line()
		{
			using namespace weec::lex;
			using namespace weec::parse;

			std::string IfStatement = "if(true)\ttrue; else false;";
			auto IfParserOutput = wcIfParser(*new wcParseState(*new wcTokenizer(IfStatement, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (IfParserOutput.Error.Code != wcParserErrorCode::None)	return 1;
			if (IfParserOutput.AST.begin() == IfParserOutput.AST.end())	return 2;

			return 0;
		}

		int Test_wcIfParser_if_true_else_newlines_and_braces()
		{
			using namespace weec::lex;
			using namespace weec::parse;

			std::string IfStatement = "if(true)\n{\n true;\n }\n else \n{\n false;\n }";
			auto IfParserOutput = wcIfParser(*new wcParseState(*new wcTokenizer(IfStatement, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (IfParserOutput.Error.Code != wcParserErrorCode::None)	return 1;
			if (IfParserOutput.AST.begin() == IfParserOutput.AST.end())	return 2;

			return 0;
		}
		int Test_wcIfParser_if_true_else_newlines()
		{
			using namespace weec::lex;
			using namespace weec::parse;

			std::string IfStatement = "if(true)\n true;\n  else \n false;";
			auto IfParserOutput = wcIfParser(*new wcParseState(*new wcTokenizer(IfStatement, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (IfParserOutput.Error.Code != wcParserErrorCode::None)	return 1;
			if (IfParserOutput.AST.begin() == IfParserOutput.AST.end())	return 2;

			return 0;
		}
	}
}

#endif