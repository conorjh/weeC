#include "test.h"
#include "lex.h"
#include "parse.h"
#include "interpreter.h"
#include "tree_util.hh"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

using namespace std;
using namespace weec;
using namespace weec::lex;
using namespace weec::parse;
using namespace weec::interpreter;
using weec::lex::wcToken;

namespace weec
{
	namespace lex
	{

	}
}

void printTree(tree<wcParseNode> pTree)
{
	auto it = pTree.begin();

	while (it != pTree.end())
	{
		for (int t = 0; t < pTree.depth(it); t++)
			cout << " - ";
		cout << it->Token.StringToken.Data << " (" << to_string(it->Type) << ")" << endl;
		it++;
	}
}

int main(int argc, char* argv[])
{
	weec::test::lex::Test_AllLex();
	weec::test::lex::Test_AllParse();
}

int weec::test::lex::Test_AllLex()
{
	cout << "Running lex tests..." << endl;
	for (int t = 0; t < allLexTests.size(); ++t)
	{
		int testResult = allLexTests[t]();
		if (testResult)
			cout << "Test #" << t + 1 << " failed: " << testResult << endl;
		else
			cout << "Test #" << t + 1 << " passed" << endl;
	}

	return 0;
}

int weec::test::lex::Test_AllParse()
{
	cout << "Running parse tests..." << endl;
	for (int t = 0; t < allParseTests.size(); ++t)
	{
		int testResult = allParseTests[t]();
		if (testResult)
			cout << "\tTest #" << t + 1 << " failed: " << testResult << endl;
		else
			cout << "\tTest #" << t + 1 << " passed" << endl;
	}

	return 0;
}

int weec::test::lex::Test_StringTokenizer1()
{
	string input = listing::list_stringtokenizer4;
	wcStringTokenizer stringTokenizer(input);


	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "Hello")
		return 1;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::NewLine)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "World")
		return 2;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::NewLine)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "Longer")
		return 3;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::Whitespace)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "sentence")
		return 5;
	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::NewLine)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "Finishing")
		return 6;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::Whitespace)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "with")
		return 7;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::Whitespace)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "newline" || stringTokenizer.GetStringToken().Line != 4)
		return 8;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::NewLine)
		return 4;

	if (stringTokenizer.NextStringToken())	//end of stream
		return 9;

	return 0;
}

int weec::test::lex::Test_wcIdentalyzer1()
{
	if (!wcIdentalyzer().ContainsGlobal(ParserConsts.GlobalIdentifier))		return 1;
	if (wcIdentalyzer().ContainsGlobal("shouldFail"))						return 2;

	if (!wcIdentalyzer().ContainsNamespace(ParserConsts.GlobalIdentPrefix))	return 3;
	if (!wcIdentalyzer().ContainsNamespace("testNS::testIdentifier"))		return 4;
	if (wcIdentalyzer().ContainsNamespace("shouldFail"))					return 5;

	if (!wcIdentalyzer().IsFunction("function()"))							return 6;
	if (!wcIdentalyzer().IsFunction("$g::ns::function()"))					return 7;
	if (wcIdentalyzer().IsFunction("shouldFail"))							return 8;

	if (!wcIdentalyzer().IsQualified("$g::ns::ident"))						return 9;
	if (wcIdentalyzer().IsQualified("shouldFail"))							return 10;

	if (!wcIdentalyzer().IsQualifiedWithGlobal("$g::ns::ident"))			return 11;
	if (wcIdentalyzer().IsQualifiedWithGlobal("ns::shouldFail"))			return 12;
	if (wcIdentalyzer().IsQualifiedWithGlobal("shouldFail"))				return 13;

	if (wcIdentalyzer().GetIdentifierFromQualifiedIdentifier("$g::ns::ident") != "ident")		return 14;
	if (wcIdentalyzer().GetIdentifierFromQualifiedIdentifier("$g::ns::ident()") != "ident()")	return 15;
	if (wcIdentalyzer().GetIdentifierFromQualifiedIdentifier("ns::ident") != "ident")			return 16;

	if (wcIdentalyzer().GetNamespaceFromQualifiedIdentifier("$g::ns::ident") != "$g::ns")		return 17;
	if (wcIdentalyzer().GetNamespaceFromQualifiedIdentifier("$g::ns::ident()") != "$g::ns")		return 18;
	if (wcIdentalyzer().GetNamespaceFromQualifiedIdentifier("ns::ident") != "ns")				return 19;

	return 0;
}

