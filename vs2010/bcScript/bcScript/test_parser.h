#pragma once
#include "parser.h"
#include "testcommon.h"


int test_parser_console()
{
	vector<string> source;	
	std::string in;
	bc::lexer::bcLexer lex;		
	bc::parser::bcParser parser;
	

	while(true)
	{
		in="";
		std::cin>>in;
		lex.Setup(in);
		lex.Lex();
		PrintLex(&lex);
		parser.Setup(lex.Get());
		parser.Parse();
		//PrintParser(&parser);
	}
	std::cin>>in;
	return 1;
}

int test_parser_bigger()
{
	vector<string> source;
	bc::parser::bcParser parser;
	bcreadfile("example\\isbigger.bcs",&source);
	std::string c;
	std::cin >> c;
	bc::lexer::bcLexer lex(&source);
	lex.Lex();
	PrintLex(&lex);	
	parser.Setup(lex.Get());
	parser.Parse();
	return 1;
}