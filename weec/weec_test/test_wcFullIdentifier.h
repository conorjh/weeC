#ifndef WC_TEST_WCFULLIDENTIFIER_H
#define WC_TEST_WCFULLIDENTIFIER_H
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

		int Test_wcFullIdentifier_default_constructor()
		{
			using namespace std;
			using namespace weec::parse;

			string emtpyString = "";
			wcFullIdentifier blankIdentifier;
			if (blankIdentifier != emtpyString)												return 1;
			if (blankIdentifier.Size() != 0) 												return 2;
			if (blankIdentifier.IsFunction())												return 3;
			if (blankIdentifier.to_string() != ParserConsts.GlobalIdentifier)				return 4;
			if (blankIdentifier.to_unqualified_string() != emtpyString)						return 5;
			if (blankIdentifier.to_string_no_global() != emtpyString)						return 6;
			if (blankIdentifier.to_string_no_arguments() != ParserConsts.GlobalIdentifier)	return 7;
			if (blankIdentifier.to_string_no_arguments_no_global() != emtpyString)			return 8;
			if (blankIdentifier.to_string_unqualified_no_arguments() != emtpyString)		return 9;

			return 0;
		}

		int Test_wcFullIdentifier_string_constructor()
		{
			using namespace std;
			using namespace weec::parse;

			string identifier = "ident", 
				full_ident = ParserConsts.GlobalIdentPrefix + identifier;
			wcFullIdentifier strconstructor(identifier);
			if (strconstructor != full_ident)										return 1;
			if (strconstructor.Size() != identifier.size()) 						return 2;
			if (strconstructor.IsFunction())										return 3;
			if (strconstructor.to_string() != full_ident)							return 4;
			if (strconstructor.to_unqualified_string() != identifier)				return 5;
			if (strconstructor.to_string_no_global() != identifier)					return 6;
			if (strconstructor.to_string_no_arguments() != full_ident)				return 7;
			if (strconstructor.to_string_no_arguments_no_global() != identifier)	return 8;
			if (strconstructor.to_string_unqualified_no_arguments() != identifier)	return 9;

			return 0;
		}

		int Test_wcFullIdentifier_string_constructor_with_namespace()
		{
			using namespace std;
			using namespace weec::parse;

			string identifier = "namespace::ident", 
				full_ident = ParserConsts.GlobalIdentPrefix + identifier;
			wcFullIdentifier strconstructor(identifier);
			if (strconstructor != full_ident)										return 1;
			if (strconstructor.Size() != identifier.size()) 						return 2;
			if (strconstructor.IsFunction())										return 3;
			if (strconstructor.to_string() != full_ident)							return 4;
			if (strconstructor.to_unqualified_string() != "ident")					return 5;
			if (strconstructor.to_string_no_global() != identifier)					return 6;
			if (strconstructor.to_string_no_arguments() != full_ident)				return 7;
			if (strconstructor.to_string_no_arguments_no_global() != identifier)	return 8;
			if (strconstructor.to_string_unqualified_no_arguments() != "ident")		return 9;

			return 0;
		}

		int Test_wcFullIdentifier_string_constructor_with_function_and_namespace()
		{
			using namespace std;
			using namespace weec::parse;

			string identifier = "namespace::ident()", 
				full_ident = ParserConsts.GlobalIdentPrefix + identifier;
			wcFullIdentifier strconstructor(identifier);
			if (strconstructor != full_ident)												return 1;
			if (strconstructor.Size() != identifier.size()) 								return 2;
			if (!strconstructor.IsFunction())												return 3;
			if (strconstructor.to_string() != full_ident)									return 4;
			if (strconstructor.to_unqualified_string() != "ident()")						return 5;
			if (strconstructor.to_string_no_global() != identifier)							return 6;
			if (strconstructor.to_string_no_arguments() != "$g::namespace::ident")			return 7;
			if (strconstructor.to_string_no_arguments_no_global() != "namespace::ident")	return 8;
			if (strconstructor.to_string_unqualified_no_arguments() != "ident")				return 9;

			return 0;
		}

		int Test_wcFullIdentifier_string_constructor_with_namespace_and_global()
		{
			using namespace std;
			using namespace weec::parse;

			string identifier = "$g::NS::anotherNS::SomeIdentifier";
			wcFullIdentifier strconstructor(identifier);
			if (strconstructor != identifier)															return 1;
			if (strconstructor.Size() != string("NS::anotherNS::SomeIdentifier").size()) 				return 2;
			if (strconstructor.IsFunction())															return 3;
			if (strconstructor.to_string() != identifier)												return 4;
			if (strconstructor.to_unqualified_string() != "SomeIdentifier")								return 5;
			if (strconstructor.to_string_no_global() != "NS::anotherNS::SomeIdentifier")				return 6;
			if (strconstructor.to_string_no_arguments() != identifier)									return 7;
			if (strconstructor.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")	return 8;
			if (strconstructor.to_string_unqualified_no_arguments() != "SomeIdentifier")				return 9;

			return 0;
		}

		int Test_wcFullIdentifier_string_constructor_with_function_namespace_and_global()
		{
			using namespace std;
			using namespace weec::parse;

			string identifier = "$g::NS::anotherNS::SomeIdentifier()";
			wcFullIdentifier strconstructor(identifier);
			if (strconstructor != identifier)															return 1;
			if (strconstructor.Size() != string("NS::anotherNS::SomeIdentifier()").size()) 				return 2;
			if (!strconstructor.IsFunction())															return 3;
			if (strconstructor.to_string() != identifier)												return 4;
			if (strconstructor.to_unqualified_string() != "SomeIdentifier()")							return 5;
			if (strconstructor.to_string_no_global() != "NS::anotherNS::SomeIdentifier()")				return 6;
			if (strconstructor.to_string_no_arguments() != "$g::NS::anotherNS::SomeIdentifier")			return 7;
			if (strconstructor.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")	return 8;
			if (strconstructor.to_string_unqualified_no_arguments() != "SomeIdentifier")				return 9;

			return 0;
		}

		int Test_wcFullIdentifier_copy_constructor_with_function_namespace_and_global()
		{
			using namespace std;
			using namespace weec::parse;

			wcFullIdentifier copy_source("$g::NS::anotherNS::SomeIdentifier()"), 
				copyconstructor(copy_source);
			if (copyconstructor != "$g::NS::anotherNS::SomeIdentifier()")								return 1;
			if (copyconstructor != copy_source)															return 2;
			if (copyconstructor.Size() != string("NS::anotherNS::SomeIdentifier()").size()) 			return 3;
			if (!copyconstructor.IsFunction())															return 4;
			if (copyconstructor.to_string() != "$g::NS::anotherNS::SomeIdentifier()")					return 5;
			if (copyconstructor.to_unqualified_string() != "SomeIdentifier()")							return 6;
			if (copyconstructor.to_string_no_global() != "NS::anotherNS::SomeIdentifier()")				return 7;
			if (copyconstructor.to_string_no_arguments() != "$g::NS::anotherNS::SomeIdentifier")		return 8;
			if (copyconstructor.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")	return 9;
			if (copyconstructor.to_string_unqualified_no_arguments() != "SomeIdentifier")				return 10;

			return 0;
		}

		int Test_wcFullIdentifier_part_constructor_with_function_namespace_and_global()
		{
			using namespace std;
			using namespace weec::parse;

			wcIdentifierScope scope1("$g::NS::anotherNS");
			wcIdentifier identifier1("SomeIdentifier()");
			wcFullIdentifier partconstructor(scope1, identifier1);
			if (partconstructor != "$g::NS::anotherNS::SomeIdentifier()")								return 1;
			if (partconstructor.Size() != string("NS::anotherNS::SomeIdentifier()").size()) 			return 2;
			if (!partconstructor.IsFunction())															return 3;
			if (partconstructor.to_string() != "$g::NS::anotherNS::SomeIdentifier()")					return 4;
			if (partconstructor.to_unqualified_string() != "SomeIdentifier()")							return 5;
			if (partconstructor.to_string_no_global() != "NS::anotherNS::SomeIdentifier()")				return 6;
			if (partconstructor.to_string_no_arguments() != "$g::NS::anotherNS::SomeIdentifier")		return 7;
			if (partconstructor.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")	return 8;
			if (partconstructor.to_string_unqualified_no_arguments() != "SomeIdentifier")				return 9;

			return 0;
		}

		int Test_wcFullIdentifier_part_constructor_with_function_parameters_namespace_and_global()
		{
			using namespace std;
			using namespace weec::parse;

			wcIdentifierScope scope1("$g::NS::anotherNS");
			wcIdentifier identifier2("SomeIdentifier($g::int,$g::int)");
			wcFullIdentifier partconstructor(scope1, identifier2);
			if (partconstructor != "$g::NS::anotherNS::SomeIdentifier($g::int,$g::int)")					return 1;
			if (partconstructor.Size() != string("NS::anotherNS::SomeIdentifier($g::int,$g::int)").size()) 	return 2;
			if (!partconstructor.IsFunction())																return 3;
			if (partconstructor.to_string() != "$g::NS::anotherNS::SomeIdentifier($g::int,$g::int)")		return 4;
			if (partconstructor.to_unqualified_string() != "SomeIdentifier($g::int,$g::int)")				return 5;
			if (partconstructor.to_string_no_global() != "NS::anotherNS::SomeIdentifier($g::int,$g::int)")	return 6;
			if (partconstructor.to_string_no_arguments() != "$g::NS::anotherNS::SomeIdentifier")			return 7;
			if (partconstructor.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")		return 8;
			if (partconstructor.to_string_unqualified_no_arguments() != "SomeIdentifier")					return 9;

			return 0;
		}

		int Test_wcFullIdentifier_part_constructor_with_function_local()
		{
			using namespace std;
			using namespace weec::parse;

			wcIdentifierScope scope1("$g::NS::anotherNS");
			wcIdentifier identifier3("SomeIdentifier($g::int,$g::int)::a");
			wcFullIdentifier partconstructor(scope1, identifier3);
			if (partconstructor != "$g::NS::anotherNS::SomeIdentifier($g::int,$g::int)::a")									return 1;
			if (partconstructor.Size() != string("NS::anotherNS::SomeIdentifier($g::int,$g::int)::a").size()) 				return 2;
			if (partconstructor.IsFunction())																				return 3;
			if (partconstructor.to_string() != "$g::NS::anotherNS::SomeIdentifier($g::int,$g::int)::a")						return 4;
			if (partconstructor.to_unqualified_string() != "a")																return 5;
			if (partconstructor.to_string_no_global() != "NS::anotherNS::SomeIdentifier($g::int,$g::int)::a")				return 6;
			if (partconstructor.to_string_no_arguments() != "$g::NS::anotherNS::SomeIdentifier($g::int,$g::int)::a")		return 7;
			if (partconstructor.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier($g::int,$g::int)::a")	return 8;
			if (partconstructor.to_string_unqualified_no_arguments() != "a")												return 9;

			return 0;
		}
	}
}

#endif