#ifndef BC_PARSER_H
#define BC_PARSER_H

#include <unordered_map>
#include "error.h" 
#include "lexer.h"
#include "include/tree.hh"

namespace bc
{
	namespace vm
	{
		enum bcValType
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

		struct bcVal
		{
			bcValType type;
			int val;	
		};
	}
		
	namespace parse
	{
		//symbols are referenced from symTab by their fullIdents
		enum bcSymbolType{
			st_null, st_namespace, st_var, st_type, st_utype, st_object, st_function	};
	
		struct bcSymbol
		{
			bcSymbol();
			std::string ident;		//local identifier varname
			std::string fullIdent;	//fully qualified name $global::varname
			std::string dataType;	//string varname;
			bcSymbolType type;		//pointer to type symbol
			unsigned int size;		//number of sequential copies in memory
			int	offset;				//filled in by gen, local stack offset
			bool	isConst,		//constant data, read only
					isArray,		//simple c array []
					isStatic;		//static variable, the same value across many stackFrames
		};

		enum bcParseNodeType{
							pn_null, pn_head, pn_exp, pn_type,					
							//straight copies of their tt_ counterparts
							pn_string, pn_int, pn_float, pn_object, pn_var, pn_bool, pn_scolon, pn_colon, pn_comma, 
							pn_period, pn_squote, pn_dquote, pn_qmark, pn_exclam, pn_minus, pn_plus,pn_pipe, pn_div, 
							pn_mult, pn_mod, pn_expo, pn_assign, pn_underscore, pn_tilde, pn_oparen, pn_cparen, 
							pn_obracket, pn_cbracket, pn_obrace, pn_cbrace, pn_bslash, pn_percent, pn_newline, pn_dollar, 
							pn_amper, pn_greater, pn_less, pn_greaterequal, pn_lessequal, pn_equal, pn_notequal, 
							pn_logand, pn_logor, pn_lognot,	pn_intlit, pn_strlit, pn_fltlit, pn_ident, pn_comment, 
							pn_arrayindex, pn_varident,pn_funcident,pn_namespaceident,
							pn_dec, pn_true, pn_false, pn_function, pn_incr, pn_decr,pn_plusassign, pn_minusassign, 
							pn_multassign, pn_divassign, pn_assignment,pn_block,pn_vardec,pn_paramdec,pn_funcdec, pn_namespacedec,
							pn_paramlist,pn_decparamlist,pn_funccall, pn_if,pn_else,pn_if_trueblock, pn_if_elseblock, pn_while,pn_break,
							pn_return, pn_continue,
							pn_statement, pn_negate
							};

		struct bcParseNode
		{	
			bcParseNode();bcParseNode(lex::bcTokenType t);
			bcParseNode(bcSymbolType t);bcParseNode(bcParseNodeType t){type=t;};
			bcParseNode(bcParseNodeType t,lex::bcToken tk){type=t;tokens.push_back(tk);};
			bcParseNodeType type;
			std::vector<lex::bcToken> tokens;		//index of tokens in bcLexer.data
		};

		struct bcExpression
		{
			bcExpression();
			std::string dataType,rpn;	//dataTypes fullIdent,reverse polish notation (postfix) conversion
			tree<bcParseNode>::iterator node;
			bool	isConst,	//all operands are const
					isBool,		//leftmost operand is bool
					isFunc,		//leftmost operand is a function
					isArray,	//leftmost operand is an array
					isAssign;	//operator is tt_assign
		};
	
		struct bcParamList
		{
			std::vector<std::string> params;	//fullIdent of type
		};

		struct bcFuncInfo
		{
			std::string ident,fullIdent,dataType;
			bool isOverloaded;														//has overloaded method signatures
			int gOffset,lOffset;													//global offset, and local offset for local vars
			std::unordered_map<std::string,std::vector<std::string>> stackFrames;	//method signature to a vector of symbol fullIdents
			std::unordered_map<std::string,tree<bcParseNode>::iterator> body;		//method signature to parsenode 
			std::unordered_map<std::string,bcParamList>	sigs;						//method signature to paramlist (inc methodsig)
		};																			//	if(!isOverloaded) sigs[0] is functions signature

