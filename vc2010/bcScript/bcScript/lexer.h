#pragma once
#include "types.h"
#include "error.h"
#include <vector>

namespace bc
{
	namespace lexer
	{
		bool IsDelim(string);
		bcTokenType DeriveType(string);
		bcTokenType DeriveType(char);

		class bcLexer
		{
		public:
			bcLexer();
			bcLexer(vector<string>*);
			bcLexer(string);

			int	Setup(vector<string>*);
			int	Setup(string);
			bool IsError();
			int	GetError();
			vector<bcToken>* Get()	{return out;};	//get output

			int Lex();				//lex entire input
			bcToken* GetToken();	//return current token
			bcToken* NextToken();	//inc index, return next token	
			bcToken* PeekToken();	//return next token without inc'ing index		

		private:
			bool IncIndex();
			bool DecIndex();
			string GetChar();
			string PeekChar();		

			bool done;
			bool iserror;
			int error;
			int xindex,yindex;
			vector<string>* in;
			vector<bcToken>* out;
		};
	}
}
