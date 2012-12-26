#pragma once
#include "types.h"
#include "error.h"
#include <vector>
#include <map>

//Lexer consts
//Lexer
const bool bcLexer_CheckEmptyInput = false;

//Post Lexer
const bool bcLexer_DropWS = true;
const bool bcLexer_DropComments = true;
const bool bcLexer_TrimWS = false;

class bcLexer
{
public:
	bcLexer();

	int Setup(vector<string>*);
	
	bcToken* GetToken();	//return current token
	bcToken* NextToken();	//inc index, return next token	
	bcToken* PeekToken();	//return next token without inc index

	vector<bcToken>* Get();

private:
	int index;
	vector<string>* in;
	vector<bcToken>* out;
}

/*
class bcLexer
{

public:
	bcLexer();
	bcLexer(vector<string>*);

	int xindex,yindex;
	bool Inc();

	int Lex();
	int Lex(vector<string>*);

	vector<bcToken> data;
	vector<string>* in;
	map<string,bcTokenType> tokens;
	vector<bcTokenType> delims;	
	bool error;
	bcErrorCode errorcode;
	
	bcTokenType GetTokenType(string);
	bcTokenType GetTokenType(char);
	bool IsDelim(char);
	bool IsDelim(string);

	bool IsError();
	bcErrorCode GetError();
	
};
*/