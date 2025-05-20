#ifndef WC_TEST_WCFUNCTIONIDENTIFIER_H
#define WC_TEST_WCFUNCTIONIDENTIFIER_H
#include <string>
#include <vector>
#include "parse.h"

namespace weec
{
	namespace test
	{
		int Test_wcFunctionIdentifier_default_constructor()
		{
			using namespace std;
			using namespace weec::parse;

			wcFunctionIdentifier FuncIdent;
			if (FuncIdent.ArgumentCount() != 0)					return 1;
			if (FuncIdent.to_string() != "$g")					return 3;
			if (FuncIdent.to_string_no_arguments() != "$g")		return 4;
			if (FuncIdent != string("$g"))						return 5;

			return 0;
		}

		int Test_wcFunctionIdentifier_default_constructor2()
		{
			using namespace std;
			using namespace weec::parse;

			wcFunctionIdentifier FuncIdent1("test()", {});
			if (FuncIdent1.ArgumentCount() != 0)							return 1;
			if (FuncIdent1.to_string() != "$g::test()")						return 2;
			if (FuncIdent1.to_string_no_arguments() != string("$g::test"))	return 3;
			if (FuncIdent1 != string("$g::test()"))							return 4;

			return 0;
		}

		int Test_wcFunctionIdentifier_string_constructor_no_parenthesis_in_identifier()
		{
			using namespace std;
			using namespace weec::parse;

			//construct with no parenthesis in identifier, should be corrected
			wcFunctionIdentifier FuncIdent2("testWithoutParens", {});
			if (FuncIdent2.ArgumentCount() != 0)										return 1;
			if (FuncIdent2.to_string() != string("$g::testWithoutParens()"))			return 2;
			if (FuncIdent2.to_string_no_arguments() != string("$g::testWithoutParens"))	return 3;
			if (FuncIdent2 != string("$g::testWithoutParens()"))						return 4;

			return 0;
		}

		int Test_wcFunctionIdentifier_string_constructor_with_parameters()
		{
			using namespace std;
			using namespace weec::parse;

			wcFunctionIdentifier FuncIdent4("testWithParens()", { wcParseParameter("int", "param1"), wcParseParameter("int", "param2") });
			if (FuncIdent4.ArgumentCount() != 2)											return 1;
			if (FuncIdent4.to_string() != string("$g::testWithParens($g::int,$g::int)"))	return 2;
			if (FuncIdent4.to_string_no_arguments() != string("$g::testWithParens"))		return 3;
			if (FuncIdent4 != string("$g::testWithParens($g::int,$g::int)"))				return 4;

			return 0;
		}

		int Test_wcFunctionIdentifier_string_constructor_with_parameters_and_namespace()
		{
			using namespace std;
			using namespace weec::parse;

			wcFunctionIdentifier FuncIdent5("NS::NS2::testWithParensAndNS()", { wcParseParameter("string", "param1"), wcParseParameter("float", "param2") });
			if (FuncIdent5.ArgumentCount() != 2)															return 1;
			if (FuncIdent5.to_string() != string("$g::NS::NS2::testWithParensAndNS($g::string,$g::float)"))	return 2;
			if (FuncIdent5.to_string_no_arguments() != string("$g::NS::NS2::testWithParensAndNS"))			return 3;
			if (FuncIdent5 != string("$g::NS::NS2::testWithParensAndNS($g::string,$g::float)"))				return 4;

			return 0;
		}
	}
}

#endif#pragma once
