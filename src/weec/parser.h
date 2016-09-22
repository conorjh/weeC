#ifndef WEEC_PARSER_H
#define WEEC_PARSER_H

#include <unordered_map>
#include "error.h" 
#include "lexer.h"
#include "include\tree.hh"

#define CASE_BASIC_TYPES_TT case tt_bool: case tt_int: case tt_float: case tt_string: case tt_chr: case tt_object: case tt_var:
#define CASE_ALL_LITERALS_TT case tt_intlit: case tt_strlit: case tt_fltlit: case tt_true: case tt_false:
#define CASE_ALL_ARITHMETIC_TT case tt_assign: case tt_mult: case tt_div: case tt_plus: case tt_minus:
#define CASE_ALL_OPERATORS_TT case tt_assign: case tt_logor: case tt_logand: case tt_equal: case tt_notequal: case tt_greater:case tt_less: case tt_lessequal: case tt_greaterequal: case tt_mult: case tt_div: case tt_plus: case tt_minus:	case tt_lognot:
#define CASE_ALL_BOOLEAN_OPERATORS_TT case tt_less: case tt_greater: case tt_lessequal: case tt_greaterequal: case tt_equal: case tt_notequal: case tt_logand: case tt_logor:

namespace wc
{
	namespace vm
	{
		enum wcValType
		{
			vt_null,
			vt_instr,		//instruction (istream ref)
			vt_mem,			//objects and dynamic memory in the newstore
			vt_reg,			//registers
			vt_stack,		//local stack
			vt_astack,		//an absolute entire stack reference 
			vt_int,			//integer
			vt_float,		//float	(2*int)
			vt_string,		//string table ref
			vt_bool,		//boolean
			vt_funccall,	//function call	(fstream ref)
			vt_vmcall		//vm function call (c++ code)
		};

		struct wcVal
		{
			wcValType type;
			int val;
		};
	}

	namespace parse
	{
		enum wcSymbolType 
		{
			st_null, st_namespace, st_var, st_type, st_utype, st_object, st_function
		};

		//holds all information for a symbol
		struct wcSymbol
		{
			wcSymbol();
			std::string ident;		//local identifier varname
			std::string fullIdent;	//fully qualified name $global::varname
			std::string dataType;	//same as the wcSymbol::ident of this vars type
			wcSymbolType type;		//pointer to type symbol
			unsigned int size;		//number of sequential copies in memory
			int	offset;				//filled in by gen, local stack offset
			bool	isConst,		//constant data, read only
					isArray,		//simple c array []
					isStatic;		//static variable, the same value across many stackFrames
		};

		enum wcParseNodeType {
			pn_null, pn_head, pn_exp, pn_type,

			//straight copies of their tt_ counterparts
			pn_str, pn_int, pn_flt, pn_obj, pn_var, pn_bool, pn_scolon, pn_colon, pn_comma,
			pn_period, pn_squote, pn_dquote, pn_qmark, pn_exclam, pn_minus, pn_plus, pn_pipe, pn_div,
			pn_mult, pn_mod, pn_expo, pn_assign, pn_underscore, pn_tilde, pn_oparen, pn_cparen,
			pn_obracket, pn_cbracket, pn_obrace, pn_cbrace, pn_bslash, pn_percent, pn_newline, pn_dollar,
			pn_amper, pn_greater, pn_less, pn_greaterequal, pn_lessequal, pn_equal, pn_notequal,
			pn_logand, pn_logor, pn_lognot, pn_intlit, pn_strlit, pn_fltlit, pn_ident, pn_comment,
			pn_arrayindex, pn_varident, pn_funcident, pn_namespaceident,
			pn_dec, pn_true, pn_false, pn_function, pn_incr, pn_decr, pn_plusassign, pn_minusassign,
			pn_multassign, pn_divassign, pn_assignment, pn_block, pn_vardec, pn_paramdec, pn_funcdec, pn_namespacedec,
			pn_paramlist, pn_decparamlist, pn_funccall, pn_if, pn_else, pn_if_trueblock, pn_if_elseblock, pn_while, pn_break,
			pn_return, pn_continue,
			pn_statement, pn_negate
		};

		struct wcParseNode
		{
			wcParseNode(); wcParseNode(lex::wcTokenType t);
			wcParseNode(wcSymbolType t); wcParseNode(wcParseNodeType t) { type = t; };
			wcParseNode(wcParseNodeType t, lex::wcToken tk) { type = t; tokens.push_back(tk); };
			wcParseNodeType type;
			std::vector<lex::wcToken> tokens;		//index of tokens in wcLexer.data
		};

		struct wcExpression
		{
			wcExpression();
			std::string dataType, rpn;	//dataTypes fullIdent,reverse polish notation (postfix) conversion
			tree<wcParseNode>::iterator node;
			bool	isConst,	//all operands are const
					isBool,		//leftmost operand is bool
					isFunc,		//leftmost operand is a function
					isArray,	//leftmost operand is an array
					isAssign;	//operator is tt_assign
		};

		struct wcParamList
		{
			std::vector<std::string> params;	//fullIdent of type
		};

		struct wcStackFrameInfo
		{
			std::vector<std::string> paramVars, localVars;
		};

