#pragma once
#include <string>
#include <vector>
#include <map>

using namespace std;


	//bottom level construct
	enum bcTokenType{tt_null, tt_unk , tt_empty , tt_string, tt_int, tt_float, tt_object, tt_var, tt_bool, tt_scolon, tt_colon, tt_comma, tt_period, tt_squote,
					tt_dquote, tt_qmark, tt_pipe, tt_minus, tt_plus, tt_div, tt_mult, tt_mod, tt_pow, tt_assign, tt_underscore, tt_tilde,
					tt_oparen, tt_cparen, tt_obracket, tt_cbracket, tt_obrace, tt_cbrace, tt_bslash, tt_percent, tt_newline,
					tt_dollar, tt_amper, tt_greater, tt_less, tt_equal, tt_notequal, tt_greaterequal, tt_lessequal,
					tt_lognot, tt_logor, tt_logand, tt_incr, tt_decr, tt_plusassign, tt_minusassign, tt_multassign, tt_divassign,
					tt_intlit, tt_strlit, tt_fltlit, tt_ident, tt_comment, tt_dec,
					tt_true, tt_false, tt_function, tt_if, tt_else, tt_while, tt_break, tt_return, tt_continue,
					tt_varident,tt_funcident,
				
					tt_tab, tt_ws,tt_eof
					};



	//top level construct
	enum bcParseNodeType{pn_null,pn_unk, pn_head,
						pn_exp, pn_type,
					
						//straight copies of their tt_ counterparts
						pn_string, pn_int, pn_float, pn_object, pn_var, pn_bool, pn_scolon, pn_colon, pn_comma, pn_period, pn_squote,
						pn_dquote, pn_qmark, pn_exclam, pn_minus, pn_plus, pn_div, pn_mult, pn_mod, pn_pow, pn_assign, pn_underscore, pn_tilde,
						pn_oparen, pn_cparen, pn_obracket, pn_cbracket, pn_obrace, pn_cbrace, pn_bslash, pn_percent, pn_newline,
						pn_dollar, pn_amper, pn_greater, pn_less, pn_greaterequal, pn_lessequal, pn_equal, pn_notequal, pn_logand, pn_logor, pn_lognot,
						pn_intlit, pn_strlit, pn_fltlit, pn_ident, pn_comment, pn_dec, pn_true, pn_false, pn_function, pn_incr, pn_decr,pn_plusassign, pn_minusassign, pn_multassign, pn_divassign,
	
						pn_assignment, pn_block,pn_declaration,pn_paramdeclaration,pn_funcdec,pn_paramlist,pn_decparamlist,pn_funccall, pn_if,pn_else,
						pn_while,pn_break, pn_return, pn_continue,};
					


	enum bcSymbolType{
						st_null, st_unk, st_var, st_type, st_object, st_function, st_namespace
					};

	enum bcOpcode{
						oc_null,oc_mov,oc_push,oc_pop,oc_call,oc_var,oc_add,oc_sub,oc_div,oc_mult,oc_pow,oc_mod,oc_inc,oc_dec,oc_ltn,oc_gtn,oc_leq, oc_geq,
						oc_eq,oc_neq,oc_land,oc_lor
					};

	enum bcOperandType{
							ot_null,ot_unk,ot_memref,ot_strlit,ot_instrindex,ot_ident,ot_int,ot_string,ot_float,ot_bool,ot_object,
							ot_stackindex,ot_stackindextop,ot_register
	};

	enum bcValType{
							rt_null, rt_instrindex, rt_int, rt_flt, rt_string, rt_bool, rt_float, rt_object, rt_stackindex, rt_stackindextop, rt_register
	};

	struct bcToken
	{
		int line;
		int col;
		bcTokenType type;
		string data;
	};

	//node for our parsetree, contains an index to lexer.data
	struct bcParseNode
	{
		bcParseNodeType type;
		vector<int> tokens;	//index of tokens in bcLexer.data
		string tag;			//used to hold internal function names currently TODO find a better way to do this!
	};

	struct bcSymbol
	{
		string ident;
		bcSymbolType type;
		string datatype;
		map<string,bcSymbol>::iterator scope;
		int stackindex;
		string init;
	};

	struct bcOperand
	{
		bcOperandType type;
		string data;
	};

	struct bcInstr
	{
		bcInstr()														{opcode=oc_null;operands.reserve(3);};
		bcInstr(bcOpcode oc,vector<bcOperand> ops)						{opcode=oc;operands=ops;};
		bcInstr(bcOpcode oc,bcOperand op1)								{opcode=oc;operands.push_back(op1);};
		bcInstr(bcOpcode oc,bcOperand op1,bcOperand op2)				{opcode=oc;operands.push_back(op1);operands.push_back(op2);};
		bcInstr(bcOpcode oc,bcOperand op1,bcOperand op2,bcOperand op3)	{opcode=oc;operands.push_back(op1);operands.push_back(op2);operands.push_back(op3);};

		int GetOpCount();
		int AddOp(bcOperand);
		int RemoveOp(int);
	
		bcOpcode opcode;
		vector<bcOperand> operands;
	};

	struct bcVal
	{
		string data;
		bcValType type;
	};
