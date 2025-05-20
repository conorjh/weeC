#ifndef WC_TEST_WCDECLARATIONPARSER_H
#define WC_TEST_WCDECLARATIONPARSER_H
#include <string>
#include <vector>
#include "parse.h"

namespace weec
{
	namespace test
	{
		namespace listing
		{

		}



		int Test_wcDeclarationParser_simple_declaration()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;

			string SourceCode = "int a;";
			auto ParseOutput = wcDeclarationParser(*new wcParseState(*new wcTokenizer(SourceCode, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (ParseOutput.Error.Code != wcParserErrorCode::None)				return 1;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("a")))			return 2;

			return 0;
		}

		int Test_wcDeclarationParser_simple_declaration_and_assignment()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;


			string SourceCode = "int jiminy = 0;";
			auto ParseOutput = wcDeclarationParser(*new wcParseState(*new wcTokenizer(SourceCode, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (ParseOutput.Error.Code != wcParserErrorCode::None)				return 1;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("jiminy")))	return 2;

			return 0;
		}

		int Test_wcDeclarationParser_simple_function_declaration()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;

			string SourceCode = "int methodName()";
			auto ParseOutput = wcDeclarationParser(*new wcParseState(*new wcTokenizer(SourceCode, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (ParseOutput.Error.Code != wcParserErrorCode::None)									return 1;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("methodName()")))					return 2;
			if (ParseOutput.SymbolTable.Get(wcFullIdentifier("methodName()")).DataType != "int")	return 3;

			return 0;
		}

		int Test_wcDeclarationParser_simple_function_declaration_with_body()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;

			string SourceCode = "int methodName(){ return 0; }";
			auto ParseOutput = wcDeclarationParser(*new wcParseState(*new wcTokenizer(SourceCode, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (ParseOutput.Error.Code != wcParserErrorCode::None)									return 1;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("methodName()")))					return 2;
			if (ParseOutput.SymbolTable.Get(wcFullIdentifier("methodName()")).DataType != "int")	return 3;

			return 0;
		}

		int Test_wcDeclarationParser_simple_function_declaration_with_arguments_and_body()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;


			string SourceCode = "int methodNameWithArgs(int a, int b){ return 0; }";
			auto ParseOutput = wcDeclarationParser(*new wcParseState(*new wcTokenizer(SourceCode, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (ParseOutput.Error.Code != wcParserErrorCode::None)											return 1;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("methodNameWithArgs($g::int,$g::int)")))	return 2;

			return 0;
		}

		int Test_wcDeclarationParser_simple_function_declaration_with_arguments_and_body2()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;


			string SourceCode = "int methodNameWithArgs(int a, int b){ return a + b; }";
			auto ParseOutput = wcDeclarationParser(*new wcParseState(*new wcTokenizer(SourceCode, true), *new wcParseSymbolTable(), *new wcParseScopes())).Parse();
			if (ParseOutput.Error.Code != wcParserErrorCode::None)												return 1;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("methodNameWithArgs($g::int,$g::int)")))		return 2;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("methodNameWithArgs($g::int,$g::int)::a")))	return 3;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("methodNameWithArgs($g::int,$g::int)::b")))	return 4;

			return 0;
		}

		int Test_wcDeclarationParser_simple_function_declaration_with_arguments_and_body3()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;

			string SourceCode = "int aaa(int a, int b){ return a + b; } \n int bbb(int a, int b){ return a + b; }";
			auto Token = *new wcTokenizer(SourceCode, true); 
			auto SymbolTable = *new wcParseSymbolTable();
			
			//parse first half of the declaration
			auto ParseOutput = wcDeclarationParser(*new wcParseState(Token, SymbolTable, *new wcParseScopes())).Parse();
			if (ParseOutput.Error.Code != wcParserErrorCode::None)								return 1;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("aaa($g::int,$g::int)")))		return 2;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("aaa($g::int,$g::int)::a")))	return 3;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("aaa($g::int,$g::int)::b")))	return 4;

			//parse the second half
			ParseOutput += wcDeclarationParser(*new wcParseState(Token, SymbolTable, *new wcParseScopes())).Parse();
			if (ParseOutput.Error.Code != wcParserErrorCode::None)								return 1;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("bbb($g::int,$g::int)")))		return 5;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("bbb($g::int,$g::int)::a")))	return 6;
			if (!ParseOutput.SymbolTable.Exists(wcFullIdentifier("bbb($g::int,$g::int)::b")))	return 7;

			return 0;
		}
	}
}

#endif