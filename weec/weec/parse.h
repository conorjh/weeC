#ifndef WC_PARSE_H
#define WC_PARSE_H
#include <vector>
#include "tree.hh"
#include "error.h"
#include "lex.h"

#define CASE_BASIC_TYPES_TT case tt_bool: case tt_int: case tt_float: case tt_string: case tt_chr: case tt_object: case tt_var:
#define CASE_ALL_LITERALS_TT case tt_intlit: case tt_strlit: case tt_fltlit: case tt_true: case tt_false:
#define CASE_ALL_LITERALS_PN case pn_intlit: case pn_strlit: case pn_fltlit: case pn_true: case pn_false:
#define CASE_ALL_ARITHMETIC_TT case tt_assign: case tt_mult: case tt_div: case tt_plus: case tt_minus:
#define CASE_ALL_OPERATORS_TT case tt_assign: case tt_logor: case tt_logand: case tt_equal: case tt_notequal: case tt_greater:case tt_less: case tt_lessequal: case tt_greaterequal: case tt_mult: case tt_div: case tt_plus: case tt_minus:	case tt_lognot:
#define CASE_ALL_BOOLEAN_OPERATORS_TT case tt_less: case tt_greater: case tt_lessequal: case tt_greaterequal: case tt_equal: case tt_notequal: case tt_logand: case tt_logor:
#define CASE_ALL_OPERATORS_PN case pn_assign: case pn_logor: case pn_logand: case pn_equal: case pn_notequal: case pn_greater:case pn_less: case pn_lessequal: case pn_greaterequal: case pn_mult: case pn_div: case pn_plus: case pn_minus:	case pn_lognot:
#define CASE_ALL_WS_TT case tt_ws: case tt_tab: case tt_newline:

namespace wc
{
	namespace parse
	{
		enum wcSymbolType
		{
			st_null, st_namespace, st_var, st_type, st_utype, st_object, st_function
		};

		struct wcSymbol
		{
			wcSymbol();
			wcSymbol(lex::wcTokenType);
			wcSymbol(std::string);
			wcSymbol(std::string, std::string);
			wcSymbol(wcSymbolType type, std::string ident, std::string fullIdent, bool isNamespace, 
				bool isArray, bool isConst, bool isStatic, unsigned int size, unsigned int dataSize, int stackOffset, wcSymbol* dataType);

			std::string ident, fullyQualifiedIdent;
			bool isNamespace, isArray, isConst, isStatic;
			wcSymbolType type;
			unsigned int size, dataSize;
			int stackOffset;
			wcSymbol* dataType;
		};

		struct wcSymbolTable
		{
			wcSymbolTable();

			wcSymbol* getSymbol(std::string p_fullyQualifiedIdent);
			wcSymbol* getSymbol(std::string p_fullyQualifiedScopeIdent, std::string p_ident);
			wcSymbol* getSymbol(wcSymbol* p_scope, std::string p_ident);
			wcSymbol* getSymbolFromShortIdent(std::string p_ident, std::vector<wcSymbol> p_openScopes);

			int addSymbol(wcSymbol p_sym);
			bool isSymbolInScope(wcSymbol* p_scope, wcSymbol* p_symbol);
			std::unordered_map<std::string, wcSymbol> ident2Symbol;
		};

		enum wcParseNodeType
		{
			pn_null, pn_head, pn_exp, pn_type, pn_statement,