int weec::test::lex::Test_wcIdentifier1()
{
	//a blank wcIdentifier
	string emptyString = "";
	wcIdentifier blankIdent;
	if (blankIdent != emptyString)													return 1;
	if (blankIdent.Size() != 0)														return 2;
	if (blankIdent.IsQualified())													return 3;
	if (blankIdent.IsFunction())													return 4;
	if (blankIdent.to_string() != emptyString)										return 5;
	if (blankIdent.to_string_no_global() != emptyString)							return 6;
	if (blankIdent.to_unqualified_string() != emptyString)							return 7;
	if (blankIdent.to_string_no_arguments() != emptyString)							return 8;
	if (blankIdent.to_string_no_arguments_no_global() != emptyString)				return 9;
	if (blankIdent.to_string_unqualified_no_arguments() != emptyString)				return 10;


	//a string constructed wcIdentifier
	string identString1 = "_TestIdent1";	wcIdentifier constructedIdentifier(identString1);
	if (constructedIdentifier != identString1)													return 11;
	if (constructedIdentifier.Size() != identString1.size())									return 12;
	if (constructedIdentifier.IsQualified())													return 13;
	if (constructedIdentifier.IsFunction())														return 14;
	if (constructedIdentifier.to_string() != identString1)										return 15;
	if (constructedIdentifier.to_string_no_global() != identString1)							return 16;
	if (constructedIdentifier.to_unqualified_string() != identString1)							return 17;
	if (constructedIdentifier.to_string_no_arguments() != identString1)							return 18;
	if (constructedIdentifier.to_string_no_arguments_no_global() != identString1)				return 19;
	if (constructedIdentifier.to_string_unqualified_no_arguments() != identString1)				return 20;


	//a string constructed wcIdentifier, with a namespace in the string (or a fully qualified identifier) 
	string identStringWithNamespace = "ns::IdentWithNamespace";
	wcIdentifier fullyQualifiedIdentifier(identStringWithNamespace);
	if (fullyQualifiedIdentifier != identStringWithNamespace)										return 21;
	if (fullyQualifiedIdentifier.Size() != identStringWithNamespace.size())							return 22;
	if (!fullyQualifiedIdentifier.IsQualified())													return 23;
	if (fullyQualifiedIdentifier.IsFunction())														return 24;
	if (fullyQualifiedIdentifier.to_string() != identStringWithNamespace)							return 25;
	if (fullyQualifiedIdentifier.to_string_no_global() != identStringWithNamespace)					return 26;
	if (fullyQualifiedIdentifier.to_unqualified_string() != "IdentWithNamespace")					return 27;
	if (fullyQualifiedIdentifier.to_string_no_arguments() != identStringWithNamespace)				return 28;
	if (fullyQualifiedIdentifier.to_string_no_arguments_no_global() != identStringWithNamespace)	return 29;
	if (fullyQualifiedIdentifier.to_string_unqualified_no_arguments() != "IdentWithNamespace")		return 30;


	//a string constructed wcIdentifier, with a function in the string 
	string identStringWithFunction = "ThisIsAFunction()", identStringWithoutFunction = "ThisIsAFunction";
	wcIdentifier identifierWithFunction(identStringWithFunction);
	if (identifierWithFunction != identStringWithFunction)														return 31;
	if (identifierWithFunction != identStringWithoutFunction)													return 32;
	if (identifierWithFunction.to_string() != identStringWithFunction)											return 33;
	if (identifierWithFunction.Size() != identStringWithFunction.size())										return 34;
	if (identifierWithFunction.IsQualified())																	return 35;
	if (!identifierWithFunction.IsFunction())																	return 36;
	if (identifierWithFunction.to_string() != identStringWithFunction)											return 37;
	if (identifierWithFunction.to_string_no_global() != identStringWithFunction)								return 38;
	if (identifierWithFunction.to_unqualified_string() != identStringWithFunction)								return 39;
	if (identifierWithFunction.to_string_no_arguments() != identStringWithoutFunction)							return 40;
	if (identifierWithFunction.to_string_no_arguments_no_global() != identStringWithoutFunction)				return 41;
	if (identifierWithFunction.to_string_unqualified_no_arguments() != identStringWithoutFunction)				return 42;


	//a string constructed wcIdentifier, with a fully qualified function in the string 
	string identStringWithFunctionAndNamespace = "ns::ThisIsAFunctionWithNamespace()", identStringWithoutFunctionButFunction = "ns::ThisIsAFunctionWithNamespace";
	wcIdentifier identifierWithFunctionAndNamespace(identStringWithFunctionAndNamespace);
	if (identifierWithFunctionAndNamespace != identStringWithFunctionAndNamespace)										return 43;
	if (identifierWithFunctionAndNamespace != identStringWithoutFunctionButFunction)									return 44;
	if (identifierWithFunctionAndNamespace.to_string() != identStringWithFunctionAndNamespace)							return 45;
	if (identifierWithFunctionAndNamespace.Size() != identStringWithFunctionAndNamespace.size())						return 46;
	if (!identifierWithFunctionAndNamespace.IsQualified())																return 47;
	if (!identifierWithFunctionAndNamespace.IsFunction())																return 48;
	if (identifierWithFunctionAndNamespace.to_string() != identStringWithFunctionAndNamespace)							return 49;
	if (identifierWithFunctionAndNamespace.to_string_no_global() != identStringWithFunctionAndNamespace)				return 50;
	if (identifierWithFunctionAndNamespace.to_unqualified_string() != "ThisIsAFunctionWithNamespace()")					return 51;
	if (identifierWithFunctionAndNamespace.to_string_no_arguments() != identStringWithoutFunctionButFunction)			return 52;
	if (identifierWithFunctionAndNamespace.to_string_no_arguments_no_global() != identStringWithoutFunctionButFunction)	return 53;
	if (identifierWithFunctionAndNamespace.to_string_unqualified_no_arguments() != "ThisIsAFunctionWithNamespace")		return 54;


	//a copy constructed wcIdentifier, with a fully qualified ident with global namespace
	string copyConstructedString = "$g::NS::anotherNS::SomeIdentifier";
	wcIdentifier originalIdent(copyConstructedString), copyConstructedIdent(originalIdent);
	if (copyConstructedIdent != copyConstructedString)													return 55;
	if (copyConstructedIdent != originalIdent)															return 56;
	if (copyConstructedIdent.to_string() != copyConstructedString)										return 57;
	if (copyConstructedIdent.Size() != copyConstructedString.size() - 4)								return 58;
	if (!copyConstructedIdent.IsQualified())															return 59;
	if (copyConstructedIdent.IsFunction())																return 60;
	if (copyConstructedIdent.to_string() != copyConstructedString)										return 61;
	if (copyConstructedIdent.to_string_no_global() != "NS::anotherNS::SomeIdentifier")					return 62;
	if (copyConstructedIdent.to_unqualified_string() != "SomeIdentifier")								return 63;
	if (copyConstructedIdent.to_string_no_arguments() != copyConstructedString)							return 64;
	if (copyConstructedIdent.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")		return 65;
	if (copyConstructedIdent.to_string_unqualified_no_arguments() != "SomeIdentifier")					return 66;


	//a copy constructed wcIdentifier, with a fully qualified function ident with global namespace
	string copyConstructedString2 = "$g::NS::anotherNS::SomeIdentifier()";
	wcIdentifier originalIdent2(copyConstructedString2), copyConstructedIdent2(originalIdent2);
	if (copyConstructedIdent2 != copyConstructedString2)											return 67;
	if (copyConstructedIdent2 != originalIdent2)													return 68;
	if (copyConstructedIdent2.to_string() != copyConstructedString2)								return 69;
	if (copyConstructedIdent2.Size() != copyConstructedString2.size() - 4)							return 70;
	if (!copyConstructedIdent2.IsQualified())														return 71;
	if (!copyConstructedIdent2.IsFunction())														return 72;
	if (copyConstructedIdent2.to_string() != copyConstructedString2)								return 73;
	if (copyConstructedIdent2.to_string_no_global() != "NS::anotherNS::SomeIdentifier()")			return 74;
	if (copyConstructedIdent2.to_unqualified_string() != "SomeIdentifier()")						return 75;
	if (copyConstructedIdent2.to_string_no_arguments() != "$g::NS::anotherNS::SomeIdentifier")		return 76;
	if (copyConstructedIdent2.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")return 77;
	if (copyConstructedIdent2.to_string_unqualified_no_arguments() != "SomeIdentifier")				return 78;

	return 0;
}

int weec::test::lex::Test_wcScopeIdentifier1()
{
	//a blank wcIdentifierScope
	string emptyString = "";	wcIdentifierScope blankScopeIdent;
	if (blankScopeIdent != emptyString)									return 1;
	if (blankScopeIdent.Size() != 0)									return 2;
	if (blankScopeIdent.to_string() != ParserConsts.GlobalIdentifier)	return 3;
	if (blankScopeIdent.to_unqualified_string() != emptyString)			return 4;
	if (blankScopeIdent.to_string_no_global() != emptyString)			return 5;

	//string constructed scope, short identifier
	string simpleScope = "scope";	wcIdentifierScope simpleIdentScope(simpleScope);
	if (simpleIdentScope != simpleScope)												return 6;
	if (simpleIdentScope.Size() != simpleScope.size())									return 7;
	if (simpleIdentScope.to_string() != ParserConsts.GlobalIdentPrefix + simpleScope)	return 8;
	if (simpleIdentScope.to_unqualified_string() != simpleScope)						return 9;
	if (simpleIdentScope.to_string_no_global() != simpleScope)							return 10;


	//string constructed scope, already qualified identifier
	string simpleScope2 = "$g::scope";	wcIdentifierScope simpleIdentScope2(simpleScope2);
	if (simpleIdentScope2 != simpleScope2)						return 11;
	if (simpleIdentScope2.Size() != string("scope").size())		return 12;
	if (simpleIdentScope2.to_string() != simpleScope2)			return 13;
	if (simpleIdentScope2.to_unqualified_string() != "scope")	return 14;
	if (simpleIdentScope2.to_string_no_global() != "scope")		return 15;

	//copy constructed scope, already qualified identifier
	string simpleScope3 = "$g::somescope::anotherscope";
	wcIdentifierScope simpleIdentScope3(simpleScope3), copyConstructedScope(simpleIdentScope3);
	if (copyConstructedScope != simpleScope3)										return 16;
	if (copyConstructedScope != simpleIdentScope3)									return 17;
	if (copyConstructedScope.Size() != string("somescope::anotherscope").size())	return 18;
	if (copyConstructedScope.to_string() != simpleScope3)							return 19;
	if (copyConstructedScope.to_unqualified_string() != "anotherscope")				return 20;
	if (copyConstructedScope.to_string_no_global() != "somescope::anotherscope")	return 21;

	return 0;
}


int weec::test::lex::Test_wcFullIdentifier1()
{
	//constructors tests...

	//a blank wcIdentifier
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


	//string constructed wcFullIdentifier
	string ident1 = "ident", full_ident1 = ParserConsts.GlobalIdentPrefix + ident1;
	wcFullIdentifier constructor1(ident1);
	if (constructor1 != full_ident1)									return 11;
	if (constructor1.Size() != ident1.size()) 							return 12;
	if (constructor1.IsFunction())										return 13;
	if (constructor1.to_string() != full_ident1)						return 14;
	if (constructor1.to_unqualified_string() != ident1)					return 15;
	if (constructor1.to_string_no_global() != ident1)					return 16;
	if (constructor1.to_string_no_arguments() != full_ident1)			return 17;
	if (constructor1.to_string_no_arguments_no_global() != ident1)		return 18;
	if (constructor1.to_string_unqualified_no_arguments() != ident1)	return 19;

	//string constructed wcFullIdentifier with namespace
	string ident2 = "namespace::ident", full_ident2 = ParserConsts.GlobalIdentPrefix + ident2;
	wcFullIdentifier constructor2(ident2);
	if (constructor2 != full_ident2)										return 21;
	if (constructor2.Size() != ident2.size()) 								return 22;
	if (constructor2.IsFunction())											return 23;
	if (constructor2.to_string() != full_ident2)							return 24;
	if (constructor2.to_unqualified_string() != "ident")					return 25;
	if (constructor2.to_string_no_global() != ident2)						return 26;
	if (constructor2.to_string_no_arguments() != full_ident2)				return 27;
	if (constructor2.to_string_no_arguments_no_global() != ident2)			return 28;
	if (constructor2.to_string_unqualified_no_arguments() != "ident")		return 29;

	//string constructed wcFullIdentifier with function and namespace
	string ident3 = "namespace::ident()", full_ident3 = ParserConsts.GlobalIdentPrefix + ident3;
	wcFullIdentifier constructor3(ident3);
	if (constructor3 != full_ident3)											return 31;
	if (constructor3.Size() != ident3.size()) 									return 32;
	if (!constructor3.IsFunction())												return 33;
	if (constructor3.to_string() != full_ident3)								return 34;
	if (constructor3.to_unqualified_string() != "ident()")						return 35;
	if (constructor3.to_string_no_global() != ident3)							return 36;
	if (constructor3.to_string_no_arguments() != "$g::namespace::ident")		return 37;
	if (constructor3.to_string_no_arguments_no_global() != "namespace::ident")	return 38;
	if (constructor3.to_string_unqualified_no_arguments() != "ident")			return 39;


	//string constructed wcFullIdentifier with namespace and global 
	string ident4 = "$g::NS::anotherNS::SomeIdentifier";
	wcFullIdentifier constructor4(ident4);
	if (constructor4 != ident4)																return 41;
	if (constructor4.Size() != string("NS::anotherNS::SomeIdentifier").size()) 				return 42;
	if (constructor4.IsFunction())															return 44;
	if (constructor4.to_string() != ident4)													return 44;
	if (constructor4.to_unqualified_string() != "SomeIdentifier")							return 45;
	if (constructor4.to_string_no_global() != "NS::anotherNS::SomeIdentifier")				return 46;
	if (constructor4.to_string_no_arguments() != ident4)									return 47;
	if (constructor4.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")	return 48;
	if (constructor4.to_string_unqualified_no_arguments() != "SomeIdentifier")				return 49;

	//string constructed wcFullIdentifier with function, namespace and global 
	string ident5 = "$g::NS::anotherNS::SomeIdentifier()";
	wcFullIdentifier constructor5(ident5);
	if (constructor5 != ident5)																return 51;
	if (constructor5.Size() != string("NS::anotherNS::SomeIdentifier()").size()) 			return 52;
	if (!constructor5.IsFunction())															return 55;
	if (constructor5.to_string() != ident5)													return 55;
	if (constructor5.to_unqualified_string() != "SomeIdentifier()")							return 55;
	if (constructor5.to_string_no_global() != "NS::anotherNS::SomeIdentifier()")			return 56;
	if (constructor5.to_string_no_arguments() != "$g::NS::anotherNS::SomeIdentifier")		return 57;
	if (constructor5.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")	return 58;
	if (constructor5.to_string_unqualified_no_arguments() != "SomeIdentifier")				return 59;


	//copy constructed wcFullIdentifier with function, namespace and global 
	wcFullIdentifier original1("$g::NS::anotherNS::SomeIdentifier()"), copyConstructed1(original1);
	if (copyConstructed1 != "$g::NS::anotherNS::SomeIdentifier()")								return 61;
	if (copyConstructed1 != original1)															return 62;
	if (copyConstructed1.Size() != string("NS::anotherNS::SomeIdentifier()").size()) 			return 63;
	if (!copyConstructed1.IsFunction())															return 64;
	if (copyConstructed1.to_string() != "$g::NS::anotherNS::SomeIdentifier()")					return 65;
	if (copyConstructed1.to_unqualified_string() != "SomeIdentifier()")							return 66;
	if (copyConstructed1.to_string_no_global() != "NS::anotherNS::SomeIdentifier()")			return 67;
	if (copyConstructed1.to_string_no_arguments() != "$g::NS::anotherNS::SomeIdentifier")		return 68;
	if (copyConstructed1.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")	return 69;
	if (copyConstructed1.to_string_unqualified_no_arguments() != "SomeIdentifier")				return 70;


	//part constructed wcFullIdentifier with function, namespace and global 
	wcIdentifierScope scope1("$g::NS::anotherNS");
	wcIdentifier identifier1("SomeIdentifier()");
	wcFullIdentifier fullIdent1(scope1, identifier1);
	if (fullIdent1 != "$g::NS::anotherNS::SomeIdentifier()")								return 61;
	//if (original1 != original1)															return 62;
	if (fullIdent1.Size() != string("NS::anotherNS::SomeIdentifier()").size()) 				return 63;
	if (!fullIdent1.IsFunction())															return 64;
	if (fullIdent1.to_string() != "$g::NS::anotherNS::SomeIdentifier()")					return 65;
	if (fullIdent1.to_unqualified_string() != "SomeIdentifier()")							return 66;
	if (fullIdent1.to_string_no_global() != "NS::anotherNS::SomeIdentifier()")				return 67;
	if (fullIdent1.to_string_no_arguments() != "$g::NS::anotherNS::SomeIdentifier")			return 68;
	if (fullIdent1.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")	return 69;
	if (fullIdent1.to_string_unqualified_no_arguments() != "SomeIdentifier")				return 70;
	/*
	//part constructed wcFullIdentifier with function, namespace and global
	wcIdentifierScope scope1("$g::NS::anotherNS");
	wcIdentifier identifier1("SomeIdentifier()");
	wcFullIdentifier fullIdent1(scope1, identifier1);
	if (fullIdent1 != "$g::NS::anotherNS::SomeIdentifier()")								return 61;
	//if (original1 != original1)															return 62;
	if (fullIdent1.Size() != string("NS::anotherNS::SomeIdentifier()").size()) 				return 63;
	if (!fullIdent1.IsFunction())															return 64;
	if (fullIdent1.to_string() != "$g::NS::anotherNS::SomeIdentifier()")					return 65;
	if (fullIdent1.to_unqualified_string() != "SomeIdentifier()")							return 66;
	if (fullIdent1.to_string_no_global() != "NS::anotherNS::SomeIdentifier()")				return 67;
	if (fullIdent1.to_string_no_arguments() != "$g::NS::anotherNS::SomeIdentifier")			return 68;
	if (fullIdent1.to_string_no_arguments_no_global() != "NS::anotherNS::SomeIdentifier")	return 69;
	if (fullIdent1.to_string_unqualified_no_arguments() != "SomeIdentifier")				return 70;
	*/
	return 0;
}

int weec::test::lex::Test_wcFunctionIdentifier1()
{
	wcFunctionIdentifier FuncIdent;
	if (FuncIdent.ArgumentCount() != 0)					return 1;
	if (FuncIdent.to_string() != "$g")					return 3;
	if (FuncIdent.to_string_no_arguments() != "$g")		return 4;
	if (FuncIdent != string("$g"))						return 5;

	wcFunctionIdentifier FuncIdent1("test()", {});
	if (FuncIdent1.ArgumentCount() != 0)							return 11;
	if (FuncIdent1.to_string() != "$g::test()")						return 13;
	if (FuncIdent1.to_string_no_arguments() != string("$g::test"))	return 14;
	if (FuncIdent1 != string("$g::test()"))							return 15;

	//construct with no parenthesis in identifier, should be corrected
	wcFunctionIdentifier FuncIdent2("testWithoutParens", {});
	if (FuncIdent2.ArgumentCount() != 0)										return 21;
	if (FuncIdent2.to_string() != string("$g::testWithoutParens()"))			return 23;
	if (FuncIdent2.to_string_no_arguments() != string("$g::testWithoutParens"))	return 24;
	if (FuncIdent2 != string("$g::testWithoutParens()"))						return 25;

	wcFunctionIdentifier FuncIdent4("testWithParens()", { wcParseParameter("int", "param1"), wcParseParameter("int", "param2") });
	if (FuncIdent4.ArgumentCount() != 2)											return 41;
	if (FuncIdent4.to_string() != string("$g::testWithParens($g::int,$g::int)"))	return 43;
	if (FuncIdent4.to_string_no_arguments() != string("$g::testWithParens"))		return 44;
	if (FuncIdent4 != string("$g::testWithParens($g::int,$g::int)"))				return 45;

	wcFunctionIdentifier FuncIdent5("NS::NS2::testWithParensAndNS()", { wcParseParameter("string", "param1"), wcParseParameter("float", "param2") });
	if (FuncIdent5.ArgumentCount() != 2)															return 51;
	if (FuncIdent5.to_string() != string("$g::NS::NS2::testWithParensAndNS($g::string,$g::float)"))	return 53;
	if (FuncIdent5.to_string_no_arguments() != string("$g::NS::NS2::testWithParensAndNS"))			return 54;
	if (FuncIdent5 != string("$g::NS::NS2::testWithParensAndNS($g::string,$g::float)"))				return 55;

	return 0;
}

int weec::test::lex::Test_wcParseSymbolTable1()
{
	//default constructor, check built in types
	wcParseSymbolTable SymTab1;
	if (!SymTab1.Exists("void"))	return 1;
	if (!SymTab1.Exists("int"))		return 1;
	if (!SymTab1.Exists("uint"))	return 1;
	if (!SymTab1.Exists("double"))	return 1;
	if (!SymTab1.Exists("float"))	return 1;
	if (!SymTab1.Exists("bool"))	return 1;
	if (!SymTab1.Exists("string"))	return 1;

	auto IntSymbol = SymTab1.Get("int");
	if (IntSymbol.Arguments != 0) 					return 2;
	if (IntSymbol.Const != false) 					return 3;
	if (IntSymbol.DataType != "") 					return 4;
	if (IntSymbol.FullIdent != "int") 				return 5;
	if (IntSymbol.HasOverloads != false) 			return 6;
	if (IntSymbol.IdentToken != wcToken()) 			return 7;
	if (IntSymbol.Registered != true) 				return 8;
	if (IntSymbol.Type != wcParseSymbolType::Type)	return 9;

	auto UIntSymbol = SymTab1.Get("uint");
	if (UIntSymbol.Arguments != 0) 					return 3;
	if (UIntSymbol.Const != false) 					return 3;
	if (UIntSymbol.DataType != "") 					return 3;
	if (UIntSymbol.FullIdent != "uint") 			return 3;
	if (UIntSymbol.HasOverloads != false) 			return 3;
	if (UIntSymbol.IdentToken != wcToken()) 		return 3;
	if (UIntSymbol.Registered != true) 				return 3;
	if (UIntSymbol.Type != wcParseSymbolType::Type)	return 3;

	auto DoubleSymbol = SymTab1.Get("double");
	if (DoubleSymbol.Arguments != 0) 					return 4;
	if (DoubleSymbol.Const != false) 					return 4;
	if (DoubleSymbol.DataType != "") 					return 4;
	if (DoubleSymbol.FullIdent != "double") 			return 4;
	if (DoubleSymbol.HasOverloads != false) 			return 4;
	if (DoubleSymbol.IdentToken != wcToken()) 			return 4;
	if (DoubleSymbol.Registered != true) 				return 4;
	if (DoubleSymbol.Type != wcParseSymbolType::Type)	return 4;

	auto FloatSymbol = SymTab1.Get("float");
	if (FloatSymbol.Arguments != 0) 					return 5;
	if (FloatSymbol.Const != false) 					return 5;
	if (FloatSymbol.DataType != "") 					return 5;
	if (FloatSymbol.FullIdent != "float") 				return 5;
	if (FloatSymbol.HasOverloads != false) 				return 5;
	if (FloatSymbol.IdentToken != wcToken()) 			return 5;
	if (FloatSymbol.Registered != true) 				return 5;
	if (FloatSymbol.Type != wcParseSymbolType::Type)	return 5;

	auto BoolSymbol = SymTab1.Get("bool");
	if (BoolSymbol.Arguments != 0) 					return 6;
	if (BoolSymbol.Const != false) 					return 6;
	if (BoolSymbol.DataType != "") 					return 6;
	if (BoolSymbol.FullIdent != "bool") 			return 6;
	if (BoolSymbol.HasOverloads != false) 			return 6;
	if (BoolSymbol.IdentToken != wcToken()) 		return 6;
	if (BoolSymbol.Registered != true) 				return 6;
	if (BoolSymbol.Type != wcParseSymbolType::Type)	return 6;

	auto StringSymbol = SymTab1.Get("string");
	if (StringSymbol.Arguments != 0) 					return 7;
	if (StringSymbol.Const != false) 					return 7;
	if (StringSymbol.DataType != "") 					return 7;
	if (StringSymbol.FullIdent != "string") 			return 7;
	if (StringSymbol.HasOverloads != false) 			return 7;
	if (StringSymbol.IdentToken != wcToken()) 			return 7;
	if (StringSymbol.Registered != true) 				return 7;
	if (StringSymbol.Type != wcParseSymbolType::Type)	return 7;

	auto VoidSymbol = SymTab1.Get("void");
	if (VoidSymbol.Arguments != 0) 					return 7;
	if (VoidSymbol.Const != false) 					return 7;
	if (VoidSymbol.DataType != "") 					return 7;
	if (VoidSymbol.FullIdent != "void") 			return 7;
	if (VoidSymbol.HasOverloads != false) 			return 7;
	if (VoidSymbol.IdentToken != wcToken()) 		return 7;
	if (VoidSymbol.Registered != true) 				return 7;
	if (VoidSymbol.Type != wcParseSymbolType::Type)	return 7;

	return 0;
}

int weec::test::lex::Test_wcParseSymbolTable2()
{	
	wcParseSymbolTable SymTab1;
	wcToken FakeToken(wcStringToken("a", wcStringTokenType::Alpha, 0, 0));

	auto SymbolCountBeforeAdd = SymTab1.Count();
	SymTab1.Add(wcParseSymbol(wcParseSymbolType::Variable, wcFullIdentifier("a"), wcFullIdentifier("int"), FakeToken));
	if (SymTab1.Count() != SymbolCountBeforeAdd + 1)	return 1;

	auto OurSymbol = SymTab1.Get("a");
	if (OurSymbol.Arguments != 0) 						return 7;
	if (OurSymbol.Const != false) 						return 7;
	if (OurSymbol.DataType != "int") 					return 7;
	if (OurSymbol.FullIdent != "a") 					return 7;
	if (OurSymbol.HasOverloads != false) 				return 7;
	if (OurSymbol.IdentToken != FakeToken) 				return 7;
	if (OurSymbol.Registered != true) 					return 7;
	if (OurSymbol.Type != wcParseSymbolType::Variable)	return 7;

	return 0;
}

int weec::test::lex::Test_wcParseScope()
{
	wcParseScope Scope("Test");
	if (Scope.Name != "Test")		return 1;
	if (Scope.Container.size())		return 2;

	wcParseScope Scope2("Namespace::Test", { "Symbol1", "Symbol2", "Symbol3" });
	if (Scope2.Name != "Namespace::Test")	return 1;
	if (!Scope2.Exists("Symbol2"))			return 2;
	if (!Scope2.Exists("Symbol2"))			return 3;
	if (!Scope2.Exists("Symbol3"))			return 4;
	if (Scope2.Container.size() != 3)		return 5;

	return 0;
}

int weec::test::lex::Test_wcParseScopes()
{
	wcParseScopes Scopes;
	if (Scopes.Size() != 1)										return 1;
	if (Scopes.Top().Exists("void") != true)					return 2;
	if (Scopes.Top().Exists("int") != true)						return 2;
	if (Scopes.Top().Exists("uint") != true)					return 3;
	if (Scopes.Top().Exists("float") != true)					return 4;
	if (Scopes.Top().Exists("double") != true)					return 5;
	if (Scopes.Top().Exists("bool") != true)					return 6;
	if (Scopes.Top().Exists("string") != true)					return 7;
	if (Scopes.Top().Count() != ParserConsts.BasicTypeCount)	return 8;
	if (Scopes.Size() != 1)				return 2;

	Scopes.Push(wcParseScope("test"));
	if (Scopes.Size() != 2)										return 9;
	if (Scopes.Top().Count() != 0 )	return 10;
	if (Scopes.Top().Name != "test")							return 11;

	Scopes.AddSymbol("testSymbol");
	if (Scopes.Size() != 2)											return 12;
	if (Scopes.Top().Count() != 1)	return 13;
	if (!Scopes.Top().Exists("testSymbol"))							return 14;


		

	return 0;
}

int weec::test::lex::Test_wcBlockParser1()
{
	std::string Source1 = "{}", Source2 = "{\n{\n}\n}";

	auto Output1 = wcBlockParser(*new wcTokenizer(Source1), *new wcParseSymbolTable(), *new wcParseScopes()).Parse(true);
	auto Output2 = wcBlockParser(*new wcTokenizer(Source2), *new wcParseSymbolTable(), *new wcParseScopes()).Parse(true);

	return 0;
}

int weec::test::lex::Test_wcDeclarationParser1()
{
	string Source1 = "int a;";
	auto Output1 = wcDeclarationParser(*new wcTokenizer(Source1, true), *new wcParseSymbolTable(), *new wcParseScopes()).Parse();
	if (Output1.Error.Code != wcParserErrorCode::None)				return 1;
	if (!Output1.SymbolTable.Exists(wcFullIdentifier("a")))			return 2;

	string Source2 = "int jiminy = 0;";
	auto Output2 = wcDeclarationParser(*new wcTokenizer(Source2, true), *new wcParseSymbolTable(), *new wcParseScopes()).Parse();
	if (Output2.Error.Code != wcParserErrorCode::None)				return 3;
	if (!Output2.SymbolTable.Exists(wcFullIdentifier("jiminy")))	return 4;

	string Source3 = "int methodName(){ return 0; }";
	auto Output3 = wcDeclarationParser(*new wcTokenizer(Source3, true), *new wcParseSymbolTable(), *new wcParseScopes()).Parse();
	if (Output3.Error.Code != wcParserErrorCode::None)									return 5;
	if (!Output3.SymbolTable.Exists(wcFullIdentifier("methodName()")))					return 6;
	if (Output3.SymbolTable.Get(wcFullIdentifier("methodName()")).DataType != "int")	return 7;

	string Source4 = "int methodNameWithArgs(int a, int b){ return 0; }";
	auto Output4 = wcDeclarationParser(*new wcTokenizer(Source4, true), *new wcParseSymbolTable(), *new wcParseScopes()).Parse();
	if (Output4.Error.Code != wcParserErrorCode::None)											return 5;
	if (!Output4.SymbolTable.Exists(wcFullIdentifier("methodNameWithArgs($g::int,$g::int)")))	return 6;

	string Source5 = "int methodNameWithArgs(int a, int b){ return a + b; }";
	auto Output5 = wcDeclarationParser(*new wcTokenizer(Source5, true), *new wcParseSymbolTable(), *new wcParseScopes()).Parse();
	if (Output5.Error.Code != wcParserErrorCode::None)											return 5;
	if (!Output5.SymbolTable.Exists(wcFullIdentifier("methodNameWithArgs($g::int,$g::int)")))	return 6;
	if (!Output5.SymbolTable.Exists(wcFullIdentifier("methodNameWithArgs($g::int,$g::int)::a")))	return 7;
	if (!Output5.SymbolTable.Exists(wcFullIdentifier("methodNameWithArgs($g::int,$g::int)::b")))	return 8;
	printTree(Output5.AST);

	return 0;
}

int weec::test::lex::Test_Tokenizer1()
{
	string input = listing::list_tokenizer1;
	wcTokenizer tokenizer(input);

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::FloatLiteral)
		return 1;

	return 0;
}

int weec::test::lex::Test_Tokenizer2()
{
	string input = listing::list_tokenizer2;
	wcTokenizer tokenizer(input);

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::StringLiteral)
		return 1;

	return 0;
}

int weec::test::lex::Test_Tokenizer3()
{
	string input = listing::list_tokenizer3;
	wcTokenizer tokenizer(input);

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CharLiteral)
		return 1;

	return 0;
}

int weec::test::lex::Test_Tokenizer4()
{
	string input = listing::list_tokenizer4;
	wcTokenizer tokenizer(input);

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::ConstKeyword && tokenizer.GetToken().StringToken.Data == "const")
		return 1;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
		return 2;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier || tokenizer.GetToken().StringToken.Data != "TestIdent")
		return 3;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::AssignOperator)
		return 4;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 5;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MultiplyOperator)
		return 6;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 7;

	return 0;
}

