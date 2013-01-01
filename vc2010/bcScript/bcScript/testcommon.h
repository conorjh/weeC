#pragma once
#include "lexer.h"
#include <iostream>

void PrintLex(bc::lexer::bcLexer* l)
{
	std::cout << "Lexer Output " << std::endl;
	for(int t=0;t<l->Get()->size();++t)
	{
		std::cout << "00" << t << "	" << l->Get()->at(t).type << std::endl;
	}
	std::cout << std::endl;
	std::string c;
	std::cin >> c;
}