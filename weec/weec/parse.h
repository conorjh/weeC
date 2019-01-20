#ifndef WC_PARSE_H
#define WC_PARSE_H
#include <vector>
#include "tree.hh"
#include "error.h"
#include "lex.h"

#define CASE_BASIC_TYPES_TT case tt_keyword_bool: case tt_keyword_int: case tt_keyword_float: case tt_keyword_string: case tt_keyword_char:
#define CASE_ALL_LITERALS_TT case tt_intlit: case tt_strlit: case tt_fltlit: case tt_keyword_true: case tt_keyword_false:
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
			pn_return, pn_continue, pn_negate
		};

		const std::unordered_multimap<wcParseNodeType, const char *> pnTypeStrings =
		{
			{	pn_null, "pn_null"	},			{ pn_head, "pn_head"	},		{	pn_exp,	"pn_exp" },
			{ pn_type, "pn_type" },				{ pn_statement,	"pn_statement" },{ pn_str,"pn_str" },
			{ pn_int,"pn_int" },				{ pn_flt,"pn_flt"},			{ pn_obj,"pn_obj"},
			{ pn_var,"pn_var"},					{ pn_bool, "pn_bool"},			{ pn_scolon,"pn_scolon"},
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
			wcParseNode(wcParseNodeType, std::vector<lex::wcToken>);

			std::vector<lex::wcToken> tokens;
			wcParseNodeType type;
			tree<wcParseNode>::iterator parent;
		};

		struct wcAST;
		struct wcASTIndex
		{
			wcASTIndex(wcAST&);

			wcASTIndex operator=(wcASTIndex),
				operator=(tree<wcParseNode>::iterator),
				operator--(), operator--(int),
				operator++(), operator++(int),
				operator-(int),
				operator+(int);
			tree<wcParseNode>::iterator operator[](tree<wcParseNode>::iterator);

			tree<wcParseNode>::iterator get(), next(), prev(), up(), set(tree<wcParseNode>::iterator);
			int depth(), depth(tree<wcParseNode>::iterator);

		protected:
			tree<wcParseNode>::iterator index;
			wcAST& ast;
		};

		struct wcParserOutput;
		struct wcAST
		{
			wcAST();
			wcAST(tree<wcParseNode>);

			wcAST operator+=(wcAST),
				operator+(wcAST);

			tree<wcParseNode>::iterator addNode(wcASTIndex&, wcParseNode),	addNode(wcASTIndex&, wcParserOutput),
				addChild(wcASTIndex&, wcParseNode),	addChild(wcASTIndex&, wcParserOutput);	//add node, but pindex remains on parent node
			void removeNode(wcASTIndex&);

			tree<wcParseNode> parseTree;
		};

		struct wcIdent
		{
			wcIdent();
			wcIdent(std::string);

			bool isDeclared() const;
			bool isValid() const;

			int line, column;
			std::string fullIdentifier;
		};

		enum wcParseSymbolType
		{
			st_var, st_func, st_type
		};

		struct wcParseSymbol
		{
			wcParseSymbol();

			wcIdent ident;
			wcParseSymbolType type;
		};

		class wcParserSymbolTable
		{
		public:
			bool exists(wcIdent);
			wcParseSymbol& find(wcIdent);
			wcParseSymbol& reg(wcIdent);

			std::unordered_map<std::string, wcParseSymbol> lookup;
		};

		struct wcParserOutput
		{
			wcParserOutput();
			wcParserOutput(error::wcError);

			wcParserOutput operator+=(wcParserOutput),
				operator+(wcParserOutput);

			tree<wcParseNode>::iterator addNode(wcASTIndex&, wcParserOutput), addNode(wcASTIndex&, wcParseNode),
				addChild(wcASTIndex&, wcParseNode),	addChild(wcASTIndex&, wcParserOutput);	//add node, but pindex remains on parent node

			wcAST ast;
			wcParserSymbolTable symTab;
			error::wcError error;
		};

		//keeps track how far the parser has got into the input stream, and
		//where the parser is within the output tree. 
		struct wcParserIndex
		{
			wcParserIndex(lex::wcTokenStream&);
			wcParserIndex(lex::wcTokenStream&, wcAST&);

			wcAST& ast;
			wcASTIndex astIndex;
			lex::wcTokenStream& input;
			lex::wcTokenStreamIndex tokenIndex;
		};

		struct wcParseData
		{
			wcParseData(lex::wcTokenStream&);
			wcParseData(lex::wcTokenStream&, wcAST&);

			wcParserIndex index;
			wcParserOutput output;
		};

		struct wcParseExpression
		{

		};

		struct wcParseDeclaration
		{
			wcParseDeclaration(wcParseSymbol&);

			wcParseSymbol& type;
			wcIdent identifier;
			wcParseExpression exp;
		};
		class wcSubParser;
		class wcStatementParser;	class wcDeclarationParser; 
		class wcIdentParser;		class wcTypeParser;
		class wcExpressionParser;	class wcIfParser;
		class wcWhileParser;		class wcCodeBlockParser;
		class wcReturnParser;		class wcSColonParser;
		class wcNamespaceParser;
		class wcParserSubParserCollection
		{
		public:
			wcParserSubParserCollection();

			wcSubParser& sub;
			wcStatementParser& statement;
			wcDeclarationParser& dec;

			wcIdentParser& ident;
			wcTypeParser& type;
			wcExpressionParser& exp;

			wcNamespaceParser& ns;
			wcIfParser& conditional;
			wcWhileParser& wloop;
			wcCodeBlockParser& block;
			wcReturnParser& ret;
			wcSColonParser& scolon;
		};

		class wcSubParser
		{
		public:
			wcSubParser();

			wcParserOutput parse(wcParseData&);
			wcParserSubParserCollection subs;
		};

		class wcExpressionParser : wcSubParser
		{
		public:
			wcExpressionParser();
			wcParserOutput parse(wcParseData&);
		};

		class wcNamespaceParser : wcSubParser
		{
		public:
			wcNamespaceParser();
			wcParserOutput parse(wcParseData&);
		};

		class wcIfParser : wcSubParser
		{
		public:
			wcIfParser();
			wcParserOutput parse(wcParseData&);
		};

		class wcWhileParser : wcSubParser
		{
		public:
			wcWhileParser();
			wcParserOutput parse(wcParseData&);
		};

		class wcSemiColonParser : wcSubParser
		{
		public:
			wcSemiColonParser();
			wcParserOutput parse(wcParseData&);
		};

		class wcIdentParser : wcSubParser
		{
		public:
			wcIdentParser();
			wcParserOutput parse(wcParseData&), 
				parse(wcParseData&, wcIdent&);
		};

		class wcStatementParser : wcSubParser
		{
		public:
			wcStatementParser();
			wcParserOutput parse(wcParseData&);
		};

		class wcDeclarationParser : wcSubParser
		{
		public:
			wcDeclarationParser();
			wcParserOutput parse(wcParseData&),
				parse(wcParseData&, wcParseDeclaration&);
		};

		class wcCodeBlockParser : wcSubParser
		{
		public:
			wcCodeBlockParser();
			wcParserOutput parse(wcParseData&);
		};

		class wcTypeParser : wcSubParser
		{
		public:
			wcTypeParser();
			wcParserOutput parse(wcParseData&),
				parse(wcParseData&, wcIdent&);
		};

		class wcReturnParser : wcSubParser
		{
		public:
			wcReturnParser();
			wcParserOutput parse(wcParseData&);
		};

		class wcSColonParser : wcSubParser
		{
		public:
			wcSColonParser();
			wcParserOutput parse(wcParseData&);
		};

		class wcParser
		{
		public:
			wcParser();

			virtual wcParserOutput parse(lex::wcTokenStream&);
			
			virtual wcParserOutput subParse(wcParseData&);
			wcParserSubParserCollection subs;
		};

		int getPrecedence(lex::wcToken);
		bool isRightAssociative(lex::wcToken);
	}
}
#endif