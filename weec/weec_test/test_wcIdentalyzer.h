#ifndef WC_TEST_WCIDENTALYZER_H
#define WC_TEST_WCIDENTALYZER_H
#include <string>
#include <vector>
#include "parse.h"

namespace weec
{
	namespace test
	{
		int Test_wcIdentalyzer_ContainsGlobal()
		{
			using namespace weec::parse;

			if (!wcIdentalyzer().ContainsGlobal(ParserConsts.GlobalIdentifier))		return 1;
			if (wcIdentalyzer().ContainsGlobal("shouldFail"))						return 2;

			return 0;
		}

		int Test_wcIdentalyzer_ContainsNamespace()
		{
			using namespace weec::parse;

			if (!wcIdentalyzer().ContainsNamespace(ParserConsts.GlobalIdentPrefix))	return 1;
			if (!wcIdentalyzer().ContainsNamespace("testNS::testIdentifier"))		return 2;
			if (wcIdentalyzer().ContainsNamespace("shouldFail"))					return 3;

			return 0;
		};

		int Test_wcIdentalyzer_IsFunction()
		{
			using namespace weec::parse;

			if (!wcIdentalyzer().IsFunction("function()"))						return 1;
			if (!wcIdentalyzer().IsFunction("$g::ns::function()"))				return 2;
			if (!wcIdentalyzer().IsFunction("function($g::int)"))				return 3;
			if (!wcIdentalyzer().IsFunction("$g::ns::function($g::int)"))		return 4;
			if (wcIdentalyzer().IsFunction("shouldFail"))						return 5;
			if (wcIdentalyzer().IsFunction("shouldFail()::a"))					return 6;
			if (wcIdentalyzer().IsFunction("shouldFail($g::int)::a"))			return 7;
			if (wcIdentalyzer().IsFunction("$g::ns::shouldFail()::a"))			return 8;

			return 0;
		}

		int Test_wcIdentalyzer_IsQualified()
		{
			using namespace weec::parse;

			if (!wcIdentalyzer().IsQualified("$g::ns::ident"))				return 1;
			if (!wcIdentalyzer().IsQualified("$g::ns::ident()"))			return 2;
			if (!wcIdentalyzer().IsQualified("$g::ns::ident($g::int)"))		return 3;
			if (!wcIdentalyzer().IsQualified("ident()::a"))					return 4;
			if (!wcIdentalyzer().IsQualified("ident($g::int)::a"))			return 5;
			if (wcIdentalyzer().IsQualified("shouldFail"))					return 6;
			if (wcIdentalyzer().IsQualified("shouldFail($g::int)"))			return 7;

			return 0;
		}

		int Test_wcIdentalyzer_IsMember()
		{
			using namespace weec::parse;

			if (wcIdentalyzer().IsMember("$g::ns::ident"))				return 1;
			if (wcIdentalyzer().IsMember("$g::ns::ident()"))			return 2;
			if (wcIdentalyzer().IsMember("$g::ns::ident($g::int)"))		return 3;
			if (!wcIdentalyzer().IsMember("$g::ns::ident.member"))		return 4;
			if (wcIdentalyzer().IsMember("ident()::a"))					return 5;
			if (!wcIdentalyzer().IsMember("ident()::a.a"))				return 6;
			if (wcIdentalyzer().IsMember("ident($g::int)::a"))			return 7;
			if (!wcIdentalyzer().IsMember("ident($g::int).a.b.c"))		return 8;
			if (wcIdentalyzer().IsMember("shouldFail"))					return 9;
			if (wcIdentalyzer().IsMember("shouldFail($g::int)"))		return 10;

			return 0;
		}

		int Test_wcIdentalyzer_IsQualifiedWithGlobal()
		{
			using namespace weec::parse;

			if (!wcIdentalyzer().IsQualifiedWithGlobal("$g::ns::ident"))			return 1;
			if (wcIdentalyzer().IsQualifiedWithGlobal("ns::shouldFail"))			return 2;
			if (wcIdentalyzer().IsQualifiedWithGlobal("shouldFail"))				return 3;
			if (wcIdentalyzer().IsQualifiedWithGlobal("shouldFail()::a"))			return 4;
			if (wcIdentalyzer().IsQualifiedWithGlobal("shouldFail($g::int)::a"))	return 5;

			return 0;
		}

