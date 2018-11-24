#ifndef WC_CLASSIC_H
#define WC_CLASSIC_H
#include "compiler.h"
#include "codegen.h"
#include "lex.h"
#include "vm.h"

namespace wc
{
	namespace codegen
	{
		class wcClassicBytecodeGen : public wcBaseBytecodeGen
		{
		public:
			virtual bytecode::wcExecContext gen(parse::wcAST&);
		};
	}

	namespace lex
	{
		class wcClassicLexer : public wcLexer
		{
		public:
			wcClassicLexer();
			~wcClassicLexer();

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
		};

	}

	namespace compile
	{
		class wcClassicCompiler : public wcBaseCompiler
		{
		public:
			wcClassicCompiler();

			virtual api::wcScript compile(std::vector<std::string> input, parse::wcAST* p_outputAST = nullptr);
		};
	}


	namespace vm
	{
		class wcClassicVM : public wcBaseVM
		{
		public:
			wcClassicVM();
			virtual int execInstruction(bytecode::wcExecContext& context, bytecode::wcInstruction instr);
		};
	}
	
}


#endif