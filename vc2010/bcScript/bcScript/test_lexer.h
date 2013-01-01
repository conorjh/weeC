#pragma once
#include "lexer.h"
#include "testcommon.h"



int test_lexer_basic()
{
	bc::lexer::bcLexer lex("//  one two 223 44 +");
	lex.Lex();
	PrintLex(&lex);
	return 1;
}