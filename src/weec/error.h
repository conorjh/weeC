#ifndef WC_ERROR_H
#define WC_ERROR_H
#include <unordered_map>


namespace wc
{
	enum wcErrorCode
	{
		ec_null,
		ec_p_undeclaredsymbol,			//symbol is not in symbol table
		ec_p_redefinition,				//attempting to create a symbol with a fullIdent already in use
		ec_p_unexpectedtoken,			//...
		ec_p_invalidsymbol,				//symbol is declared, but cannot be accessed from here
		ec_p_badparams,					//a function call uses the wrong parameters
		ec_p_nonintsubscript,			//array[t]    t must be int
		ec_p_expmustbeconst,			//an expression (usually array subscripts) should have consisted entirely of consts
		ec_p_expmustbearray,			//an expression should have returned array 
		ec_p_illegalidentcall,			//lexIdent encountered an error
		ec_p_undeclaredtype				//identifer is not a type - normally in from parsing a declaration (func, var etc)
	};

	const std::unordered_multimap<wcErrorCode, std::string> errorStrings =
	{
		{ ec_null, "No error" },
		{ ec_p_undeclaredsymbol, "Undeclared symbol" },		
		{ ec_p_redefinition, "Symbol name already in use" },
		{ ec_p_unexpectedtoken,	"Unexpected input" },
		{ ec_p_invalidsymbol, "Symbol cannot be accessed from this scope" },
		{ ec_p_badparams, "The parameters are incorrect for this function" },
		{ ec_p_nonintsubscript,	"Array elements can only be addressed with an Integer" },
		{ ec_p_expmustbeconst, "Expression must be const" },
		{ ec_p_expmustbearray, "Expression must return an array" },
		{ ec_p_illegalidentcall, "Illegal ident" },
		{ ec_p_undeclaredtype, "Symbol is not a type"}
	};

}
#endif //WC_ERROR_H