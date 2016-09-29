#ifndef WEEC_LEXER_H
#define WEEC_LEXER_H
#include <unordered_map>
#include <string>
#include <vector>

namespace wc
{
	namespace lex
	{
		enum wcTokenType
		{
			tt_null, tt_string, tt_int, tt_float, tt_object, tt_var, tt_bool, tt_scolon, tt_colon, tt_dcolon, tt_comma, tt_period, tt_squote,
			tt_dquote, tt_qmark, tt_pipe, tt_minus, tt_plus, tt_div, tt_mult, tt_mod, tt_expo, tt_assign, tt_underscore, tt_tilde,
			tt_oparen, tt_cparen, tt_obracket, tt_cbracket, tt_obrace, tt_cbrace, tt_bslash, tt_percent, tt_newline,
			tt_dollar, tt_amper, tt_greater, tt_less, tt_equal, tt_notequal, tt_greaterequal, tt_lessequal,
			tt_lognot, tt_logor, tt_logand, tt_incr, tt_decr, tt_plusassign, tt_minusassign, tt_multassign, tt_divassign,
			tt_intlit, tt_strlit, tt_fltlit, tt_ident, tt_comment, tt_chr, tt_const,
			tt_true, tt_false, tt_namespace, tt_function, tt_if, tt_else, tt_elseif, tt_while, tt_break, tt_return, tt_continue,
			tt_varident, tt_funcident, tt_lvalue,

			tt_tab, tt_ws, tt_eof
		};
		
		
		struct wcToken
		{
			wcToken() {};
			wcToken(wcTokenType ty) { type = ty; };
			wcToken(std::string data_) { type = tt_ident; data = data_; };
			wcToken(wcTokenType ty, std::string data_) { type = ty; data = data_; };
			std::string data;
			wcTokenType type;
			unsigned int x, y;
		};

		struct wcLexerData
		{
			std::vector<wcToken> tokens;
			std::vector<std::string>* source;
			bool done;
			int oldx, oldy, x, y, offset;
			const std::unordered_multimap<std::string, wcTokenType> tokenStrings =
			{
				{ "\n" , tt_newline },{ " ", tt_ws },{ "0", tt_intlit },
				{ "1", tt_intlit },{ "2", tt_intlit },{ "3", tt_intlit },
				{ "4", tt_intlit },{ "5", tt_intlit },{ "6", tt_intlit },
				{ "7", tt_intlit },{ "8", tt_intlit },{ "9", tt_intlit },
				{ "\t", tt_tab },{ ".", tt_period },{ ",", tt_comma },
				{ "+", tt_plus, },{ "-", tt_minus },{ "/", tt_div },
				{ "*", tt_mult },{ "%", tt_mod },{ "^", tt_expo },
				{ "\"", tt_squote },{ "\"", tt_dquote },{ "=",tt_assign },
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
				{ "if", tt_if },{ "else", tt_else },{ "return", tt_return },{ "`",tt_tilde },
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


		};

		class wcLexer
		{
		public:
			wcLexer();
			void clear();

			wcToken* nextToken();
			wcToken* getToken();
			wcToken* prevToken();
			wcToken* peekToken();
			bool finished();

			void rewind();
			bool inc(), dec();
			std::string getChar(), peekChar();

			wcLexerData data;
		};

		//helpers
		int lexInput(wcLexer*);

		std::vector<std::string> tokenizeString(std::string);
		wcTokenType getTokenType(wcLexer*, std::string);
		wcTokenType getTokenType(std::string);
		bool isDelim(wcLexer*, std::string); 
		bool isDelim(std::string);

	}
}
#endif //WEEC_LEXER_H
