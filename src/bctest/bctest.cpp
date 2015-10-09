#include "bctest.h"
#include "..\bcScript\bcscript.h"
#include "..\bcc\bcc.h"
#include "..\bcvm\bcvm.h"
#include <iostream>

using namespace std;
using namespace bc;
using namespace bc::test;

//test resources


int bc::test::test_all()
{
	test_lexer();
	test_loadScriptFromFile();

	return 1;
}

int bc::test::test_lexer()
{
	cout << "test_lexer" << endl;

	return 1;
}

int bc::test::test_loadScriptFromFile()
{
	cout << "test_loadScriptFromFile" << endl;
	
	
	return 1;
}

