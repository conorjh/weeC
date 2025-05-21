#ifndef WC_TEST_WCIDENTIFIER_H
#define WC_TEST_WCIDENTIFIER_H
#include <string>
#include "parse.h"

namespace weec
{
	namespace test
	{
		int Test_wcIdentifier_blank_identifier()
		{
			using namespace std;
			using namespace weec::parse;

			//a blank wcIdentifier
			string emptyString = "";
			wcIdentifier blankIdent;

			if (blankIdent != emptyString)											return 1;
			if (blankIdent.Size() != 0)												return 2;
			if (blankIdent.IsQualified())											return 3;
			if (blankIdent.IsFunction())											return 4;
			if (blankIdent.IsMember())												return 4;
			if (blankIdent.to_string() != emptyString)								return 5;
			if (blankIdent.to_string_no_global() != emptyString)					return 6;
			if (blankIdent.to_unqualified_string() != emptyString)					return 7;
			if (blankIdent.to_string_no_arguments() != emptyString)					return 8;
			if (blankIdent.to_string_no_arguments_no_global() != emptyString)		return 9;
			if (blankIdent.to_string_unqualified_no_arguments() != emptyString)		return 10;

			return 0;
		}

		int Test_wcIdentifier1_string_constructed()
		{
			using namespace std;
			using namespace weec::parse;

			//a string constructed wcIdentifier
			string identString1 = "_TestIdent1";	
			wcIdentifier constructedIdentifier(identString1);

			if (constructedIdentifier != identString1)											return 1;
			if (constructedIdentifier.Size() != identString1.size())							return 2;
			if (constructedIdentifier.IsQualified())											return 3;
			if (constructedIdentifier.IsFunction())												return 4;
			if (constructedIdentifier.IsMember())												return 5;
			if (constructedIdentifier.to_string() != identString1)								return 6;
			if (constructedIdentifier.to_string_no_global() != identString1)					return 7;
			if (constructedIdentifier.to_unqualified_string() != identString1)					return 8;
			if (constructedIdentifier.to_string_no_arguments() != identString1)					return 9;
			if (constructedIdentifier.to_string_no_arguments_no_global() != identString1)		return 10;
			if (constructedIdentifier.to_string_unqualified_no_arguments() != identString1)		return 11;

			return 0;
		}

		int Test_wcIdentifier_string_constructed_with_namespace_in_string()
		{
			using namespace std;
			using namespace weec::parse;

			//a string constructed wcIdentifier, with a namespace in the string (or a fully qualified identifier) 
			string identStringWithNamespace = "ns::IdentWithNamespace";
			wcIdentifier fullyQualifiedIdentifier(identStringWithNamespace);

			if (fullyQualifiedIdentifier != identStringWithNamespace)										return 1;
			if (fullyQualifiedIdentifier.Size() != identStringWithNamespace.size())							return 2;
			if (!fullyQualifiedIdentifier.IsQualified())													return 3;
			if (fullyQualifiedIdentifier.IsFunction())														return 4;
			if (fullyQualifiedIdentifier.IsMember())														return 5;
			if (fullyQualifiedIdentifier.to_string() != identStringWithNamespace)							return 6;
			if (fullyQualifiedIdentifier.to_string_no_global() != identStringWithNamespace)					return 7;
			if (fullyQualifiedIdentifier.to_unqualified_string() != "IdentWithNamespace")					return 8;
			if (fullyQualifiedIdentifier.to_string_no_arguments() != identStringWithNamespace)				return 9;
			if (fullyQualifiedIdentifier.to_string_no_arguments_no_global() != identStringWithNamespace)	return 10;
			if (fullyQualifiedIdentifier.to_string_unqualified_no_arguments() != "IdentWithNamespace")		return 11;

			return 0;
		}

		int Test_wcIdentifier_string_constructed_with_function_in_string()
		{
			using namespace std;
			using namespace weec::parse;

			//a string constructed wcIdentifier, with a function in the string 
			string identStringWithFunction = "ThisIsAFunction()", 
			identStringWithoutFunction = "ThisIsAFunction";
			wcIdentifier identifierWithFunction(identStringWithFunction);

			if (identifierWithFunction != identStringWithFunction)														return 1;
			if (identifierWithFunction != identStringWithoutFunction)													return 2;
			if (identifierWithFunction.to_string() != identStringWithFunction)											return 3;
			if (identifierWithFunction.Size() != identStringWithFunction.size())										return 4;
			if (identifierWithFunction.IsQualified())																	return 5;
			if (!identifierWithFunction.IsFunction())																	return 6;
			if (identifierWithFunction.IsMember())																		return 7;
			if (identifierWithFunction.to_string() != identStringWithFunction)											return 8;
			if (identifierWithFunction.to_string_no_global() != identStringWithFunction)								return 9;
			if (identifierWithFunction.to_unqualified_string() != identStringWithFunction)								return 10;
			if (identifierWithFunction.to_string_no_arguments() != identStringWithoutFunction)							return 11;
			if (identifierWithFunction.to_string_no_arguments_no_global() != identStringWithoutFunction)				return 12;
			if (identifierWithFunction.to_string_unqualified_no_arguments() != identStringWithoutFunction)				return 13;

			return 0;
		}

