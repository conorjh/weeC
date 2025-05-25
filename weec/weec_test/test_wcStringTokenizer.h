#ifndef WC_TEST_WCSTRINGTOKENIZER_H
#define WC_TEST_WCSTRINGTOKENIZER_H
#include <string>
#include "test_listings.h"
#include "lex.h"

namespace weec
{
	namespace test
	{


		inline int Test_StringTokenizer_whitespace()
		{
			using namespace std;
			using namespace weec::lex;

			string input1 = listing::list_lexer_whitespace1;
			wcStringTokenizer strTok1(input1);
			if (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)	return 1;
			if (strTok1.NextStringToken())	return 1;	//end of file

			string input2 = listing::list_lexer_whitespace2;
			wcStringTokenizer strTok2(input2);
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::Whitespace)	return 2;
			if (strTok2.NextStringToken())	return 2;	//end of file

			string input3 = listing::list_lexer_whitespace3;
			wcStringTokenizer strTok3(input3);
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::NewLine)	return 3;
			if (strTok3.NextStringToken())	return 3;	//end of file

			string input4 = listing::list_lexer_whitespace4;
			wcStringTokenizer strTok4(input4);
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::Whitespace)	return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::NewLine)	return 4;
			if (strTok4.NextStringToken())	return 4;	//end of file

			string input5 = listing::list_lexer_whitespace5;
			wcStringTokenizer strTok5(input5);
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::Whitespace)	return 5;
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::NewLine)	return 5;;
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::Whitespace)	return 5;
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::NewLine)	return 5;
			if (strTok5.NextStringToken())	return 5;	//end of file

			string input6 = listing::list_lexer_whitespace6;
			wcStringTokenizer strTok6(input6);
			if (!strTok6.NextStringToken() || strTok6.GetStringToken().Type != wcStringTokenType::Whitespace)	return 6;
			if (strTok6.NextStringToken())	return 6;	//end of file

			string input7 = listing::list_lexer_whitespace7;
			wcStringTokenizer strTok7(input7);
			if (!strTok7.NextStringToken() || strTok7.GetStringToken().Type != wcStringTokenType::Whitespace)	return 7;
			if (strTok7.NextStringToken())	return 7;	//end of file

			string input8 = listing::list_lexer_whitespace8;
			wcStringTokenizer strTok8(input8);
			if (!strTok8.NextStringToken() || strTok8.GetStringToken().Type != wcStringTokenType::NewLine)	return 8;
			if (!strTok8.NextStringToken() || strTok8.GetStringToken().Type != wcStringTokenType::NewLine)	return 8;
			if (!strTok8.NextStringToken() || strTok8.GetStringToken().Type != wcStringTokenType::NewLine)	return 8;
			if (!strTok8.NextStringToken() || strTok8.GetStringToken().Type != wcStringTokenType::NewLine)	return 8;
			if (strTok8.NextStringToken())	return 8;	//end of file

			string input9 = listing::list_lexer_whitespace9;
			wcStringTokenizer strTok9(input9);
			if (!strTok9.NextStringToken() || strTok9.GetStringToken().Type != wcStringTokenType::Whitespace)	return 9;
			if (strTok9.NextStringToken())	return 9;	//end of file

			return 0;
		}

		inline int Test_StringTokenizer_comments()
		{
			using namespace std;
			using namespace weec::lex;

			string input1 = listing::list_lexer_comments1;
			wcStringTokenizer strTok1(input1);
			if (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok1.GetStringToken().Data != "/")	return 1;
			if (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok1.GetStringToken().Data != "/")	return 1;
			if (strTok1.NextStringToken())	return 1;	//end of file

			string input2 = listing::list_lexer_comments2;
			wcStringTokenizer strTok2(input2);
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok2.GetStringToken().Data != "/")	return 2;
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok2.GetStringToken().Data != "/")	return 2;
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok2.GetStringToken().Data != "comment")	return 2;
			if (strTok2.NextStringToken())	return 2;	//end of file

			string input3 = listing::list_lexer_comments3;
			wcStringTokenizer strTok3(input3);
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok3.GetStringToken().Data != "/")	return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok3.GetStringToken().Data != "/")	return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok3.GetStringToken().Data != "comment")	return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::NewLine)	return 3;
			if (strTok3.NextStringToken())	return 3;	//end of file

			string input4 = listing::list_lexer_comments4;
			wcStringTokenizer strTok4(input4);
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok4.GetStringToken().Data != "/")	return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok4.GetStringToken().Data != "/")	return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::Whitespace) return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok4.GetStringToken().Data != "comment")	return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::NewLine)	return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok4.GetStringToken().Data != "/")	return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok4.GetStringToken().Data != "/")	return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok4.GetStringToken().Data != "second")	return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::Whitespace) return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok4.GetStringToken().Data != "comment")	return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::NewLine)	return 4;
			if (!strTok4.NextStringToken() || strTok4.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok4.GetStringToken().Data != "ident")	return 4;
			if (strTok4.NextStringToken())	return 4;	//end of file

			string input5 = listing::list_lexer_comments5;
			wcStringTokenizer strTok5(input5);
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::Whitespace) return 5;
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok5.GetStringToken().Data != "/")	return 5;
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok5.GetStringToken().Data != "/")	return 5;
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::Whitespace) return 5;
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok5.GetStringToken().Data != "comment")	return 5;
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::Whitespace) return 5;
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::NewLine)	return 5;
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::Whitespace) return 5;
			if (!strTok5.NextStringToken() || strTok5.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok5.GetStringToken().Data != "ident")	return 5;
			if (strTok5.NextStringToken())	return 5;	//end of file

			return 0;
		}


		inline int Test_StringTokenizer_multiline_comments()
		{
			using namespace std;
			using namespace weec::lex;

			// /*multiline\ncomment*/
			string input1 = listing::list_lexer_multiline_comment1;
			wcStringTokenizer strTok1(input1);
			if (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok1.GetStringToken().Data != "/")	return 1;
			if (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok1.GetStringToken().Data != "*")	return 1;
			if (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok1.GetStringToken().Data != "multiline")	return 1;
			if (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::NewLine) return 1;
			if (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok1.GetStringToken().Data != "comment")	return 1;
			if (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok1.GetStringToken().Data != "*")	return 1;
			if (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok1.GetStringToken().Data != "/")	return 1;
			if (strTok1.NextStringToken())	return 1;	//end of file

			// /*multiline\ncomment*/
			string input2 = listing::list_lexer_multiline_comment2;
			wcStringTokenizer strTok2(input2);
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok2.GetStringToken().Data != "/")	return 2;
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok2.GetStringToken().Data != "*")	return 2;
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok2.GetStringToken().Data != "multi")	return 2;
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::NewLine) return 2;
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok2.GetStringToken().Data != "line")	return 2;
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::NewLine) return 2;
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok2.GetStringToken().Data != "comment")	return 2;
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok2.GetStringToken().Data != "*")	return 2;
			if (!strTok2.NextStringToken() || strTok2.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok2.GetStringToken().Data != "/")	return 2;
			if (strTok2.NextStringToken())	return 2;	//end of file

			// /*\n\tmulti  \n\t\tline\n\t\t\tcomment\n*/
			string input3 = listing::list_lexer_multiline_comment3;
			wcStringTokenizer strTok3(input3);
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok3.GetStringToken().Data != "/")	return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok3.GetStringToken().Data != "*")	return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::NewLine) return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Whitespace) return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok3.GetStringToken().Data != "multi")	return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Whitespace) return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::NewLine) return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Whitespace) return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok3.GetStringToken().Data != "line")	return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::NewLine) return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Whitespace) return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Alpha
				|| strTok3.GetStringToken().Data != "comment")	return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::NewLine) return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok3.GetStringToken().Data != "*")	return 3;
			if (!strTok3.NextStringToken() || strTok3.GetStringToken().Type != wcStringTokenType::Punctuation
				|| strTok3.GetStringToken().Data != "/")	return 3;
			if (strTok3.NextStringToken())	return 3;	//end of file

			return 0;

		}

		inline int Test_StringTokenizer_keywords()
		{
			using namespace std;
			using namespace weec::lex;
			//"true false string double int uint float bool char void object var namespace func if else while break continue return inline const print struct";

			string input1 = listing::list_lexer_keywords;
			wcStringTokenizer strTok1(input1);
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "true")	
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace))	return 1;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "false")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 2;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "string")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 3;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "double")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 4;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "int")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 5;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "uint")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 6;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "float")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 7;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "bool")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 7;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "char")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 8;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "void")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 9;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "object")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 10;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "var")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 11;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "namespace")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 12;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "func")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 13;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "if")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 14;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "else")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 15;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "while")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 16;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "break")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 17;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "continue")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 18;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "return")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 19;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "inline")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 20;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "const")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 21;
			if ((!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "print")
				|| (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Whitespace)) return 22;
			if (!strTok1.NextStringToken() || strTok1.GetStringToken().Type != wcStringTokenType::Alpha || strTok1.GetStringToken().Data != "struct")
				return 23;
			if (strTok1.NextStringToken()) return 24;	 //end of file

			return 0;
		}



		inline int Test_StringTokenizer_Alphanumeric()
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