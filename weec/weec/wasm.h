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

			{ "add", tt_asm_key_add },{ "sub", tt_asm_key_sub },{ "mul", tt_asm_key_mul },
			{ "div", tt_asm_key_div },{ "mov", tt_asm_key_mov },{ "cmp", tt_asm_key_cmp },
			{ "inc", tt_asm_key_inc },{ "dec", tt_asm_key_dec },{ "and", tt_asm_key_and },
			{ "or", tt_asm_key_or },{ "not", tt_asm_key_not },{ "jmp", tt_asm_key_jmp },
			{ "je", tt_asm_key_je },{ "jne", tt_asm_key_jne },{ "jz", tt_asm_key_jz },
			{ "jg", tt_asm_key_jg },{ "jge", tt_asm_key_jge },{ "jl", tt_asm_key_jl },
			{ "jle", tt_asm_key_jle },{ "cmp", tt_asm_key_cmp },{ "call", tt_asm_key_call },
			{ "ret", tt_asm_key_ret },{ "push", tt_asm_key_push },{ "pop", tt_asm_key_pop },

			{ "const", tt_const },{ "namespace", tt_namespace },{ "func", tt_function },
			{ "if", tt_if },{ "else", tt_else },{ "return", tt_return },{ "`",tt_tilde },
			{ "while", tt_while },{ "break", tt_break },{ "continue", tt_continue },
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
