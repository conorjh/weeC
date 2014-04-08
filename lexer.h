#pragma once
#include <string>
#include <vector>

namespace bc
{
	namespace lex
	{
		enum bcTokenType
		{
			tt_null, tt_string, tt_int, tt_float, tt_object, tt_var, tt_bool, tt_scolon, tt_colon, tt_dcolon, tt_comma, tt_period, tt_squote,
			tt_dquote, tt_qmark, tt_pipe, tt_minus, tt_plus, tt_div, tt_mult, tt_mod, tt_pow, tt_assign, tt_underscore, tt_tilde,
			tt_oparen, tt_cparen, tt_obracket, tt_cbracket, tt_obrace, tt_cbrace, tt_bslash, tt_percent, tt_newline,
			tt_dollar, tt_amper, tt_greater, tt_less, tt_equal, tt_notequal, tt_greaterequal, tt_lessequal,
			tt_lognot, tt_logor, tt_logand, tt_incr, tt_decr, tt_plusassign, tt_minusassign, tt_multassign, tt_divassign,
			tt_intlit, tt_strlit, tt_fltlit, tt_ident, tt_comment, tt_dec,
			tt_true, tt_false, tt_function, tt_if, tt_else, tt_while, tt_break, tt_return, tt_continue,
			tt_varident,tt_funcident,
				
			tt_tab, tt_ws,tt_eof				
		};
	
		struct bcToken
		{
			std::string data;
			bcTokenType type;
			unsigned int x,y;
		};

		class bcLexer
		{
		public:
			void startup();
			void shutdown();

			int lex();
			void clear();
			bcToken* nextToken();
			bcToken* getToken();
			bcToken* peekToken();
			bool inc(),dec();
			std::string getChar(),peekChar();

			std::vector<bcToken> tokens;
			std::vector<std::string>* source;
		private:
			bool done;
			int x,y;
		};

		bcTokenType getTokenType(std::string*);
		bcTokenType getTokenType(char);
		bool isDelim(std::string*);
	}
}