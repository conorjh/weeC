#ifndef WC_ERROR_H
#define WC_ERROR_H
#include <unordered_map>
namespace wc
{
	namespace lex
	{
		struct wcToken;
	}

	namespace error
	{
		enum wcErrorCode
		{
			ec_null,

			ec_lex_eos,					//end of stream whilst a token was expected
			ec_lex_somekindoferror,		//generic error placeholder
			ec_lex_eos_strlit,			//end of stream encountered whilst lexing a string literal
			ec_lex_unexpectedtoken,		//unacceptable token encountered

			ec_par_eos,					//eos whilst parsing
			ec_par_unexpectedtoken,		//unexpected token encountered
			ec_par_malformedident,		//ident not syntactically correct
			ec_par_undeclaredident,		//ident unknown at this point
			ec_par_identoutofscope,		//ident used outside it's valid scope(s)
			ec_par_invalidsymbol,		//symbol not valid in given context
			ec_par_illegalstatement,	//statement inside a namespace normally
			ec_par_redeclaration,

			ec_vm_unknownopcode			//unknown opcode encoutered during execution
		};


		const std::unordered_multimap<int, std::string> errorStrings =
		{
			{ec_null,"No error" },

			{ec_lex_eos,				"Unexpected end of input whilst lexing"},
			{ec_lex_somekindoferror,	"Some kind of error whilst lexing"},
			{ec_lex_eos_strlit,			"Unexpected end of input whilst lexing a string literal"},
			{ec_lex_unexpectedtoken,	"Unexpected token encountered whilst lexing"},

			{ec_par_eos,				"Unexpected end of input whilst parsing"},
			{ec_par_unexpectedtoken,	"Unexpected token"},
			{ec_par_malformedident,		"Malformed identifier"},
			{ec_par_undeclaredident,	"Undeclared identifier"},
			{ec_par_identoutofscope,	"Identifier invalid in this scope"},
			{ec_par_invalidsymbol,		"Symbol cannot be used in this context" },
			{ec_par_illegalstatement,	"Statement illegal in this context" },
			{ec_par_redeclaration,		"Symbol redeclaration"}

		};

		struct wcError
		{
			wcError() { code = ec_null; text = ""; line = col = 0; };
			wcError(wcErrorCode p_ec);
			wcError(wcErrorCode p_ec, std::string p_text);
			wcError(wcErrorCode p_ec, std::string p_text, int p_line, int p_col);
			wcError(wcErrorCode p_ec, wc::lex::wcToken p_token);
			bool operator==(const wcError& p_err) const;
			bool operator==(const wcErrorCode& p_errCode) const;
			bool operator!=(const wcError& p_err) const;
			bool operator!=(const wcErrorCode& p_errCode) const;

			wcErrorCode code;
			std::string text;
			int line, col;
		};
	}
}

#endif