		struct bcAST
		{
			tree<bcParseNode>* tree;			
			std::vector<std::vector<std::string>> stackFrames;		//working stackFrames stack
			std::unordered_map<std::string,bcSymbol>* symTab;		//fullIdent to bcSymbol
			std::unordered_map<std::string,bcFuncInfo>* funcTab;	//fullIdent to funcinfo
			std::unordered_map<std::string,vm::bcVal> constTab;		//fullIdent to constant bcVal
		};

		class bcParser
		{
		public:
			void startup(),shutdown();
			int parse();			
			void clear();

			//Parse tree
			tree<bcParseNode>::iterator getNode();				
			tree<bcParseNode>::iterator prevNode();				
			tree<bcParseNode>::iterator addNode(bcParseNode);	//add node and point pindex to the new child node
			tree<bcParseNode>::iterator addChild(bcParseNode);	//add node, but pindex remains on parent node
			void parent();

			//symbol table
			bcSymbol* getSymbol(std::string);
			bcSymbol* getSymbol(std::string,bcSymbol*);
			bool addSymbol(bcSymbol*);
			bool addSymbol(std::string,bcSymbol*);
		
			//error
			unsigned int getError();
			void setError(bcErrorCode,std::string);		//line and column from lexer->getToken()
			void setError(bcErrorCode,int,int,std::string);

			//data
			bcAST ast;
			unsigned int parenCount;
			int sOffset;
			bool noDecVar,noDecFunc,noDecName;
			tree<bcParseNode>::iterator lastIdent;
			bcErrorCode error;
			int errorL,errorC;
			std::string errorS;
			bcSymbol* currentScope;
			bcFuncInfo* currentFunc;
			bcParamList* currentParamList;
			lex::bcLexer* lexer;
			tree<bcParseNode>::iterator pindex;		//Current parse node we are working on	
		};

		//Recursive parsing methods			
		//level 1
		void parseStatement(bcParser*);
		void parseBlock(bcParser*);

		//level 2
		void parseDecFunc(bcParser*);
		void parseDecVar(bcParser*);
		void parseDecNamespace(bcParser*);
		void parseIf(bcParser*);
		void parseWhile(bcParser*);
		void parseAssignment(bcParser*,bcParseNode);
		void parseFuncCall(bcParser*,bcParseNode);
		bcExpression parseFExp(bcParser*);		
		bcExpression parseFExp(bcParser*,bcParseNode);

		//level 3
		bcParseNode parseIdent(bcParser*);
		void parseArrayIndex(bcParser*,bcParseNode*);
		void parseType(bcParser*);
		void parseParamListCall(bcParser*,bcSymbol*);	
		void parseParamListDec(bcParser*);	
		void parseSColon(bcParser*);
		void parseBreak(bcParser*);
		void parseReturn(bcParser*);
		void parseContinue(bcParser*);
		void parseDecInParam(bcParser*);			
		bcExpression	parseExp(bcParser*);
		lex::bcToken	parseSubExp(bcParser*,bcExpression*);
		lex::bcToken	parseTerm(bcParser*,bcExpression*);
		lex::bcToken	parseFactor(bcParser*,bcExpression*);

		//type compatability
		bool checkOperandTypes(bcParser*,lex::bcToken oper1,lex::bcToken op,lex::bcToken oper2);
		bcParseNodeType DeriveType(lex::bcToken);
		//int getTypeSize(bcSymbol*);
		int getTypeSize(lex::bcToken);


		//identifier tings
		bcSymbol resolveIdent(bcParser*,std::string);
		std::string consumeIdent(bcParser*);
		bcSymbol* addIdentDec(bcParser*,bcSymbolType);
		bool isIdentExplicit(bcParser*,std::string);
		
		//symbol ting
		std::string getDatatype(bcParser*,lex::bcToken);
		
		//ident strings
		std::string getFullIdent(bcParser* par,std::string ident,bcSymbol* scope);
		std::string getShortIdent(std::string);
		
		//params
		std::string getMethodStringSignature(bcParamList*);				//all param dataTypes mashed into a string
		bool checkForOverload(bcParser*,bcParamList* pl,bcSymbol* s);	//check a potential paramlist exists or not (bcSymbol.sigs[pl.StringMethodSignature])
		
		//expression helpers
		int evalConstExp(bcParser*,bcExpression);
		int getPrecedence(lex::bcToken);
		int getAssociativity(lex::bcToken);
		int isOperator(lex::bcToken);
	}
}

#endif