		int Test_wcIdentifier_string_constructed_with_member_in_string()
		{
			using namespace std;
			using namespace weec::parse;

			//a string constructed wcIdentifier, with a member in the string 
			string identStringWithMember = "ident.member";
			wcIdentifier identifierWithMember(identStringWithMember);

			if (identifierWithMember != identStringWithMember)													return 1;
			if (identifierWithMember != identStringWithMember)													return 2;
			if (identifierWithMember.to_string() != identStringWithMember)										return 3;
			if (identifierWithMember.Size() != identStringWithMember.size())									return 4;
			if (identifierWithMember.IsQualified())																return 5;
			if (identifierWithMember.IsFunction())																return 6;
			if (!identifierWithMember.IsMember())																return 7;
			if (identifierWithMember.to_string() != identStringWithMember)										return 8;
			if (identifierWithMember.to_string_no_global() != identStringWithMember)							return 9;
			if (identifierWithMember.to_unqualified_string() != identStringWithMember)							return 10;
			if (identifierWithMember.to_string_no_arguments() != identStringWithMember)							return 11;
			if (identifierWithMember.to_string_no_arguments_no_global() != identStringWithMember)				return 12;
			if (identifierWithMember.to_string_unqualified_no_arguments() != identStringWithMember)				return 13;

			return 0;
		}

		int Test_wcIdentifier_string_constructed_with_function_and_member_in_string1()
		{
			using namespace std;
			using namespace weec::parse;

			//a string constructed wcIdentifier, with a function and member in the string 
			string identStringWithFunctionAndMember = "func().member";
			wcIdentifier identifierWithFunctionAndMember(identStringWithFunctionAndMember);

			if (identifierWithFunctionAndMember != identStringWithFunctionAndMember)											return 1;
			if (identifierWithFunctionAndMember != identStringWithFunctionAndMember)											return 2;
			if (identifierWithFunctionAndMember.to_string() != identStringWithFunctionAndMember)								return 3;
			if (identifierWithFunctionAndMember.Size() != identStringWithFunctionAndMember.size())								return 4;
			if (identifierWithFunctionAndMember.IsQualified())																	return 5;
			if (identifierWithFunctionAndMember.IsFunction())																	return 6;
			if (!identifierWithFunctionAndMember.IsMember())																	return 7;
			if (identifierWithFunctionAndMember.to_string() != identStringWithFunctionAndMember)								return 8;
			if (identifierWithFunctionAndMember.to_string_no_global() != identStringWithFunctionAndMember)						return 9;
			if (identifierWithFunctionAndMember.to_unqualified_string() != identStringWithFunctionAndMember)					return 10;
			if (identifierWithFunctionAndMember.to_string_no_arguments() != identStringWithFunctionAndMember)					return 11;
			if (identifierWithFunctionAndMember.to_string_no_arguments_no_global() != identStringWithFunctionAndMember)			return 12;
			if (identifierWithFunctionAndMember.to_string_unqualified_no_arguments() != identStringWithFunctionAndMember)		return 13;

			return 0;
		}

		int Test_wcIdentifier_string_constructed_with_function_in_string2()
		{
			using namespace std;
			using namespace weec::parse;

			//a string constructed wcIdentifier, with a function and member in the string 
			string identStringWithFunctionAsMember = "func().member()";
			wcIdentifier identifierWithFunctionAsMember(identStringWithFunctionAsMember);

			if (identifierWithFunctionAsMember != identStringWithFunctionAsMember)								return 1;
			if (identifierWithFunctionAsMember != identStringWithFunctionAsMember)								return 2;
			if (identifierWithFunctionAsMember.to_string() != identStringWithFunctionAsMember)					return 3;
			if (identifierWithFunctionAsMember.Size() != identStringWithFunctionAsMember.size())				return 4;
			if (identifierWithFunctionAsMember.IsQualified())													return 5;
			if (!identifierWithFunctionAsMember.IsFunction())													return 6;
			if (!identifierWithFunctionAsMember.IsMember())														return 7;
			if (identifierWithFunctionAsMember.to_string() != identStringWithFunctionAsMember)					return 8;
			if (identifierWithFunctionAsMember.to_string_no_global() != identStringWithFunctionAsMember)		return 9;
			if (identifierWithFunctionAsMember.to_unqualified_string() != identStringWithFunctionAsMember)		return 10;
			if (identifierWithFunctionAsMember.to_string_no_arguments() != "func().member")						return 11;
			if (identifierWithFunctionAsMember.to_string_no_arguments_no_global() != "func().member")			return 12;
			if (identifierWithFunctionAsMember.to_string_unqualified_no_arguments() != "func().member")			return 13;

			return 0;
		}

