#ifndef WC_ERROR_H
#define WC_ERROR_H

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
		ec_p_expmustbeconst,				//an expression (usually array subscripts) should have consisted entirely of consts
		ec_p_expmustbearray,				//an expression should have returned array 
		ec_p_illegalidentcall			//lexIdent encountered an error
	};

}
#endif //WC_ERROR_H