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

int main(int argc, char* argv[])
{
	weec::test::Test_AllLex();
	weec::test::Test_AllParse();
	cin;
}

int weec::test::Test_AllLex()
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

int weec::test::Test_AllParse()
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

int weec::test::Test_wcParseScope()
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

int weec::test::Test_wcParseScopes()
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

