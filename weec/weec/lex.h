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
			tt_null, tt_string, tt_int, tt_float, tt_object, tt_var, tt_bool, tt_scolon, tt_colon, tt_dcolon, tt_comma, tt_period, tt_squote,
			tt_dquote, tt_qmark, tt_pipe, tt_minus, tt_plus, tt_div, tt_mult, tt_mod, tt_expo, tt_assign, tt_underscore, tt_tilde,
			tt_oparen, tt_cparen, tt_obracket, tt_cbracket, tt_obrace, tt_cbrace, tt_bslash, tt_percent, tt_newline,
			tt_dollar, tt_amper, tt_greater, tt_less, tt_equal, tt_notequal, tt_greaterequal, tt_lessequal,
			tt_lognot, tt_logor, tt_logand, tt_incr, tt_decr, tt_plusassign, tt_minusassign, tt_multassign, tt_divassign,
			tt_intlit, tt_strlit, tt_fltlit, tt_ident, tt_comment, tt_chr, tt_const,
			tt_true, tt_false, tt_namespace, tt_function, tt_if, tt_else, tt_elseif, tt_while, tt_break, tt_key_return, tt_continue,
			tt_varident, tt_funcident, tt_lvalue,
			
			tt_asm_key_add, tt_asm_key_sub, tt_asm_key_mul,
			tt_asm_key_div, tt_asm_key_mov, tt_asm_key_cmp,
			tt_asm_key_inc, tt_asm_key_dec, tt_asm_key_and,
			tt_asm_key_or, tt_asm_key_not, tt_asm_key_jmp,
			tt_asm_key_je, tt_asm_key_jne, tt_asm_key_jz,
			tt_asm_key_jg, tt_asm_key_jge, tt_asm_key_jl,
			tt_asm_key_jle, tt_asm_key_call,
			tt_asm_key_ret, tt_asm_key_push, tt_asm_key_pop,

			tt_tab, tt_ws, tt_eos
		};

		const std::unordered_multimap<std::string, wcTokenType> tokenStrings =
		{
			{ "" , tt_null },
			{ "\n" , tt_newline },{ " ", tt_ws },{ "0", tt_intlit },
			{ "1", tt_intlit },{ "2", tt_intlit },{ "3", tt_intlit },
			{ "4", tt_intlit },{ "5", tt_intlit },{ "6", tt_intlit },
			{ "7", tt_intlit },{ "8", tt_intlit },{ "9", tt_intlit },
			{ "\t", tt_tab },{ ".", tt_period },{ ",", tt_comma },
			{ "+", tt_plus, },{ "-", tt_minus },{ "/", tt_div },
			{ "*", tt_mult },{ "%", tt_mod },{ "^", tt_expo },
			{ "'", tt_squote },{ "\"", tt_dquote },{ "=",tt_assign },
			{ "|",tt_pipe },{ "$",tt_dollar },{ "&",tt_amper },
			{ "!", tt_lognot },{ ">",tt_greater },{ "<",tt_less },
			{ ";",tt_scolon },{ ":",tt_colon },{ "(",tt_oparen },
			{ ")",tt_cparen },{ "_",tt_underscore },{ "{",tt_obrace },
			{ "}",tt_cbrace },{ "[",tt_obracket },{ "]",tt_cbracket },
			{ "++", tt_incr },{ "--", tt_decr },{ "+=", tt_plusassign },
			{ "-=", tt_minusassign },{ "*=", tt_multassign },{ "/=", tt_divassign },
			{ "==", tt_equal },{ "!=", tt_notequal },{ ">=", tt_greaterequal },
			{ "<=", tt_lessequal },{ "!",  tt_lognot },{ "||", tt_logor },
			{ "&&", tt_logand },{ "::", tt_dcolon },{ "\\", tt_bslash },
			{ "chr", tt_chr },{ "var", tt_var },{ "obj", tt_object },
			{ "int", tt_int },{ "str", tt_string },{ "flt", tt_float },
			{ "bool", tt_bool },{ "true", tt_true },{ "false", tt_false },
			{ "const", tt_const },{ "namespace", tt_namespace },{ "func", tt_function },
			{ "if", tt_if },{ "else", tt_else },{ "return", tt_key_return },{ "`",tt_tilde },
			{ "while", tt_while },{ "break", tt_break },{ "continue", tt_continue },
		};

		const std::vector<wcTokenType> delimTypes =
		{
			tt_ws,	 tt_newline,	 tt_tab,	 tt_period,
			tt_comma,	 tt_plus,	 tt_minus,	 tt_div,
			tt_mult,	 tt_expo, 	 tt_mod, 	 tt_squote,
			tt_assign,	 tt_pipe, 	 tt_dollar,	 tt_amper,
			tt_greater, tt_less, 	 tt_lognot,	 tt_dquote,
			tt_bslash,	 tt_scolon,	 tt_percent, tt_oparen,
			tt_cparen,  tt_obrace,  tt_cbrace,  tt_obracket,
			tt_cbracket, tt_tilde,  tt_colon
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

		class wcLexer
		{
		public:
			wcLexer();
			~wcLexer();

			void reset();

			std::vector<wcToken> lex(const char *);
			std::vector<wcToken> lex(std::string);
			virtual std::vector<wcToken> lex(std::vector<std::string>);

			bool isDelim(std::string);
			virtual bool isDelim(wcTokenType);
			bool isDelimDroppable(wcToken);
			virtual bool isDelimDroppable(wcTokenType);
			virtual bool isPunctuation(wcTokenType);

			virtual wcTokenType deriveTokenType(const char *);
			virtual wcTokenType deriveTokenType(char);
			virtual wcTokenType deriveTokenType(std::string);

			bool isError();
			error::wcError getError();
			void setError(error::wcError);

		protected:
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