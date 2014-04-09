#pragma once
#include <unordered_map>
#include "lexer.h"
#include "include/tree.hh"

namespace bc
{
	namespace parse
	{
		enum bcParseNodeType{
							pn_null, pn_head, pn_exp, pn_type,					
							//straight copies of their tt_ counterparts
							pn_string, pn_int, pn_float, pn_object, pn_var, pn_bool, pn_scolon, pn_colon, pn_comma, 
							pn_period, pn_squote, pn_dquote, pn_qmark, pn_exclam, pn_minus, pn_plus,pn_pipe, pn_div, 
							pn_mult, pn_mod, pn_pow, pn_assign, pn_underscore, pn_tilde, pn_oparen, pn_cparen, 
							pn_obracket, pn_cbracket, pn_obrace, pn_cbrace, pn_bslash, pn_percent, pn_newline, pn_dollar, 
							pn_amper, pn_greater, pn_less, pn_greaterequal, pn_lessequal, pn_equal, pn_notequal, 
							pn_logand, pn_logor, pn_lognot,	pn_intlit, pn_strlit, pn_fltlit, pn_ident, pn_comment, 
							pn_dec, pn_true, pn_false, pn_function, pn_incr, pn_decr,pn_plusassign, pn_minusassign, 
							pn_multassign, pn_divassign, pn_assignment,pn_block,pn_vardec,pn_paramdec,pn_funcdec,
							pn_funcdec_type,pn_funcdec_ident,pn_paramlist,pn_decparamlist,pn_funccall, pn_if,pn_else, pn_while,pn_break,
							pn_return, pn_continue,
							pn_statement
							};

		struct bcParseNode
		{			
			bcParseNode(bcParseNodeType t){type=t;};
			bcParseNode(bcParseNodeType t,lex::bcToken tk){type=t;tokens.push_back(tk);};
			bcParseNodeType type;
			std::vector<lex::bcToken> tokens;		//index of tokens in bcLexer.data
			std::string tag;					//used to hold internal function names currently TODO find a better way to do this!		
		};
	
		enum bcSymbolType{st_null, st_var, st_type, st_object, st_function, st_namespace};

		//symbols are referenced from symtab by their fullidents
		struct bcSymbol
		{
			std::string ident;		//variable name
			std::string fullident;	//full name $global::varname
			bcSymbolType type;
		};

		struct bcAST
		{
			tree<bcParseNode>* tree;
			std::unordered_map<std::string,bcSymbol>* symtab;	
			//std::unordered_map<std::string,std::vector<std::unordered_map<std::string,bcSymbol>::iterator>>* funcsig;
			//std::unordered_map<std::string,std::vector<std::unordered_map<std::string,bcSymbol>::iterator>>* stackframes;
		};

		class bcParser
		{
		public:
			void startup(),shutdown();
			
			int parse();			

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
		
			bcAST ast;
			bcSymbol* currentScope;
			lex::bcLexer* lexer;		
			tree<bcParseNode>::iterator pindex;		//Current parse node we are working on	
		};

		//Recursive parsing methods			
		//level 1
		void parseStatement(bcParser*);

		//level 2
		void parseBlock(bcParser*);
		void parseBlockNoDec(bcParser*);
		void parseWhile(bcParser*);
		void parseAssignment(bcParser*);
		void parseDec(bcParser*);
		void parseFuncCall(bcParser*);
		void parseFExp(bcParser*);
		void parseIf(bcParser*);
		void parseSColon(bcParser*);

		//level 3
		void parseDecFunc(bcParser*);
		void parseDecVar(bcParser*);
		void parseBreak(bcParser*);
		void parseReturn(bcParser*);
		void parseContinue(bcParser*);			
		lex::bcToken	parseExp(bcParser*);

		//level 4
		void parseParamList(bcParser*);	
		void parseDecParamList(bcParser*);			
		void parseDecFunc_Type(bcParser*);
		void parseDecFunc_Ident(bcParser*);		
		bcSymbol parseIdent(bcParser*);
		void parseIdent_Namespace(bcParser*);
		void parseIdent_Var(bcParser*);
		void parseIdent_Function(bcParser*);
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
		std::string GetTag(bcParser*,bcSymbol*);

		bcParseNodeType DeriveType(lex::bcToken);
		bcSymbol resolveIdent(bcParser*,std::string);
		std::string getFullIdent(std::string ident,bcSymbol* scope);
	}
}