		int Test_wcIdentalyzer_GetIdentifierFromQualifiedIdentifier()
		{
			using namespace weec::parse;

			if (wcIdentalyzer().GetIdentifierFromQualifiedIdentifier("$g::ns::ident") != "ident")									return 1;
			if (wcIdentalyzer().GetIdentifierFromQualifiedIdentifier("$g::ns::ident()") != "ident()")								return 2;
			if (wcIdentalyzer().GetIdentifierFromQualifiedIdentifier("$g::ns::ident($g::int)") != "ident($g::int)")					return 3;
			if (wcIdentalyzer().GetIdentifierFromQualifiedIdentifier("$g::ns::ident($g::int,$g::int)") != "ident($g::int,$g::int)")	return 4;
			if (wcIdentalyzer().GetIdentifierFromQualifiedIdentifier("ns::ident") != "ident")										return 5;
			if (wcIdentalyzer().GetIdentifierFromQualifiedIdentifier("ns::ident()::a") != "a")										return 6;
			if (wcIdentalyzer().GetIdentifierFromQualifiedIdentifier("ns::ident($g::int)::a") != "a")								return 7;

			return 0;
		}

		int Test_wcIdentalyzer_GetNamespaceFromQualifiedIdentifier()
		{
			using namespace weec::parse;

			if (wcIdentalyzer().GetNamespaceFromQualifiedIdentifier("$g::ns::ident") != "$g::ns")					return 1;
			if (wcIdentalyzer().GetNamespaceFromQualifiedIdentifier("$g::ns::ident()") != "$g::ns")					return 2;
			if (wcIdentalyzer().GetNamespaceFromQualifiedIdentifier("$g::ns::ident($g::int)") != "$g::ns")			return 3;
			if (wcIdentalyzer().GetNamespaceFromQualifiedIdentifier("$g::ns::ident($g::int,$g::int)") != "$g::ns")	return 4;
			if (wcIdentalyzer().GetNamespaceFromQualifiedIdentifier("ns::ident") != "ns")							return 5;

			return 0;
		}

		int Test_wcIdentalyzer_GetParameterListIdentifierString()
		{
			using namespace weec::parse;

			auto paramList1 = { wcFullIdentifier("$g::int") };
			auto paramList2 = { wcFullIdentifier("$g::int"), wcFullIdentifier("$g::int") };
			auto paramList3 = { wcFullIdentifier("$g::float"), wcFullIdentifier("$g::string"), wcFullIdentifier("$g::ns::customType") };
			if (wcIdentalyzer().GetParameterListIdentifierString(paramList1) != "$g::int")									return 1;
			if (wcIdentalyzer().GetParameterListIdentifierString(paramList2) != "$g::int,$g::int")							return 2;
			if (wcIdentalyzer().GetParameterListIdentifierString(paramList3) != "$g::float,$g::string,$g::ns::customType")	return 3;

			return 0;
		}

		int Test_wcIdentalyzer_StripArgumentsFromFunctionIdentifier()
		{
			using namespace weec::parse;

			if (wcIdentalyzer().StripArgumentsFromFunctionIdentifier("function()") != "function")							return 1;
			if (wcIdentalyzer().StripArgumentsFromFunctionIdentifier("$g::ns::function()") != "$g::ns::function")			return 2;
			if (wcIdentalyzer().StripArgumentsFromFunctionIdentifier("function($g::int)") != "function")					return 3;
			if (wcIdentalyzer().StripArgumentsFromFunctionIdentifier("$g::ns::function($g::int)") != "$g::ns::function")	return 4;
			if (wcIdentalyzer().StripArgumentsFromFunctionIdentifier("ident") != "ident")									return 5;
			if (wcIdentalyzer().StripArgumentsFromFunctionIdentifier("shouldFail()::a") != "shouldFail()::a")				return 6;

			return 0;
		}
	}

}
#endif