int weec::test::lex::Test_Tokenizer5()
{
	string input = listing::list_tokenizer5;
	wcTokenizer tokenizer(input);

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::StringKeyword || tokenizer.GetToken().StringToken.Data != "string")
		return 1;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier || tokenizer.GetToken().StringToken.Data != "TestIdent")
		return 2;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::AssignOperator)
		return 3;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::StringLiteral)
		return 4;

	return 0;
}

int weec::test::lex::Test_Tokenizer6()
{
	string input = listing::list_tokenizer6;
	wcTokenizer tokenizer(input);

	//- + += * / + ( ) { } [ ] # \\ / % & ^ ? |

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MinusOperator)
		return 1;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusOperator)
		return 2;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusAssignOperator)
		return 3;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MultiplyOperator)
		return 4;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::DivideOperator)
		return 5;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusOperator)
		return 6;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
		return 7;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
		return 8;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBrace)
		return 9;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBrace)
		return 10;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBracket)
		return 11;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBracket)
		return 12;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Hash)
		return 13;

	return 0;
}

int weec::test::lex::Test_Tokenizer7()
{
	string input = listing::list_tokenizer7;
	wcTokenizer tokenizer(input);

	//int main(int argc, char* argv[])
	//{
	//	return 0;
	//}

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
		return 1;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 2;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
		return 3;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
		return 4;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 5;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Comma)
		return 6;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CharKeyword)
		return 7;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MultiplyOperator)
		return 8;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 9;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBracket)
		return 10;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBracket)
		return 11;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
		return 12;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBrace)
		return 13;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::ReturnKeyword)
		return 14;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 15;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::SemiColon)
		return 16;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBrace)
		return 17;

	return 0;
}

