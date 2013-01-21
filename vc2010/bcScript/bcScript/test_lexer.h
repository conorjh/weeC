#pragma once
#include "testcommon.h"

int test_lexer_basic()
{
	vector<string> source;
	bcreadfile("test.txt",&source);
	std::string c;	
	std::cin >> c;
	bc::lexer::bcLexer lex(&source);
	lex.Lex();
	PrintLex(&lex);	
	return 1;
}

int test_lexer_bigger()
{
	vector<string> source;
	bcreadfile("twbar.cpp",&source);
	std::string c;
	std::cin >> c;
	bc::lexer::bcLexer lex(&source);
	lex.Lex();
	PrintLex(&lex);	
	return 1;
}