#ifndef WC_TEST_WCSCOPEIDENTIFIER_H
#define WC_TEST_WCSCOPEIDENTIFIER_H
#include <string>
#include "parse.h"

namespace weec
{
	namespace test
	{
		int Test_wcScopeIdentifier_default_constructor()
		{
			using namespace std;
			using namespace weec::parse;

			string emptyString = "";	wcIdentifierScope blankScopeIdent;
			if (blankScopeIdent != emptyString)									return 1;
			if (blankScopeIdent.Size() != 0)									return 2;
			if (blankScopeIdent.to_string() != ParserConsts.GlobalIdentifier)	return 3;
			if (blankScopeIdent.to_unqualified_string() != emptyString)			return 4;
			if (blankScopeIdent.to_string_no_global() != emptyString)			return 5;

			return 0;
		}

		int Test_wcScopeIdentifier_string_constructor_short_identifier()
		{
			using namespace std;
			using namespace weec::parse;

			string simpleScope = "scope";	wcIdentifierScope simpleIdentScope(simpleScope);
			if (simpleIdentScope != simpleScope)												return 1;
			if (simpleIdentScope.Size() != simpleScope.size())									return 2;
			if (simpleIdentScope.to_string() != ParserConsts.GlobalIdentPrefix + simpleScope)	return 3;
			if (simpleIdentScope.to_unqualified_string() != simpleScope)						return 4;
			if (simpleIdentScope.to_string_no_global() != simpleScope)							return 5;

			return 0;
		}

		int Test_wcScopeIdentifier_string_constructor_already_qualified_identifier()
		{
			using namespace std;
			using namespace weec::parse;

			string simpleScope2 = "$g::scope";	wcIdentifierScope simpleIdentScope2(simpleScope2);
			if (simpleIdentScope2 != simpleScope2)						return 1;
			if (simpleIdentScope2.Size() != string("scope").size())		return 2;
			if (simpleIdentScope2.to_string() != simpleScope2)			return 3;
			if (simpleIdentScope2.to_unqualified_string() != "scope")	return 4;
			if (simpleIdentScope2.to_string_no_global() != "scope")		return 5;

			return 0;
		}

		int Test_wcScopeIdentifier_copy_constructor_already_qualified_identifier()
		{
			using namespace std;
			using namespace weec::parse;

			string simpleScope3 = "$g::somescope::anotherscope";
			wcIdentifierScope simpleIdentScope3(simpleScope3), copyConstructedScope(simpleIdentScope3);
			if (copyConstructedScope != simpleScope3)										return 1;
			if (copyConstructedScope != simpleIdentScope3)									return 2;
			if (copyConstructedScope.Size() != string("somescope::anotherscope").size())	return 3;
			if (copyConstructedScope.to_string() != simpleScope3)							return 4;
			if (copyConstructedScope.to_unqualified_string() != "anotherscope")				return 5;
			if (copyConstructedScope.to_string_no_global() != "somescope::anotherscope")	return 6;

			return 0;
		}
	}
}

#endif