int weec::test::lex::Test_Tokenizer8()
{
	string input = listing::list_tokenizer8;
	wcTokenizer tokenizer(input);

	//  // comment  
	//  /* multi 
	//  line 
	//  comment */
	//	with extra line //then comment at the end

	if (tokenizer.NextToken())
		return 1;

	return 0;
}

int weec::test::lex::Test_Tokenizer9()
{
	string input = listing::list_tokenizer9;
	wcTokenizer tokenizer(input);

	//  // comment  
	//  /* multi 
	//  line 
	//  comment */
	//	with extra line //then comment at the end


	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 1;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 2;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 3;

	if (tokenizer.NextToken())
		return 4;

	return 0;
}

int weec::test::lex::Test_Tokenizer10()
{
	string input = listing::list_tokenizer10;
	wcTokenizer tokenizer(input);

	//  int IdentTest; char IdentTest::WithNamespace

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
		return 1;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 2;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::SemiColon)
		return 3;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CharKeyword)
		return 4;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier || tokenizer.GetToken().StringToken.Data != "IdentTest::WithNamespace")
		return 5;

	return 0;
}

int weec::test::lex::Test_Tokenizer12()
{
	string input = listing::list_tokenizer12;
	wcTokenizer tokenizer(input);

	// ident.withMember ident::withNamespace::withObject.withMember

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier && tokenizer.GetToken().StringToken.Data == "ident.withMember")
		return 1;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier && tokenizer.GetToken().StringToken.Data == "ident::withNamespace::withObject.withMember")
		return 2;

	return 0;
}