		int Test_wcIdentifier_string_constructed_with_namespace_function_and_member_in_string1()
		{
			using namespace std;
			using namespace weec::parse;

			//a string constructed wcIdentifier, with a function and member in the string 
			string identStringWithNsAndFunctionAndMember = "ns::ns2::func().x";
			wcIdentifier identifierWithNsAndFunctionAndMember(identStringWithNsAndFunctionAndMember);

			if (identifierWithNsAndFunctionAndMember != identStringWithNsAndFunctionAndMember)										return 1;
			if (identifierWithNsAndFunctionAndMember != identStringWithNsAndFunctionAndMember)										return 2;
			if (identifierWithNsAndFunctionAndMember.to_string() != identStringWithNsAndFunctionAndMember)							return 3;
			if (identifierWithNsAndFunctionAndMember.Size() != identStringWithNsAndFunctionAndMember.size())						return 4;
			if (!identifierWithNsAndFunctionAndMember.IsQualified())																return 5;
			if (identifierWithNsAndFunctionAndMember.IsFunction())																	return 6;
			if (!identifierWithNsAndFunctionAndMember.IsMember())																	return 7;
			if (identifierWithNsAndFunctionAndMember.to_string() != identStringWithNsAndFunctionAndMember)							return 8;
			if (identifierWithNsAndFunctionAndMember.to_string_no_global() != identStringWithNsAndFunctionAndMember)				return 9;
			if (identifierWithNsAndFunctionAndMember.to_unqualified_string() != "func().x")											return 10;
			if (identifierWithNsAndFunctionAndMember.to_string_no_arguments() != identStringWithNsAndFunctionAndMember)				return 11;
			if (identifierWithNsAndFunctionAndMember.to_string_no_arguments_no_global() != identStringWithNsAndFunctionAndMember)	return 12;
			if (identifierWithNsAndFunctionAndMember.to_string_unqualified_no_arguments() != "func().x")							return 13;

			return 0;
		}

		int Test_wcIdentifier_string_constructed_with_namespace_function_and_member_in_string2()
		{
			using namespace std;
			using namespace weec::parse;

			//a string constructed wcIdentifier, with a function and members in the string 
			string identStringWithFunctionNSAndMultipleMembers = "ns::ns2::func().x.func2().y()";
			wcIdentifier identifierWithFunctionNSAndMultipleMembers(identStringWithFunctionNSAndMultipleMembers);

			if (identifierWithFunctionNSAndMultipleMembers != identStringWithFunctionNSAndMultipleMembers)							return 1;
			if (identifierWithFunctionNSAndMultipleMembers != identStringWithFunctionNSAndMultipleMembers)							return 2;
			if (identifierWithFunctionNSAndMultipleMembers.to_string() != identStringWithFunctionNSAndMultipleMembers)				return 3;
			if (identifierWithFunctionNSAndMultipleMembers.Size() != identStringWithFunctionNSAndMultipleMembers.size())			return 4;
			if (!identifierWithFunctionNSAndMultipleMembers.IsQualified())															return 5;
			if (!identifierWithFunctionNSAndMultipleMembers.IsFunction())															return 6;
			if (!identifierWithFunctionNSAndMultipleMembers.IsMember())																return 7;
			if (identifierWithFunctionNSAndMultipleMembers.to_string() != identStringWithFunctionNSAndMultipleMembers)				return 8;
			if (identifierWithFunctionNSAndMultipleMembers.to_string_no_global() != identStringWithFunctionNSAndMultipleMembers)	return 9;
			if (identifierWithFunctionNSAndMultipleMembers.to_unqualified_string() != "func().x.func2().y()")						return 10;
			if (identifierWithFunctionNSAndMultipleMembers.to_string_no_arguments() != "ns::ns2::func().x.func2().y")				return 11;
			if (identifierWithFunctionNSAndMultipleMembers.to_string_no_arguments_no_global() != "ns::ns2::func().x.func2().y")		return 12;
			if (identifierWithFunctionNSAndMultipleMembers.to_string_unqualified_no_arguments() != "func().x.func2().y")			return 13;

			return 0;
		}

