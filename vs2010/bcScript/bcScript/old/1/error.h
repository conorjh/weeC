#pragma once 
enum bcErrorCode{
					ec_null,ec_unk,
					
					ec_lex_emptyinput,
					
					ec_par_emptyinput, ec_par_eof,ec_par_invalidtoken, ec_par_invalidident,ec_par_divbyzero, ec_par_nocparen, 
					ec_par_invalidcparen, ec_par_invalidbrace, ec_par_invalidbreak, ec_par_invalidcontinue, ec_par_invalidreturn,
					ec_par_invalidop, ec_par_undeclaredident, ec_par_redeclaredident, ec_par_noscolon, ec_par_illegaloperation, ec_par_emptyexpression, 
					ec_par_identinvalidscope,ec_par_paramsdontmatch, ec_par_illegaldec, 
					
					ec_cg_emptyinput,ec_cg_nohead,ec_cg_unexpectednode
				};