int weec::test::lex::Test_Tokenizer11()
{
	string input = listing::list_tokenizer11;
	wcTokenizer tokenizer(input);

	//x = 88 + (42 / 99)

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 1;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::AssignOperator)
		return 2;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 3;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusOperator)
		return 4;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
		return 5;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 6;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::DivideOperator)
		return 7;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 8;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
		return 9;

	return 0;
}

/*
int weec::test::lex::Test_wcParseNode_1()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer11;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer11 << endl;
	printTree(Output.AST);

	wcInterpreter Interp(Output);
	Interp.Exec();

	return 0;
}

int weec::test::lex::Test_wcParseNode_2()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer12;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer12 << endl;
	printTree(Output.AST);

	return 0;
}

int weec::test::lex::Test_wcParseNode_3()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer13;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer13 << endl;
	printTree(Output.AST);

	return 0;
}

int weec::test::lex::Test_wcParseNode_4()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer14;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer14 << endl;
	printTree(Output.AST);

	wcInterpreter Interp(Output);
	Interp.Exec();

	return 0;
}

int weec::test::lex::Test_wcParseNode_5()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer15;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer15 << endl;
	printTree(Output.AST);

	return 0;
}

int weec::test::lex::Test_wcParseNode_6()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer16;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer16 << endl;
	printTree(Output.AST);

	return 0;
}
*/