		struct wcFuncInfo
		{
			std::string ident, fullIdent, dataType;
			bool isOverloaded;														//has overloaded method signatures
			int gOffset, lOffset;													//global stack offset, and local stack offset for local vars
			unsigned int sfIndex;													//stackframe index within wcExecContext.stackFrames
			wcStackFrameInfo stackFrame;		
			tree<wcParseNode>::iterator body;		//method signature to parsenode 
			wcParamList	params;					//method signature to paramlist (inc methodsig)
		};																			//if(!isOverloaded) sigs[0] is functions signature

		struct wcAST
		{
			//symbol table
			wcSymbol* getSymbol(std::string);
			wcSymbol* getSymbol(std::string, wcSymbol*);
			bool addSymbol(wcSymbol*);
			bool addSymbol(std::string, wcSymbol*);
			void clear();

			tree<wcParseNode> tree;
			std::vector<wcStackFrameInfo> stackFrames;				//vector of all possible stackframes
			std::unordered_map<std::string, wcSymbol> symTab;		//wcSymbol.fullIdent to wcSymbol
			std::unordered_map<std::string, wcFuncInfo> funcTab;	//wcFuncInfo.fullIdent to funcinfo
			std::unordered_map<std::string, vm::wcVal> constTab;	//fullIdent to constant wcVal
		};

		class wcAsmParser
		{};

		class wcParser
		{
		public:
			void startup(), shutdown();
			int parse();
			void clear();

			//Parse tree
			tree<wcParseNode>::iterator getNode();
			tree<wcParseNode>::iterator prevNode();
			tree<wcParseNode>::iterator addNode(wcParseNode);	//add node and point pindex to the new child node
			tree<wcParseNode>::iterator addChild(wcParseNode);	//add node, but pindex remains on parent node
			void parent();

			//error
			unsigned int getError();
			unsigned int getErrorLine();
			unsigned int getErrorCol();
			std::string getErrorString();
			void setError(wcErrorCode, std::string);		//line and column from lexer->getToken()
			void setError(wcErrorCode, int, int, std::string);

			//data
			wcAST ast;
			unsigned int parenCount;
			int sOffset;
			bool noDecVar, noDecFunc, noDecName;
			tree<wcParseNode>::iterator lastIdent;
			wcErrorCode error;
			int errorL, errorC;
			std::string errorS;
			wcSymbol* currentScope;
			wcFuncInfo* currentFunc;
			std::string currentMethodStringSig;
			wcParamList* currentParamList;
			wcStackFrameInfo* currentStackFrame;
			lex::wcLexer* lexer;
			tree<wcParseNode>::iterator pindex;		//Current parse node we are working on	
		};

		//Recursive parsing methods			
		//level 1
		void parseStatement(wcParser*);
		void parseBlock(wcParser*);

		//level 2
		void parseDecFunc(wcParser*);
		void parseDecVar(wcParser*);
		void parseDecNamespace(wcParser*);
		void parseIf(wcParser*);
		void parseWhile(wcParser*);
		void parseAssignment(wcParser*, wcParseNode);
		void parseFuncCall(wcParser*, wcParseNode);
		wcExpression parseFExp(wcParser*);
		wcExpression parseFExp(wcParser*, wcParseNode);

		//level 3
		wcParseNode parseIdent(wcParser*);
		void parseArrayIndex(wcParser*, wcParseNode*);
		void parseType(wcParser*);
		void parseParamListCall(wcParser*, wcSymbol*);
		void parseParamListDec(wcParser*);
		void parseSColon(wcParser*);
		void parseBreak(wcParser*);
		void parseReturn(wcParser*);
		void parseContinue(wcParser*);
		void parseDecInParam(wcParser*);
		wcExpression	parseExp(wcParser*);
		lex::wcToken	parseSubExp(wcParser*, wcExpression*);
		lex::wcToken	parseTerm(wcParser*, wcExpression*);
		lex::wcToken	parseFactor(wcParser*, wcExpression*);

		//type compatability
		bool checkOperandTypes(wcParser*, lex::wcToken oper1, lex::wcToken op, lex::wcToken oper2);
		wcParseNodeType DeriveType(lex::wcToken);
		//int getTypeSize(wcSymbol*);
		//size
		int getTypeSize(lex::wcToken);
		int getSymbolStackSize(wcSymbol);

		//identifier tings
		wcSymbol resolveIdent(wcParser*, std::string);
		std::string lexIdent(wcParser*);
		wcSymbol* addIdentDec(wcParser*, wcSymbolType);
		bool isIdentExplicit(std::string);

		//symbol ting
		wcSymbolType getTypeFromDataType(wcParser*, std::string);
		std::string getDatatype(wcParser*, lex::wcToken);

		//ident strings
		std::string getFullIdent(std::string ident, wcSymbol* scope);
		std::string getShortIdent(std::string);

		//params
		std::string getMethodStringSignature(wcParamList*);				//all param dataTypes mashed into a string
		bool checkForOverload(wcParser*, wcParamList* pl, wcSymbol* s);	//check a potential paramlist exists or not (wcSymbol.sigs[pl.StringMethodSignature])

		//expression helpers
		int evalConstExp(wcParser* p_par, tree<wcParseNode>::iterator p_exp);
		int evalConstExp(wcParser* p_par, tree<wcParseNode>::iterator p_exp, std::vector<wcParseNode*> out, std::vector<wcParseNode*> stk);
		int getPrecedence(lex::wcToken);
		int getAssociativity(lex::wcToken);
		int isOperator(lex::wcToken);

		//error
		std::string getErrorCodeAsString(wcErrorCode);
	}
}

#endif //WEEC_PARSER_H