			//straight copies of their tt_ counterparts
			pn_str, pn_int, pn_flt, pn_obj, pn_var, pn_bool, pn_scolon, pn_colon, pn_comma,
			pn_period, pn_squote, pn_dquote, pn_qmark, pn_exclam, pn_minus, pn_plus, pn_pipe, pn_div,
			pn_mult, pn_mod, pn_expo, pn_assign, pn_underscore, pn_tilde, pn_oparen, pn_cparen,
			pn_obracket, pn_cbracket, pn_obrace, pn_cbrace, pn_bslash, pn_percent, pn_newline, pn_dollar,
			pn_amper, pn_greater, pn_less, pn_greaterequal, pn_lessequal, pn_equal, pn_notequal,
			pn_logand, pn_logor, pn_lognot, pn_intlit, pn_strlit, pn_fltlit, pn_ident, pn_comment,
			pn_arrayindex, pn_varident, pn_funcident, pn_namespaceident,
			pn_dec, pn_true, pn_false, pn_function, pn_incr, pn_decr, pn_plusassign, pn_minusassign,
			pn_multassign, pn_divassign, pn_assignment, pn_body, pn_decvar, pn_paramdec, pn_funcdec, pn_namespacedec,
			pn_paramlist, pn_decparamlist, pn_funccall, pn_if, pn_else, pn_if_trueblock, pn_if_elseblock, pn_while, pn_break,
			pn_return, pn_continue,
			 pn_negate
		};
		const std::unordered_multimap<wcParseNodeType, const char *> pnTypeStrings = 
		{
			{	pn_null, "pn_null"	},	{ pn_head, "pn_head"	},		{	pn_exp,	"pn_exp" },
			{ pn_type, "pn_type" },	{ pn_statement,	"pn_statement" },{ pn_str,"pn_str" },
			{ pn_int,"pn_int" }, { pn_flt,"pn_flt"},			{ pn_obj,"pn_obj"},
			{ pn_var,"pn_var"},	{ pn_bool, "pn_bool"},			{ pn_scolon,"pn_scolon"},
			{ pn_colon,"pn_colon"},				{ pn_comma,"pn_comma"},			{ pn_period,"pn_period"},
			{ pn_squote, "pn_squote"},			{ pn_dquote,"pn_dquote"},			{ pn_qmark,"pn_qmark"},
			{ pn_exclam, "pn_exclam"},			{ pn_minus, "pn_minus"},			{ pn_plus,"pn_plus"},
			{ pn_pipe,"pn_pipe"},				{ pn_div,"pn_div"},			{ pn_mult,"pn_mult"},
			{ pn_mod, "pn_mod"},				{ pn_expo ,"pn_expo" },			{ pn_assign ,"pn_assign" },
			{ pn_underscore ,"pn_underscore" },	{ pn_tilde ,"pn_tilde"  },			{ pn_oparen,"pn_oparen" },
			{ pn_cparen,"pn_cparen" },			{ pn_obracket, "pn_obracket" },			{pn_cbracket,"pn_cbracket"},
			{ pn_obrace, "pn_cbracket"},		{ pn_cbrace, "pn_cbrace"}, 			{ pn_bslash,"pn_bslash" },
			{ pn_percent,"pn_percent" },		{ pn_newline,"pn_newline" },			{ pn_dollar ,"pn_dollar" },
			{ pn_amper ,"pn_amper" },			{ pn_greater ,"pn_greater" },			{ pn_less ,"pn_less" },
			{ pn_greaterequal ,"pn_greaterequal" },	{ pn_lessequal ,"pn_lessequal" },			{ pn_equal ,"pn_equal" },
			{ pn_notequal ,"pn_notequal" },		{ pn_logand, "pn_logand"},			{ pn_logor ,"pn_logor" },
			{ pn_lognot ,"pn_lognot" },			{ pn_intlit ,"pn_intlit" },			{ pn_strlit ,"pn_strlit" },
			{ pn_fltlit ,"pn_fltlit" },			{ pn_ident ,"pn_ident" },			{ pn_comment ,"pn_comment" },
			{ pn_arrayindex ,"pn_arrayindex" },	{ pn_varident ,"pn_varident" },			{ pn_funcident ,"pn_funcident" },
			{ pn_namespaceident ,"pn_namespaceident" },			{ pn_dec ,"pn_dec" },			{ pn_true ,"pn_true" },
			{ pn_false ,"pn_false" },			{ pn_function ,"pn_function" },			{ pn_incr ,"pn_incr" },
			{ pn_decr ,"pn_decr" },				{ pn_plusassign ,"pn_plusassign" },			{ pn_minusassign ,"pn_minusassign" },
			{ pn_multassign ,"pn_multassign" },	{ pn_divassign ,"pn_divassign" },			{ pn_assignment ,"pn_assignment" },
			{ pn_body ,"pn_body" },			{ pn_decvar ,"pn_decvar" },			{ pn_paramdec ,"pn_paramdec" },
			{ pn_funcdec ,"pn_funcdec" },		{ pn_namespacedec ,"pn_namespacedec" },			{ pn_paramlist ,"pn_paramlist" },
			{ pn_decparamlist ,"pn_decparamlist" },			{ pn_funccall ,"pn_funccall" },			{ pn_if ,"pn_if" },
			{ pn_else ,"pn_else" },				{ pn_if_trueblock ,"pn_if_trueblock" },			{ pn_if_elseblock ,"pn_if_elseblock" },
			{ pn_while ,"pn_while" },			{ pn_break ,"pn_break" },			{ pn_return ,"pn_return" },
			{ pn_continue ,"pn_continue" },		{ pn_negate ,"pn_negate" }
		};

