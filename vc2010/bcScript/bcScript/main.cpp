#include "bcscript.h"
#include <Windows.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdio>
#include "test.h"


int main()
{
	vector<string> source;// = LoadText();
	bcreadfile("test2.txt",&source);
	
	test_lexer_basic();
	return 1;
}