		int Test_wcIdentifier_string_constructed_with_fully_qualified_function_in_string()
		{
			using namespace std;
			using namespace weec::parse;

			//a string constructed wcIdentifier, with a fully qualified function in the string 
			string identStringWithFunctionAndNamespace = "ns::ThisIsAFunctionWithNamespace()", 
			identStringWithoutFunctionButFunction = "ns::ThisIsAFunctionWithNamespace";
			wcIdentifier identifierWithFunctionAndNamespace(identStringWithFunctionAndNamespace);

			if (identifierWithFunctionAndNamespace != identStringWithFunctionAndNamespace)										return 1;
			if (identifierWithFunctionAndNamespace != identStringWithoutFunctionButFunction)									return 2;
			if (identifierWithFunctionAndNamespace.to_string() != identStringWithFunctionAndNamespace)							return 3;
			if (identifierWithFunctionAndNamespace.Size() != identStringWithFunctionAndNamespace.size())						return 4;
			if (!identifierWithFunctionAndNamespace.IsQualified())																return 5;
			if (!identifierWithFunctionAndNamespace.IsFunction())																return 6;
			if (identifierWithFunctionAndNamespace.IsMember())																	return 7;
			if (identifierWithFunctionAndNamespace.to_string() != identStringWithFunctionAndNamespace)							return 8;
			if (identifierWithFunctionAndNamespace.to_string_no_global() != identStringWithFunctionAndNamespace)				return 9;
			if (identifierWithFunctionAndNamespace.to_unqualified_string() != "ThisIsAFunctionWithNamespace()")					return 10;
			if (identifierWithFunctionAndNamespace.to_string_no_arguments() != identStringWithoutFunctionButFunction)			return 11;
			if (identifierWithFunctionAndNamespace.to_string_no_arguments_no_global() != identStringWithoutFunctionButFunction)	return 12;
			if (identifierWithFunctionAndNamespace.to_string_unqualified_no_arguments() != "ThisIsAFunctionWithNamespace")		return 13;

			return 0;
		}

		int Test_wcIdentifier_copy_constructed_with_fully_qualified_identifier_with_global_namespace_in_string()
		{
			using namespace std;
			using namespace weec::parse;

			//a copy constructed wcIdentifier, with a fully qualified ident with global namespace
			string copyConstructedString = "$g::NS::anotherNS::SomeIdentifier";
			wcIdentifier originalIdent(copyConstructedString), copyConstructedIdent(originalIdent);

			if (copyConstructedIdent != copyConstructedString)													return 1;
			if (copyConstructedIdent != originalIdent)															return 2;
			if (copyConstructedIdent.to_string() != copyConstructedString)										return 3;
			if (copyConstructedIdent.Size() != copyConstructedString.size() - 4)								return 4;
			if (!copyConstructedIdent.IsQualified())															return 5;
			if (copyConstructedIdent.IsFunction())																return 6;
			if (copyConstructedIdent.IsMember())																return 7;
			if (copyConstructedIdent.to_string() != copyConstructedString)										return 8;
			if (copyConstructedIdent.to_string_no_global() != "NS::anotherNS::SomeIdentifier")					return 9;
			if (copyConstructedIdent.to_unqualified_string() != "SomeIdentifier")								return 10;
			if (copyConstructedIdent.to_string_no_arguments() != copyConstructedString)							return 11;
			if (copyConstructedIdent.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")		return 12;
			if (copyConstructedIdent.to_string_unqualified_no_arguments() != "SomeIdentifier")					return 13;

			return 0;
		}

		int Test_wcIdentifier_copy_constructed_with_fully_qualified_function_identifier_with_global_namespace_in_string()
		{
			using namespace std;
			using namespace weec::parse;

			//a copy constructed wcIdentifier, with a fully qualified function ident with global namespace
			string copyConstructedString2 = "$g::NS::anotherNS::SomeIdentifier()";
			wcIdentifier originalIdent2(copyConstructedString2), copyConstructedIdent2(originalIdent2);

			if (copyConstructedIdent2 != copyConstructedString2)											return 1;
			if (copyConstructedIdent2 != originalIdent2)													return 2;
			if (copyConstructedIdent2.to_string() != copyConstructedString2)								return 3;
			if (copyConstructedIdent2.Size() != copyConstructedString2.size() - 4)							return 4;
			if (!copyConstructedIdent2.IsQualified())														return 5;
			if (!copyConstructedIdent2.IsFunction())														return 6;
			if (copyConstructedIdent2.IsMember())															return 7;
			if (copyConstructedIdent2.to_string() != copyConstructedString2)								return 8;
			if (copyConstructedIdent2.to_string_no_global() != "NS::anotherNS::SomeIdentifier()")			return 9;
			if (copyConstructedIdent2.to_unqualified_string() != "SomeIdentifier()")						return 10;
			if (copyConstructedIdent2.to_string_no_arguments() != "$g::NS::anotherNS::SomeIdentifier")		return 11;
			if (copyConstructedIdent2.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")return 12;
			if (copyConstructedIdent2.to_string_unqualified_no_arguments() != "SomeIdentifier")				return 13;

			return 0;
		}
	}
}

#endif