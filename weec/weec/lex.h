#ifndef WC_LEX_H
#define WC_LEX_H
#include <string>
#include <vector>
#include <unordered_map>
#include "error.h"

#define CASE_TT_WS case tt_ws: case tt_newline: case tt_tab:
#define CASE_ALL_2_PARTERS 	case tt_greater: case tt_less: case tt_lognot: case tt_assign: case tt_plus: case tt_minus:case tt_amper: case tt_pipe: case tt_colon:

namespace wc
{
	namespace parse
	{
		struct wcSymbol;
	}

	namespace lex
	{
		const bool wc_lexer_dropWS = true;

		enum wcTokenType
		{
			tt_null, 

			//value types
			tt_intlit, tt_strlit, tt_fltlit, tt_charlit,

			//operators
			tt_minus, tt_plus, tt_div, tt_mult, tt_mod, tt_expo, tt_incr, tt_decr, 
			tt_assign, tt_plusassign, tt_minusassign, tt_multassign, tt_divassign,
			tt_oparen, tt_cparen, tt_obracket, tt_cbracket, tt_obrace, tt_cbrace,  
			tt_greater, tt_less, tt_equal, tt_notequal, tt_greaterequal, tt_lessequal, 
			tt_lognot, tt_logor, tt_logand, 

			//reserved words
			tt_keyword_true, tt_keyword_false, 
			tt_keyword_string, tt_keyword_int, tt_keyword_float, tt_keyword_bool, tt_keyword_char, tt_keyword_object, tt_keyword_var, 
			tt_keyword_namespace, tt_keyword_function, tt_keyword_if, tt_keyword_else, tt_elseif, 
			tt_keyword_while, tt_keyword_break,  tt_keyword_continue, tt_keyword_return, tt_keyword_const,

			//misc
			tt_comment, tt_ident, 
			tt_scolon, tt_colon, tt_dcolon, tt_comma, tt_period, tt_squote, tt_dquote, 
			tt_dollar, tt_amper, tt_bslash, tt_qmark, tt_pipe, tt_underscore, tt_tilde,
			tt_tab, tt_ws, tt_newline, tt_eos, tt_eof
		};
		
		//provides a way to index your read progress
		struct wcLexInputStream;
		struct wcLineColumnIndex;
		struct wcLexInputStreamIndex
		{
			wcLexInputStreamIndex(wcLexInputStream&);

			wcLexInputStreamIndex operator=(wcLexInputStreamIndex),
				operator=(int),
				operator=(wcLineColumnIndex),
				operator--(),	operator--(int),
				operator++(),	operator++(int),
				operator-(int),
				operator+(int),
				operator-(wcLexInputStreamIndex),
				operator+(wcLexInputStreamIndex);
			const char * operator[](int);

			bool isValid();
			void reset();

			const char *next(),	*get(),
				*get(int line, int column);

			unsigned int size();

			int line, column, index;
		private:
			wcLexInputStream& source;
		};

		//use to convert from index to line/col, or line/col to index
		struct wcLineColumnIndex
		{
			wcLineColumnIndex(wcLexInputStream& source, int index);
			wcLineColumnIndex(wcLexInputStream& source, int line, int column);

			bool updateFromIndex(int index);
			bool updateFromLineColumn(int line, int column);
			bool isValid();

			int line, column, index;
			wcLexInputStream& source;
		};

		//provides a way to store and get text input 
		struct wcLexInputStream
		{
		public:
			wcLexInputStream();
			wcLexInputStream(const char *);
			wcLexInputStream(std::string);
			wcLexInputStream(std::vector<std::string>);
			wcLexInputStream(std::vector<const char *>);
			const char *operator[](int);
			bool operator!=(const wcLexInputStream&) const;
			bool operator==(const wcLexInputStream&) const;

			const char *next(wcLexInputStreamIndex&),
				*get(wcLexInputStreamIndex&),
				*get(int line, int column),
				*getLine(wcLexInputStreamIndex&),
				*getLine(int line);

			const unsigned int size(), size(unsigned int lineNumber);
			const unsigned int lines();

		private:
			std::vector<const char *> container;
		};

