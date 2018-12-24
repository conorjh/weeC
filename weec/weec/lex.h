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
			tt_null, tt_keyword_string, tt_keyword_int, tt_keyword_float, tt_object, tt_var, tt_keyword_bool, tt_scolon, tt_colon, tt_dcolon, tt_comma, tt_period, tt_squote,
			tt_dquote, tt_qmark, tt_pipe, tt_minus, tt_plus, tt_div, tt_mult, tt_mod, tt_expo, tt_assign, tt_underscore, tt_tilde,
			tt_oparen, tt_cparen, tt_obracket, tt_cbracket, tt_obrace, tt_cbrace, tt_bslash, tt_percent, tt_newline,
			tt_dollar, tt_amper, tt_greater, tt_less, tt_equal, tt_notequal, tt_greaterequal, tt_lessequal,
			tt_lognot, tt_logor, tt_logand, tt_incr, tt_decr, tt_plusassign, tt_minusassign, tt_multassign, tt_divassign,
			tt_intlit, tt_strlit, tt_fltlit, tt_ident, tt_comment, tt_keyword_chr, tt_const,
			tt_keyword_true, tt_keyword_false, tt_keyword_namespace, tt_keyword_function, tt_keyword_if, tt_keyword_else, tt_elseif, tt_keyword_while, tt_keyword_break, tt_keyword_return, tt_keyword_continue,
			tt_varident, tt_funcident, tt_lvalue,
			
			tt_asm_keyword_add, tt_asm_keyword_sub, tt_asm_keyword_mul,
			tt_asm_keyword_div, tt_asm_keyword_mov, tt_asm_keyword_cmp,
			tt_asm_keyword_inc, tt_asm_keyword_dec, tt_asm_keyword_and,
			tt_asm_keyword_or, tt_asm_keyword_not, tt_asm_keyword_jmp,
			tt_asm_keyword_je, tt_asm_keyword_jne, tt_asm_keyword_jz,
			tt_asm_keyword_jg, tt_asm_keyword_jge, tt_asm_keyword_jl,
			tt_asm_keyword_jle, tt_asm_keyword_call,
			tt_asm_keyword_ret, tt_asm_keyword_push, tt_asm_keyword_pop,

			tt_tab, tt_ws, tt_eos
		};
		
		struct wcLexIndex
		{
			wcLexIndex();

			std::string nextChar();
			std::string getChar();
			std::string peekChar();

			bool isValid();
			void reset();
			unsigned int getSize();

			int line, column, index;
			std::vector<std::string>* source;
		};

		struct wcToken
		{
			wcToken();
			wcToken(wcTokenType);
			wcToken(wcTokenType, std::string);
			wcToken(wcTokenType, std::string, int, int);
			wcToken(wc::parse::wcSymbol);
			wcToken(std::string);

			bool operator==(const wcToken&) const;
			bool operator!=(const wcToken&) const;

			wcTokenType type;
			std::string data;
			int line, col;
		};

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
			void populaterDelimiterTypes();
			const std::vector<wcTokenDefinition> definitions;
			std::vector<wcTokenType> delimiterTypes;
		};

		class wcTokenStream
		{

		};

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

		class wcLexer
		{
		public:
			wcLexer();
			~wcLexer();

			std::vector<wcToken> lex(const char *);
			std::vector<wcToken> lex(std::string);
			virtual std::vector<wcToken> lex(std::vector<std::string>);

			bool isError();
			error::wcError getError();
			void setError(error::wcError);

		protected:
			wcTokenDefinitionBank definitionsBank;
			wcTokenTypeDeriver deriver;
			wcLexIndex lexIndex;
			error::wcError error;
		};

		//free floating lexer helpers
		bool lex_stringLiteral(std::vector<wcToken>& p_output, wcLexIndex& p_index, error::wcError& p_error);
		bool lex_intLiteral(std::vector<wcToken>& p_output, wcLexIndex& p_index, error::wcError& p_error);
		bool lex_ws(std::vector<wcToken>& p_output, wcLexIndex& p_index, error::wcError& p_error);
		bool lex_2step(std::vector<wcToken>& p_output, wcLexIndex& p_index, error::wcError& p_error);
		bool lex_default(std::vector<wcToken>& p_output, wcLexIndex& p_index, error::wcError& p_error);
		bool lex_comment(std::vector<wcToken>& p_output, wcLexIndex& p_index, error::wcError& p_error);
		bool lex_commentMultiLine(std::vector<wcToken>& p_output, wcLexIndex& p_index, error::wcError& p_error);
		bool setErrorReturnFalse(error::wcError& p_error, error::wcError p_newError);
		std::vector<std::string> tokenizeString(std::string);
	}

}
#endif