string AnyToString(any In)
{
	if (strcmp(In.type().name(), "int") == 0)
		return to_string(any_cast<int>(In));
	else if (strcmp(In.type().name(), "unsigned int") == 0)
		return to_string(any_cast<unsigned int>(In));
	else if (strcmp(In.type().name(), "float") == 0)
		return to_string(any_cast<float>(In));
	else if (strcmp(In.type().name(), "double") == 0)
		return to_string(any_cast<double>(In));
	else if (strcmp(In.type().name(), "bool") == 0)
		return to_string(any_cast<bool>(In));
	return "";
}

bool printIsExpected(wcInterpreter Interp, any ExpectedResult)
{
	if (Interp.Error.Code != wcInterpreterErrorCode::None)
		cout << "Interp error: " << to_string((int)Interp.Error.Code) << " " << to_string(Interp.Error.Code) << endl;

	bool IsExpected = false;
	if (strcmp(Interp.Return.type().name(), "int") == 0)
	{
		if (ExpectedResult.has_value())
		{
			IsExpected = any_cast<int>(Interp.Return) == any_cast<int>(ExpectedResult);
			cout << "Expected: " << any_cast<int>(ExpectedResult) << "   ";
		}
		cout << "Out: " << any_cast<int>(Interp.Return) << endl;
	}
	else if (strcmp(Interp.Return.type().name(), "unsigned int") == 0)
	{
		if (ExpectedResult.has_value())
		{
			IsExpected = any_cast<unsigned int>(Interp.Return) == any_cast<int>(ExpectedResult);
			cout << "Expected: " << any_cast<unsigned int>(ExpectedResult) << "   ";
		}
		cout << "Out: " << any_cast<unsigned int>(Interp.Return) << endl;
	}
	else if (strcmp(Interp.Return.type().name(), "float") == 0)
	{
		if (ExpectedResult.has_value())
		{
			IsExpected = any_cast<float>(Interp.Return) == any_cast<float>(ExpectedResult);
			cout << "Expected: " << any_cast<float>(ExpectedResult) << "   ";
		}
		cout << "Out: " << any_cast<float>(Interp.Return) << endl;
	}
	else if (strcmp(Interp.Return.type().name(), "double") == 0)
	{
		if (ExpectedResult.has_value())
		{
			IsExpected = any_cast<double>(Interp.Return) == any_cast<double>(ExpectedResult);
			cout << "Expected: " << any_cast<double>(ExpectedResult) << "   ";
		}
		cout << "Out: " << any_cast<double>(Interp.Return) << endl;
	}
	else if (strcmp(Interp.Return.type().name(), "bool") == 0)
	{
		if (ExpectedResult.has_value())
		{
			IsExpected = any_cast<bool>(Interp.Return) == any_cast<bool>(ExpectedResult);
			cout << "Expected: " << any_cast<bool>(ExpectedResult) << "   ";
		}
		cout << "Out: " << any_cast<bool>(Interp.Return) << endl;
	}
	cout << "EAX: " << AnyToString(Interp.EAX) << endl;
	auto t = Interp.Return.type().name();
	cout << "Type: " << Interp.Return.type().name() << endl;
	return IsExpected;
}

int Test_ParserTemplate(string Listing)
{
	wcTokenizer tokenizer(Listing); //tokenizer.NextToken();

	auto Parsed = wcParser(tokenizer).Parse();

	cout << endl << endl << Listing << endl;

	printTree(Parsed.AST);

	if (Parsed.Error.Code != wcParserErrorCode::None)
	{
		cout << endl << "Error code: " << (int)Parsed.Error.Code << "  " << to_string(Parsed.Error.Code) << endl;
		return (int)Parsed.Error.Code;
	}

	auto Interp = wcInterpreter(Parsed);
	auto Result = Interp.Exec();

	return (Parsed.Error.Code == wcParserErrorCode::None && printIsExpected(Interp, any())) ? 0 : 1;
}


int Test_ExpressionParserTemplate(string Listing, any ExpectedResult)
{
	wcTokenizer tokenizer(Listing); //tokenizer.NextToken();

	auto Expr = wcParser(tokenizer).Parse();

	cout << endl << endl << Listing << endl;

	printTree(Expr.AST);

	if (Expr.Error.Code != wcParserErrorCode::None)
	{
		cout << endl << "Error code: " << (int)Expr.Error.Code << "  " << to_string(Expr.Error.Code) << endl;
		return (int)Expr.Error.Code;
	}
	auto Interp = wcInterpreter(Expr);
	auto Result = Interp.Exec();

	return (Expr.Error.Code == wcParserErrorCode::None && printIsExpected(Interp, ExpectedResult)) ? 0 : 1;
}

int weec::test::lex::Test_wcExpressionParser_1()
{
	return Test_ExpressionParserTemplate(listing::list_expression1, 5);
}

int weec::test::lex::Test_wcExpressionParser_60()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_59()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_58()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_57()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_56()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_55()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_54()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_53()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_52()
{
	return 0;
}

int weec::test::lex::Test_wcExpressionParser_51()
{
	return Test_ExpressionParserTemplate(listing::list_expression51, 0);
}

int weec::test::lex::Test_wcExpressionParser_2()
{
	return Test_ExpressionParserTemplate(listing::list_expression2, 6);
}

int weec::test::lex::Test_wcParser_30()
{
	return Test_ParserTemplate(listing::list_parser30);
}

int weec::test::lex::Test_wcParser_29()
{
	return Test_ParserTemplate(listing::list_parser29);
}

int weec::test::lex::Test_wcParser_28()
{
	return Test_ParserTemplate(listing::list_parser28);
}

