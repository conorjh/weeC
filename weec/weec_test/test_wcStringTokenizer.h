#ifndef WC_TEST_WCIDENTIFIER_H
#define WC_TEST_WCIDENTIFIER_H
#include <string>
#include <vector>
#include "lex.h"

namespace weec
{
	namespace test
	{
		namespace listing
		{
		}

		int Test_StringTokenizer1()
		{
			using namespace std;
			using namespace weec::lex;

			string input = "Hello\nWorld\nLonger sentence\nFinishing with newline\n";
			wcStringTokenizer strTok(input);

			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "Hello")											return 1;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::NewLine)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "World")											return 2;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::NewLine)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "Longer")											return 3;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::Whitespace)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "sentence")										return 5;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::NewLine)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "Finishing")										return 6;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::Whitespace)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "with")											return 7;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::Whitespace)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "newline" || strTok.GetStringToken().Line != 4)	return 8;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::NewLine)						return 4;

			if (strTok.NextStringToken())	//end of stream
				return 9;

			return 0;
		}


		int Test_StringTokenizer_Alphanumeric()
		{
			using namespace std;
			using namespace weec::lex;

			string input = "identifier _identifierWithUnderscore ";
			wcStringTokenizer strTok(input);

			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "Hello")											return 1;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::NewLine)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "World")											return 2;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::NewLine)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "Longer")											return 3;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::Whitespace)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "sentence")										return 5;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::NewLine)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "Finishing")										return 6;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::Whitespace)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "with")											return 7;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::Whitespace)						return 4;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Data != "newline" || strTok.GetStringToken().Line != 4)	return 8;
			if (!strTok.NextStringToken() || strTok.GetStringToken().Type != wcStringTokenType::NewLine)						return 4;

			if (strTok.NextStringToken())	//end of stream
				return 9;

			return 0;
		}



	}
}

#endif