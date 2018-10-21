#ifndef WC_WASM_H
#define WC_WASM_H
#include <vector>
#include "lex.h"
#include "parse.h"
#include "codegen.h"
#include "compiler.h"

namespace wc
{
	namespace wasm
	{
		namespace lex
		{
			const std::unordered_multimap<std::string, wc::lex::wcTokenType> wasmTokenStrings =
			{
			{ "" , wc::lex::tt_null },
			{ "\n" , wc::lex::tt_newline },{ " ", wc::lex::tt_ws },{ "0", wc::lex::tt_intlit },
			{ "1", wc::lex::tt_intlit },{ "2", wc::lex::tt_intlit },{ "3", wc::lex::tt_intlit },
			{ "4", wc::lex::tt_intlit },{ "5", wc::lex::tt_intlit },{ "6", wc::lex::tt_intlit },
			{ "7", wc::lex::tt_intlit },{ "8", wc::lex::tt_intlit },{ "9", wc::lex::tt_intlit },
			{ "\t", wc::lex::tt_tab },{ ".", wc::lex::tt_period },{ ",", wc::lex::tt_comma },
			{ "+", wc::lex::tt_plus, },{ "-", wc::lex::tt_minus },{ "/", wc::lex::tt_div },
			{ "*", wc::lex::tt_mult },{ "%", wc::lex::tt_mod },{ "^", wc::lex::tt_expo },
			{ "'", wc::lex::tt_squote },{ "\"", wc::lex::tt_dquote },{ "=",wc::lex::tt_assign },
			{ "|",wc::lex::tt_pipe },{ "$",wc::lex::tt_dollar },{ "&",wc::lex::tt_amper },
			{ "!", wc::lex::tt_lognot },{ ">",wc::lex::tt_greater },{ "<",wc::lex::tt_less },
			{ ";",wc::lex::tt_scolon },{ ":",wc::lex::tt_colon },{ "(",wc::lex::tt_oparen },
			{ ")",wc::lex::tt_cparen },{ "_",wc::lex::tt_underscore },{ "{",wc::lex::tt_obrace },
			{ "}",wc::lex::tt_cbrace },{ "[",wc::lex::tt_obracket },{ "]",wc::lex::tt_cbracket },
			{ "++", wc::lex::tt_incr },{ "--", wc::lex::tt_decr },{ "+=", wc::lex::tt_plusassign },
			{ "-=", wc::lex::tt_minusassign },{ "*=", wc::lex::tt_multassign },{ "/=", wc::lex::tt_divassign },
			{ "==", wc::lex::tt_equal },{ "!=", wc::lex::tt_notequal },{ ">=", wc::lex::tt_greaterequal },
			{ "<=", wc::lex::tt_lessequal },{ "!",  wc::lex::tt_lognot },{ "||", wc::lex::tt_logor },
			{ "&&", wc::lex::tt_logand },{ "::", wc::lex::tt_dcolon },{ "\\", wc::lex::tt_bslash },

			{ "add", wc::lex::tt_asm_key_add },{ "sub", wc::lex::tt_asm_key_sub },{ "mul", wc::lex::tt_asm_key_mul },
			{ "div", wc::lex::tt_asm_key_div },{ "mov", wc::lex::tt_asm_key_mov },{ "cmp", wc::lex::tt_asm_key_cmp },
			{ "inc", wc::lex::tt_asm_key_inc },{ "dec", wc::lex::tt_asm_key_dec },{ "and", wc::lex::tt_asm_key_and },
			{ "or", wc::lex::tt_asm_key_or },{ "not", wc::lex::tt_asm_key_not },{ "jmp", wc::lex::tt_asm_key_jmp },
			{ "je", wc::lex::tt_asm_key_je },{ "jne", wc::lex::tt_asm_key_jne },{ "jz", wc::lex::tt_asm_key_jz },
			{ "jg", wc::lex::tt_asm_key_jg },{ "jge", wc::lex::tt_asm_key_jge },{ "jl", wc::lex::tt_asm_key_jl },
			{ "jle", wc::lex::tt_asm_key_jle },{ "cmp", wc::lex::tt_asm_key_cmp },{ "call", wc::lex::tt_asm_key_call },
			{ "ret", wc::lex::tt_asm_key_ret },{ "push", wc::lex::tt_asm_key_push },{ "pop", wc::lex::tt_asm_key_pop },

			{ "const", wc::lex::tt_const },{ "namespace", wc::lex::tt_namespace },{ "func", wc::lex::tt_function },
			{ "if", wc::lex::tt_if },{ "else", wc::lex::tt_else },{ "return", wc::lex::tt_key_return },{ "`",wc::lex::tt_tilde },
			{ "while", wc::lex::tt_while },{ "break", wc::lex::tt_break },{ "continue", wc::lex::tt_continue },
			};


			class wasmLexer : public wc::lex::wcLexer
			{
			public:
				wasmLexer();
				~wasmLexer();

				virtual std::vector<wc::lex::wcToken> lex(std::vector<std::string>);
				virtual wc::lex::wcTokenType deriveTokenType(const char *);
			};
		}

		namespace parse
		{
			class wasmParser : public wc::parse::wcParser
			{
			public:
				wasmParser();
				~wasmParser();

				virtual wc::parse::wcAST parse(std::vector<wc::lex::wcToken>);
			};
		}

		namespace codegen
		{

		}
	}
}

#endif