int weec::test::lex::Test_wcParser_27()
{
	return Test_ParserTemplate(listing::list_parser27);
}

int weec::test::lex::Test_wcParser_26()
{
	return Test_ParserTemplate(listing::list_parser26);
}

int weec::test::lex::Test_wcParser_25()
{
	return Test_ParserTemplate(listing::list_parser25);
}

int weec::test::lex::Test_wcParser_24()
{
	return Test_ParserTemplate(listing::list_parser24);
}

int weec::test::lex::Test_wcParser_23()
{
	return Test_ParserTemplate(listing::list_parser23);
}

int weec::test::lex::Test_wcParser_22()
{
	return Test_ParserTemplate(listing::list_parser22);
}

int weec::test::lex::Test_wcParser_21()
{
	return Test_ParserTemplate(listing::list_parser21);
}
int weec::test::lex::Test_wcParser_20()
{
	return Test_ParserTemplate(listing::list_parser20);
}

int weec::test::lex::Test_wcParser_19()
{
	return Test_ParserTemplate(listing::list_parser19);
}

int weec::test::lex::Test_wcParser_18()
{
	return Test_ParserTemplate(listing::list_parser18);
}

int weec::test::lex::Test_wcParser_17()
{
	return Test_ParserTemplate(listing::list_parser17);
}

int weec::test::lex::Test_wcParser_16()
{
	return Test_ParserTemplate(listing::list_parser16);
}

int weec::test::lex::Test_wcParser_15()
{
	return Test_ParserTemplate(listing::list_parser15);
}

int weec::test::lex::Test_wcParser_14()
{
	return Test_ParserTemplate(listing::list_parser14);
}

int weec::test::lex::Test_wcParser_13()
{
	return Test_ParserTemplate(listing::list_parser13);
}

int weec::test::lex::Test_wcParser_12()
{
	return Test_ParserTemplate(listing::list_parser12);
}

int weec::test::lex::Test_wcParser_11()
{
	return Test_ParserTemplate(listing::list_parser11);
}

int weec::test::lex::Test_wcParser_10()
{
	return Test_ParserTemplate(listing::list_parser10);
}

int weec::test::lex::Test_wcParser_9()
{
	return Test_ParserTemplate(listing::list_parser9);
}

int weec::test::lex::Test_wcParser_8()
{
	return Test_ParserTemplate(listing::list_parser8);
}

int weec::test::lex::Test_wcParser_7()
{
	return Test_ParserTemplate(listing::list_parser7);
}

int weec::test::lex::Test_wcParser_6()
{
	return Test_ParserTemplate(listing::list_parser6);
}

int weec::test::lex::Test_wcParser_5()
{
	return Test_ParserTemplate(listing::list_parser5);
}

int weec::test::lex::Test_wcParser_4()
{
	return Test_ParserTemplate(listing::list_parser4);
}

int weec::test::lex::Test_wcParser_3()
{
	return Test_ParserTemplate(listing::list_parser3);
}

int weec::test::lex::Test_wcParser_2()
{
	return Test_ParserTemplate(listing::list_parser2);
}

int weec::test::lex::Test_wcParser_1()
{
	return Test_ParserTemplate(listing::list_parser1);
}

int weec::test::lex::Test_wcExpressionParser_3()
{
	return Test_ExpressionParserTemplate(listing::list_expression3, 89);
}

int weec::test::lex::Test_wcExpressionParser_4()
{
	return Test_ExpressionParserTemplate(listing::list_expression4, 4);
}

int weec::test::lex::Test_wcExpressionParser_5()
{
	return Test_ExpressionParserTemplate(listing::list_expression5, 133);
}

int weec::test::lex::Test_wcExpressionParser_6()
{
	return Test_ExpressionParserTemplate(listing::list_expression6, 87);
}

int weec::test::lex::Test_wcExpressionParser_7()
{
	return Test_ExpressionParserTemplate(listing::list_expression7, 48);
}

int weec::test::lex::Test_wcExpressionParser_8()
{
	return Test_ExpressionParserTemplate(listing::list_expression8, 5);
}

int weec::test::lex::Test_wcExpressionParser_9()
{
	return Test_ExpressionParserTemplate(listing::list_expression9, (float)2.5);
}

int weec::test::lex::Test_wcExpressionParser_10()
{
	return Test_ExpressionParserTemplate(listing::list_expression10, (float)(4 * 2.5 + 8.5 + 1.5 / 3.0));
}

int weec::test::lex::Test_wcExpressionParser_11()
{
	return Test_ExpressionParserTemplate(listing::list_expression11, (float)(5.0005 + 0.0095));
}

int weec::test::lex::Test_wcExpressionParser_12()
{
	return Test_ExpressionParserTemplate(listing::list_expression12, 69);
}

int weec::test::lex::Test_wcExpressionParser_13()
{
	return Test_ExpressionParserTemplate(listing::list_expression13, -5);
}

int weec::test::lex::Test_wcExpressionParser_14()
{
	return Test_ExpressionParserTemplate(listing::list_expression14, 35);
}

int weec::test::lex::Test_wcExpressionParser_15()
{
	return Test_ExpressionParserTemplate(listing::list_expression15, 2);
}
int weec::test::lex::Test_wcExpressionParser_16()
{
	return Test_ExpressionParserTemplate(listing::list_expression16, 10);
}
int weec::test::lex::Test_wcExpressionParser_17()
{
	return Test_ExpressionParserTemplate(listing::list_expression17, 6);
}
int weec::test::lex::Test_wcExpressionParser_18()
{
	return Test_ExpressionParserTemplate(listing::list_expression18, 10);
}
int weec::test::lex::Test_wcExpressionParser_19()
{
	return Test_ExpressionParserTemplate(listing::list_expression19, 6);
}

int weec::test::lex::Test_wcExpressionParser_20()
{
	return Test_ExpressionParserTemplate(listing::list_expression20, (2 * 3 - 4 * 5 + 6 / 3));
}

int weec::test::lex::Test_wcExpressionParser_21()
{
	return Test_ExpressionParserTemplate(listing::list_expression21, (2 * 3 * 4 / 8 - 5 / 2 * 4 + 6 + 0 / 3));
}

int weec::test::lex::Test_wcExpressionParser_22()
{
	return Test_ExpressionParserTemplate(listing::list_expression22, (10 / 4));
}

int weec::test::lex::Test_wcExpressionParser_23()
{
	return Test_ExpressionParserTemplate(listing::list_expression23, (5 / 3));
}

int weec::test::lex::Test_wcExpressionParser_24()
{
	return Test_ExpressionParserTemplate(listing::list_expression24, (3 + 8 / 5 - 1 - 2 * 5));
}

int weec::test::lex::Test_wcExpressionParser_25()
{
	return Test_ExpressionParserTemplate(listing::list_expression25, 0);
}
int weec::test::lex::Test_wcExpressionParser_26()
{
	return Test_ExpressionParserTemplate(listing::list_expression26, 0);
}
int weec::test::lex::Test_wcExpressionParser_27()
{
	return Test_ExpressionParserTemplate(listing::list_expression27, 0);
}
int weec::test::lex::Test_wcExpressionParser_28()
{
	return Test_ExpressionParserTemplate(listing::list_expression28, (5 + +6));
}
int weec::test::lex::Test_wcExpressionParser_29()
{
	return Test_ExpressionParserTemplate(listing::list_expression29, (-5 + 2));
}

int weec::test::lex::Test_wcExpressionParser_30()
{
	return Test_ExpressionParserTemplate(listing::list_expression30, 5 / 1);
}

int weec::test::lex::Test_wcExpressionParser_31()
{
	return Test_ExpressionParserTemplate(listing::list_expression31, 2 - 1 + 14 / 1 + 7);
}

int weec::test::lex::Test_wcExpressionParser_32()
{
	return Test_ExpressionParserTemplate(listing::list_expression32, (2));
}

