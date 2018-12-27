#ifndef WC_LEX_H
#define WC_LEX_H
#include <string>
#include <vector>
#include <unordered_map>
#include "error.h"

#define CASE_TT_WS case tt_ws: case tt_newline: case tt_tab:

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
			tt_intlit, tt_strlit, tt_fltlit,  tt_comment, 
			tt_ident, tt_varident, tt_funcident,

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
			tt_scolon, tt_colon, tt_dcolon, tt_comma, tt_period, tt_squote, tt_dquote, 
			tt_dollar, tt_amper, tt_bslash,
			tt_qmark, tt_pipe, tt_underscore, tt_tilde,
			tt_tab, tt_ws, tt_newline, tt_eos, tt_eof
		};
		
		//provides a way to index your read progress
		struct wcLexInputStreamIndex
		{
			wcLexInputStreamIndex(wcLexInputStream&);

			wcLexInputStreamIndex operator-(int);
			wcLexInputStreamIndex operator+(int);
			wcLexInputStreamIndex operator--();
			wcLexInputStreamIndex operator++();
			wcLexInputStreamIndex operator=(wcLexInputStreamIndex);

			bool isValid();
			void reset();

		private:
			int line, column, index;
			wcLexInputStream& source;
		};

		//provides a way to store and get text input 
		struct wcLexInputStream
		{
		public:
			wcLexInputStream();
			wcLexInputStream(const char *), wcLexInputStream(std::string);
			wcLexInputStream(std::vector<std::string>), wcLexInputStream(std::vector<const char *>);

			const char * next(wcLexInputStreamIndex&), 
				get(wcLexInputStreamIndex&),
				peek(wcLexInputStreamIndex&);

			 const unsigned int size();

		private:
			wcLexInputStreamIndex index;
			std::vector<const char *> container;
		};

		struct wcToken
		{
			wcToken(wcTokenType, const char *, int, int);

			bool operator!=(const wcToken&) const;
			bool operator==(const wcToken&) const;
			const char * operator[](unsigned int);
			wcToken operator=(const char *);

			wcTokenType type;
			const char * data;
			int line, col;
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

		struct wcTokenStreamIndex
		{
			wcTokenStreamIndex();
			wcTokenStreamIndex(wcTokenStreamIndex&);

			wcTokenStreamIndex operator-(int);
			wcTokenStreamIndex operator+(int);
			wcTokenStreamIndex operator--();
			wcTokenStreamIndex operator++();
			wcTokenStreamIndex operator=(wcTokenStreamIndex);

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

			bool isError();

			wcToken next(wcTokenStreamIndex&),
				get(wcTokenStreamIndex&),
				peek(wcTokenStreamIndex&);

			std::vector<wcToken> container;
			const error::wcError error;
		};

		//lexes string input encapsulated in wcLexInputStreams
		class wcLexer
		{
		public:
			wcLexer();
			~wcLexer();

			virtual wcTokenStream lex(wcLexInputStream&);

		protected:
			wcTokenTypeDeriver deriver;
			wcTokenDefinitionBank definitionsBank;
		};

		//free floating lexer helpers
		bool lex_stringLiteral(std::vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, error::wcError& p_error);
		bool lex_intLiteral(std::vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, error::wcError& p_error);
		bool lex_ws(std::vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, error::wcError& p_error);
		bool lex_2step(std::vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, error::wcError& p_error);
		bool lex_default(std::vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, error::wcError& p_error);
		bool lex_comment(std::vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, error::wcError& p_error);
		bool lex_commentMultiLine(std::vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, error::wcError& p_error);
		bool setErrorReturnFalse(error::wcError& p_error, error::wcError p_newError);
		std::vector<std::string> tokenizeString(std::string);
	}

}
#endif