#pragma once
#include <unordered_map>
#include "error.h" 
#include "lexer.h"
#include "include/tree.hh"

namespace bc
{
	namespace parse
	{
		//symbols are referenced from symtab by their fullidents
		enum bcSymbolType{st_null, st_var, st_type, st_object, st_function, st_namespace};
	
		struct bcSymbol
		{
			std::string ident;		//local identifier 
			std::string fullident;	//fully qualified name $global::varname
			std::string datatype;
			bcSymbolType type;
		};

		enum bcParseNodeType{
							pn_null, pn_head, pn_exp, pn_type,					
							//straight copies of their tt_ counterparts
							pn_string, pn_int, pn_float, pn_object, pn_var, pn_bool, pn_scolon, pn_colon, pn_comma, 
							pn_period, pn_squote, pn_dquote, pn_qmark, pn_exclam, pn_minus, pn_plus,pn_pipe, pn_div, 
							pn_mult, pn_mod, pn_pow, pn_assign, pn_underscore, pn_tilde, pn_oparen, pn_cparen, 
							pn_obracket, pn_cbracket, pn_obrace, pn_cbrace, pn_bslash, pn_percent, pn_newline, pn_dollar, 
							pn_amper, pn_greater, pn_less, pn_greaterequal, pn_lessequal, pn_equal, pn_notequal, 
							pn_logand, pn_logor, pn_lognot,	pn_intlit, pn_strlit, pn_fltlit, pn_ident, pn_comment, 
							pn_varident,pn_funcident,pn_namespaceident,
							pn_dec, pn_true, pn_false, pn_function, pn_incr, pn_decr,pn_plusassign, pn_minusassign, 
							pn_multassign, pn_divassign, pn_assignment,pn_block,pn_vardec,pn_paramdec,pn_funcdec,
							pn_funcdec_type,pn_funcdec_ident,pn_paramlist,pn_decparamlist,pn_funccall, pn_if,pn_else, pn_while,pn_break,
							pn_return, pn_continue,
							pn_statement, pn_negate
							};

		struct bcParseNode
		{	
			bcParseNode(lex::bcTokenType t);
			bcParseNode(bcSymbolType t);
			bcParseNode(bcParseNodeType t){type=t;};
			bcParseNode(bcParseNodeType t,lex::bcToken tk){type=t;tokens.push_back(tk);};
			bcParseNodeType type;
			std::vector<lex::bcToken> tokens;		//index of tokens in bcLexer.data
			std::string tag;					//used to hold internal function names currently TODO find a better way to do this!		
		};
	
		struct bcParamList
		{
			std::unordered_map<std::string,bcSymbol*> params;	//string method signature to entry in symtable to st_type
		};

		struct bcFuncInfo
		{
			std::string ident,fullident,datatype;
			bool isOverloaded;													//has overloaded method signatures
			std::vector<std::string> stackframe;								//fullidents
			std::unordered_map<std::string,tree<bcParseNode>::iterator*> body;	//method signature to parsenode 
			std::unordered_map<std::string,bcParamList>	sigs;					//overloaded string signature to paramlist (inc methodsig)
		};																		//	if(!isOverloaded) sigs[0] is functions signature

		struct bcAST
		{
			tree<bcParseNode>* tree;			
			std::vector<std::vector<std::string>> stackframes;		//working stackframes stack
			std::unordered_map<std::string,bcSymbol>* symtab;		//fullident to bcSymbol
			std::unordered_map<std::string,bcFuncInfo>* functab;	//fullident to funcinfo
//			std::unordered_map<std::string,bcVal> consttab;			//fullident to constant bcVal
		};

		class bcParser
		{
		public:
			void startup(),shutdown();
			
			int parse();			
			void clear();

			//Parse tree
			tree<bcParseNode>::iterator* getNode();				
			tree<bcParseNode>::iterator* addNode(bcParseNode);	//add node and point pindex to the new child node
			tree<bcParseNode>::iterator* addChild(bcParseNode);	//add node, but pindex remains on parent node
			void parent();

			//symbol table
			bcSymbol* getSymbol(std::string);
			bcSymbol* getSymbol(std::string,bcSymbol*);
			bool addSymbol(bcSymbol*);
			bool addSymbol(std::string,bcSymbol*);
		
			//error
			unsigned int getError();

			unsigned int parenCount;
			bool noDecVar,noDecFunc,noDecName;
			bcAST ast;
			bcSymbol* currentScope;
			bcFuncInfo* currentFunc;
			lex::bcLexer* lexer;
			tree<bcParseNode>::iterator pindex;		//Current parse node we are working on	
		};

		//Recursive parsing methods			
		//level 1
		void parseStatement(bcParser*);

		//level 2
		void parseDecFunc(bcParser*);
		void parseDecVar(bcParser*);
		void parseDecNamespace(bcParser*);
		void parseBlock(bcParser*);
		void parseWhile(bcParser*);
		void parseAssignment(bcParser*,bcSymbol);
		void parseFuncCall(bcParser*,bcSymbol);
		void parseFExp(bcParser*);		
		void parseFExp(bcParser*,bcSymbol);
		void parseIf(bcParser*);
		void parseSColon(bcParser*);

		//level 3
		void parseBreak(bcParser*);
		void parseReturn(bcParser*);
		void parseContinue(bcParser*);			
		lex::bcToken	parseExp(bcParser*);

		//level 4
		void parseParamList(bcParser*);	
		void parseDecParamList(bcParser*);			
		bcParseNode parseIdent(bcParser*);		
		lex::bcToken	parseSubExp(bcParser*);

		//level 5		
		void parseDecInParam(bcParser*);
		lex::bcToken	parseTerm(bcParser*);

		//level 6
		lex::bcToken	parseFactor(bcParser*);

		//type compatability
		bool checkOperandTypes(bcParser*,lex::bcToken,lex::bcToken,lex::bcTokenType);
		bool checkOperandInt(bcParser*,lex::bcToken,lex::bcTokenType);
		bool checkOperandFloat(bcParser*,lex::bcToken,lex::bcTokenType);
		bool checkOperandString(bcParser*,lex::bcToken,lex::bcTokenType);
		bool checkOperandBool(bcParser*,lex::bcToken,lex::bcTokenType);
		bool checkOperandObject(bcParser*,lex::bcToken,lex::bcTokenType);
		bool checkOperandVar(bcParser*,lex::bcToken,lex::bcTokenType);
		bool checkOperandFunction(bcParser*,lex::bcToken,lex::bcTokenType);
		bool compareFuncSigs(bcParser*,std::string,std::vector<std::unordered_map<std::string,bcSymbol>::iterator>);

		bcParseNodeType DeriveType(lex::bcToken);
		//identifier tings
		bcSymbol resolveIdent(bcParser*,std::string);
		std::string consumeIdent(bcParser*);
		std::string getFullIdent(bcParser* par,std::string ident,bcSymbol* scope);
		std::string getShortIdent(std::string);
		bcSymbol* addDecIdent(bcParser*,bcSymbolType);
		bool isIdentExplicit(bcParser*,std::string);
		std::string getStringSignature(bcParamList*);
	}
}