int weec::test::lex::Test_wcExpressionParser_33()
{
	return Test_ExpressionParserTemplate(listing::list_expression33, (5 + 2 * 3 - 1 + 7 * 8));
}

int weec::test::lex::Test_wcExpressionParser_34()
{
	return Test_ExpressionParserTemplate(listing::list_expression34, (67 + 2 * 3 - 67 + 2 / 1 - 7));
}

int weec::test::lex::Test_wcExpressionParser_35()
{
	return Test_ExpressionParserTemplate(listing::list_expression35, ((2) + (17 * 2 - 30) * (5) + 2 - (8 / 2) * 4));
}
int weec::test::lex::Test_wcExpressionParser_36()
{
	return Test_ExpressionParserTemplate(listing::list_expression36, ((5 * 7 / 5) + (23) - 5 * (98 - 4) / (6 * 7 - 42)));
}
int weec::test::lex::Test_wcExpressionParser_37()
{
	return Test_ExpressionParserTemplate(listing::list_expression37, (((((5))))));
}
int weec::test::lex::Test_wcExpressionParser_38()
{
	return Test_ExpressionParserTemplate(listing::list_expression38, ((((2)) + 4)) * ((5)));
}
int weec::test::lex::Test_wcExpressionParser_39()
{
	return Test_ExpressionParserTemplate(listing::list_expression39, 0);
}
int weec::test::lex::Test_wcExpressionParser_40()
{
	return Test_ExpressionParserTemplate(listing::list_expression40, 0);
}
int weec::test::lex::Test_wcExpressionParser_41()
{
	return Test_ExpressionParserTemplate(listing::list_expression41, 0);
}
int weec::test::lex::Test_wcExpressionParser_42()
{
	return Test_ExpressionParserTemplate(listing::list_expression42, 0);
}
int weec::test::lex::Test_wcExpressionParser_43()
{
	return Test_ExpressionParserTemplate(listing::list_expression43, (1 == +(-1) == 1.00));
}
int weec::test::lex::Test_wcExpressionParser_44()
{
	return Test_ExpressionParserTemplate(listing::list_expression44, +(-1) * 10);
}
int weec::test::lex::Test_wcExpressionParser_45()
{
	return Test_ExpressionParserTemplate(listing::list_expression45, !2);
}
int weec::test::lex::Test_wcExpressionParser_46()
{
	return Test_ExpressionParserTemplate(listing::list_expression46, 22 && (11 + 11));
}
int weec::test::lex::Test_wcExpressionParser_47()
{
	return Test_ExpressionParserTemplate(listing::list_expression47, 0 || (33 + 33));
}
int weec::test::lex::Test_wcExpressionParser_48()
{
	return Test_ExpressionParserTemplate(listing::list_expression48, +(-2) * 10);
}
int weec::test::lex::Test_wcExpressionParser_49()
{
	return Test_ExpressionParserTemplate(listing::list_expression49, !!!2);
}
int weec::test::lex::Test_wcExpressionParser_50()
{
	return Test_ExpressionParserTemplate(listing::list_expression50, !!!!(123));
}


/*
* Parser
- should compute 5 when given 2 + 3
- should compute 6 when given 2 * 3
- should compute 89 when given 89
- should raise an error when input is empty
- should omit white spaces
- should treat dot separated floating point numbers as a valid input
- should handle tight expressions
- should calculate long additive expressions from left to right
- should calculate long multiplicative expressions from left to right
- should calculate long, mixed additive and multiplicative expressions from left to right
- should return float pointing numbers when division result is not an integer
- should raise an error on wrong token
- should raise an error on syntax error
- should return Infinity when attempt to divide by zero occurs
- should compute 2 when given (2)
- should compute complex expressions enclosed in parenthesis
- should compute expressions with many subexpressions enclosed in parenthesis
- should handle nested parenthesis
- should raise an error on unbalanced parenthesis
*
describe Parser do
  before(:each) do
	@parser = Parser.new
  end

  it 'should compute 5 when given 2 + 3' do
	@parser.parse('2 + 3').should == 5
  end

  it 'should compute 6 when given 2 * 3' do
	@parser.parse('2 * 3').should == 6
  end

  it 'should compute 89 when given 89' do
	@parser.parse('89').should == 89
  end

  it 'should raise an error when input is empty' do
	lambda {@parser.parse('')}.should raise_error()
  end

  it 'should omit white spaces' do
	@parser.parse('   12        -  8   ').should == 4
	@parser.parse('142        -9   ').should == 133
	@parser.parse('72+  15').should == 87
	@parser.parse(' 12*  4').should == 48
	@parser.parse(' 50/10').should == 5
  end

  it 'should treat dot separated floating point numbers as a valid input' do
	@parser.parse('2.5').should == 2.5
	@parser.parse('4*2.5 + 8.5+1.5 / 3.0').should == 19
	@parser.parse('5.0005 + 0.0095').should be_close(5.01, 0.01)
  end

  it 'should handle tight expressions' do
	@parser.parse('67+2').should == 69
	@parser.parse(' 2-7').should == -5
	@parser.parse('5*7 ').should == 35
	@parser.parse('8/4').should == 2
  end

  it 'should calculate long additive expressions from left to right' do
	@parser.parse('2 -4 +6 -1 -1- 0 +8').should == 10
	@parser.parse('1 -1   + 2   - 2   +  4 - 4 +    6').should == 6
  end

  it 'should calculate long multiplicative expressions from left to right' do
	@parser.parse('2 -4 +6 -1 -1- 0 +8').should == 10
	@parser.parse('1 -1   + 2   - 2   +  4 - 4 +    6').should == 6
  end

  it 'should calculate long, mixed additive and multiplicative expressions from left to right' do
	@parser.parse(' 2*3 - 4*5 + 6/3 ').should == -12
	@parser.parse('2*3*4/8 -   5/2*4 +  6 + 0/3   ').should == -1
  end

  it 'should return float pointing numbers when division result is not an integer' do
	@parser.parse('10/4').should == 2.5
	@parser.parse('5/3').should be_close(1.66, 0.01)
	@parser.parse('3 + 8/5 -1 -2*5').should be_close(-6.4, 0.01)
  end

  it 'should raise an error on wrong token' do
	lambda {@parser.parse('  6 + c')}.should raise_error()
	lambda {@parser.parse('  7 & 2')}.should raise_error()
	lambda {@parser.parse('  %')}.should raise_error()
  end

  it 'should raise an error on syntax error' do
	lambda {@parser.parse(' 5 + + 6')}.should raise_error()
	lambda {@parser.parse(' -5 + 2')}.should raise_error()
  end

  it 'should return Infinity when attempt to divide by zero occurs' do
	@parser.parse('5/0').should be_infinite
	@parser.parse(' 2 - 1 + 14/0 + 7').should be_infinite
  end

  it 'should compute 2 when given (2)' do
	@parser.parse('(2)').should == 2
  end

  it 'should compute complex expressions enclosed in parenthesis' do
	@parser.parse('(5 + 2*3 - 1 + 7 * 8)').should == 66
	@parser.parse('(67 + 2 * 3 - 67 + 2/1 - 7)').should == 1
  end

  it 'should compute expressions with many subexpressions enclosed in parenthesis' do
	@parser.parse('(2) + (17*2-30) * (5)+2 - (8/2)*4').should == 8
	@parser.parse('(5*7/5) + (23) - 5 * (98-4)/(6*7-42)').should be_infinite
  end

  it 'should handle nested parenthesis' do
	@parser.parse('(((((5)))))').should == 5
	@parser.parse('(( ((2)) + 4))*((5))').should == 30
  end

  it 'should raise an error on unbalanced parenthesis' do
	lambda {@parser.parse('2 + (5 * 2')}.should raise_error()
	lambda {@parser.parse('(((((4))))')}.should raise_error()
	lambda {@parser.parse('((2)) * ((3')}.should raise_error()
	lambda {@parser.parse('((9)) * ((1)')}.should raise_error()
  end
end
*/