		struct wcParseNode
		{
			wcParseNode();
			wcParseNode(wcParseNodeType);
			wcParseNode(lex::wcToken);
			wcParseNode(wcParseNodeType, lex::wcToken);
			wcParseNode(wcParseNodeType, lex::wcToken, lex::wcToken);
			wcParseNode(wcSymbol, std::string);
			std::vector<lex::wcToken> tokens;
			wcParseNodeType type;
			tree<wcParseNode>::iterator parent;
		};

		struct wcParseIndex
		{
			bool isLexIndexValid();
			bool isLexIndexValid(int);
			void setTokens(std::vector<lex::wcToken>*);
			lex::wcToken getToken();
			lex::wcToken getToken(int);
			lex::wcToken nextToken();
			lex::wcToken nextToken(lex::wcTokenType p_expectedType, wcError& p_error);
			lex::wcToken decToken();

			void setNode(tree<wcParseNode>::iterator);
			void setNode(tree<wcParseNode>*, tree<wcParseNode>::iterator);
			tree<wcParseNode>::iterator getNode();
			tree<wcParseNode>::iterator nextNode();
			tree<wcParseNode>::iterator prevNode();
			tree<wcParseNode>::iterator backToParent();
			int getCurrentNodeDepth();
			int getNodeDepth(tree<wcParseNode>::iterator);
			tree<wcParseNode>* getTree();
		private:
			tree<wcParseNode>* parseTree;
			tree<wcParseNode>::iterator node;
			std::vector<lex::wcToken>* tokens;
			int tokenIndex;
		};

		struct wcParameter
		{
			wcParameter(wcSymbol*);
			wcSymbol* tableEntry;	//link to symbol table entry
		};

		struct wcParamList
		{
			unsigned int paramCount() { return params.size(); }
			std::vector<wcParameter> params;
		};

		struct wcFunctionInfo
		{
			wcFunctionInfo() {};
			wcFunctionInfo(std::string p_name, wcParamList p_params, wcSymbol* p_sym, tree<wcParseNode>::iterator p_it)
			{	internalFuncName = p_name; paramList = p_params, symbol = p_sym, bodyNode = p_it;	}

			std::string internalFuncName;
			wcParamList paramList;
			wcSymbol* symbol;
			tree<wcParseNode>::iterator bodyNode;
		};

		struct wcFunctionTable
		{
			wcFunctionTable();

			int addSymbol(std::string internalFunctionName, wcFunctionInfo p_sym);

			std::unordered_map<std::string, wcFunctionInfo> ident2Funcinfo;
		};

		struct wcStackframe
		{
			wcStackframe(wcSymbol* p_owner) { owner = p_owner; }
			wcSymbol* owner;
			wcFunctionInfo* thisFrame;
			std::vector<wcSymbol*> locals;
		};

		struct wcExpression
		{
			bool isError,isConst;
		};

		struct wcAST
		{
			wcAST();
			wcAST(tree<wcParseNode>);
			tree<wcParseNode>::iterator addNode(wcParseIndex&, wcParseNode);	//add node and point pindex to the new child node
			tree<wcParseNode>::iterator addChild(wcParseIndex&, wcParseNode);	//add node, but pindex remains on parent node

			tree<wcParseNode> parseTree;
			std::unordered_map<std::string, wcStackframe> frames;
			wcSymbolTable symTab;
			wcFunctionTable funcTab;
		};

		struct wcParseData
		{
			wcParseData();
			wcParseData(int);
			wcSymbol* currentScope, currentStackFrame;
			std::vector<wcStackframe> stackframes;
			int parenCount, currentStackIndex;
		};

		class wcParser
		{
		public:
			wcParser();

			virtual wcAST parse(std::vector<lex::wcToken>);

			bool isError();
			wcError getError();
			void setError(wcError);

		private:
			void init();
			wcError error;
			wcParseIndex index;
			wcParseData data;
		};

		struct wcParseParams 
		{
			wcParseParams(wcParseIndex&, wcAST&, wcError&, wcParseData&);
			wcParseIndex& pIndex;
			wcAST& pOutput;
			wcError& pError;
			wcParseData& pData;
			friend class wcParser;
		};

		int getPrecedence(lex::wcToken);
		bool isRightAssociative(lex::wcToken);

	}
}
#endif