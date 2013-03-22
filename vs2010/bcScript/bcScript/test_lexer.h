#pragma once
#include "testcommon.h"

int test_lexer_console()
{
	vector<string> source;	
	std::string in;
	bc::lexer::bcLexer lex;	

	while(true)
	{
		in="";
		std::cin>>in;
		lex.Setup(in);
		lex.Lex();
		PrintLex(&lex);						
	}
	std::cin>>in;
	return 1;
}

int test_lexer_basic()
{
	vector<string> source;
	bcreadfile("example\\helloworld.bcs",&source);
	std::string c;	
	std::cin >> c;
	bc::lexer::bcLexer lex(&source);
	lex.Lex();
	PrintLex(&lex);	
	std::cin>>c;
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