		//a lexed string with a type
		struct wcToken
		{
			wcToken();	//tt_null
			wcToken(wcTokenType, const char *, int, int);

			bool operator!=(const wcToken&) const;
			bool operator==(const wcToken&) const;
			const char * operator[](unsigned int);
			wcToken operator=(const char *);
			wcToken operator=(wcToken);

			wcTokenType type;
			const char * data;
			int line, column;
		};

		//matches string literals to wcTokenTypes
		struct wcTokenDefinition
		{
			wcTokenDefinition();	//tt_null
			wcTokenDefinition(wcTokenType _type, const char *);
			wcTokenDefinition(wcTokenType _type, const char *, bool _isDelimiter);
			wcTokenDefinition(wcTokenType _type, std::vector<std::string> _identifiers);
			wcTokenDefinition(wcTokenType _type, std::vector<std::string> _identifiers, bool _isDelimiter);

			const bool isSingleCharacterToken();
			const bool isNull();

			const wcTokenType type;
			const bool delimiter;
			const std::vector<std::string> identifiers;
		};

		//definitions used for creating wcToken
		struct wcTokenDefinitionBank
		{
			wcTokenDefinitionBank();
			wcTokenDefinitionBank(const std::vector<wcTokenDefinition>&);

			const bool exists(const char *),
				exists(char),
				exists(std::string);

			const wcTokenDefinition find(const char *),
				find(char),
				find(std::string);

		private:
			void populateDelimiterTypes();
			const std::vector<wcTokenDefinition> definitions;
			std::vector<wcTokenType> delimiterTypes;
		};
		
		//because enum type wcTokenType cant have a constructor
		class wcTokenTypeDeriver
		{
		public:
			wcTokenTypeDeriver();
			wcTokenTypeDeriver(wcTokenDefinitionBank&);

			wcTokenType derive(const char *),
				derive(char),
				derive(std::string);

			bool isDelim(const char *),
				isDelim(char),
				isDelim(std::string),
				isDelim(wcTokenType);

		private:
			wcTokenDefinitionBank& definitionsBank;
		};

		struct wcTokenStream;
		//a way to track read progress in a token stream
		struct wcTokenStreamIndex
		{
			wcTokenStreamIndex(wcTokenStream&);

			wcTokenStreamIndex operator-(int),
				operator+(int),
				operator-(wcTokenStreamIndex),
				operator+(wcTokenStreamIndex),
				operator--(),
				operator++(),
				operator=(wcTokenStreamIndex);

			bool isValid();
			void reset();

		private:
			int line, column, index;
			wcTokenStream& source;
		};

		//a way to store and read wcTokens, output from wcLexer
		struct wcTokenStream
		{
			wcTokenStream();
			wcTokenStream(error::wcError);
			wcTokenStream operator+(wcTokenStream),
				operator+(wcToken),
				operator+=(wcTokenStream),
				operator+=(wcToken);

			bool isError();

			wcToken next(wcTokenStreamIndex&),
				get(wcTokenStreamIndex&),
				peek(wcTokenStreamIndex&);

			std::vector<wcToken> container;
			error::wcError error;
		};

		//lexes string input encapsulated in wcLexInputStreams
		class wcLexer
		{
		public:
			wcLexer();
			~wcLexer();

			wcTokenStream lex(wcLexInputStream&);

		protected:
			wcTokenTypeDeriver deriver;
			wcTokenDefinitionBank definitionsBank;

			bool expect(wcTokenType);
			wcTokenStream lex_stringLiteral(wcLexInputStreamIndex& index),
				lex_intLiteral(wcLexInputStreamIndex& index),
				lex_ws(wcLexInputStreamIndex& index),
				lex_2step(wcLexInputStreamIndex& index), 
				lex_default(wcLexInputStreamIndex& index),
				lex_comment(wcLexInputStreamIndex& index),
				lex_commentMultiLine(wcLexInputStreamIndex& index);
		};

		bool setErrorReturnFalse(error::wcError& p_error, error::wcError p_newError);
		std::vector<std::string> tokenizeString(std::string);
	}

}
#endif