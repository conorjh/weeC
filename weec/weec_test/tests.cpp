#include "tests.h"
#include "error.h"
#include <algorithm>
#include <iostream>

using namespace std;
using namespace wctest::lex;
using namespace wctest::parse;
using namespace wctest::gen;
using namespace wctest::vm;

namespace wctest
{

}

int wctest::test_runall()
{

	cout << "Running all tests..." << endl;

	//lex tests
	cout << "Running lex tests..." << endl;
	for (int t = 0; t < allLexTests.size(); ++t)
	{
		int testResult = allLexTests[t]();
		if(testResult)
			cout << "\tTest #" << t + 1 << " failed: " << testResult << "	" << wc::error::errorStrings.find(testResult)->second << endl;
		else
			cout << "\tTest #" << t + 1 << " passed" << endl;
	}

	//parse tests
	cout << "Running parse tests..." << endl;
	for (int t = 0; t < allParseTests.size(); ++t)
	{
		int testResult = allParseTests[t]();
		if (testResult)
			cout << "\tTest #" << t + 1 << " failed: " << testResult << "	" << wc::error::errorStrings.find(testResult)->second << endl;
		else
			cout << "\tTest #" << t + 1 << " passed" << endl;

	}

	//bytecode generation tests
	cout << "Running bytecode generation tests..." << endl;
	for (int t = 0; t < allGenTests.size(); ++t)
	{
		int testResult = allGenTests[t]();
		if (testResult)
			cout << "\tTest #" << t + 1 << " failed: " << testResult << "	" << wc::error::errorStrings.find(testResult)->second << endl;
		else
			cout << "\tTest #" << t + 1 << " passed" << endl;
	}

	//vn tests
	cout << "Running VM tests..." << endl;
	for (int t = 0; t < allVMTests.size(); ++t)
	{
		int testResult = allVMTests[t]();
		if (testResult)
			cout << "\tTest #" << t + 1 << " failed: " << testResult << "	" << wc::error::errorStrings.find(testResult)->second << endl;
		else
			cout << "\tTest #" << t + 1 << " passed" << endl;
	}

	//compiler tests


	//complete
	cout << "All tests completed" << endl;

	return 0;
}