#include "parse.h"

using namespace wc;
using namespace wc::lex;
using namespace wc::parse;
using namespace wc::error;
using namespace std;

int wc::parse::getPrecedence(wcToken p_token)
{
	switch (p_token.type)
	{
	case tt_assign:
		return 1;
	case tt_logor:
		return 2;
	case tt_logand:
		return 3;
	case tt_equal:	case tt_notequal:
		return 4;
	case tt_greater:	case tt_less:	case tt_lessequal:	case tt_greaterequal:
		return 5;
	case tt_mod:	case tt_expo:
		return 6;
	case tt_plus:	case tt_minus:
		return 7;
	case tt_mult:	case tt_div:
		return 8;
	case tt_lognot:
		return 9;
	case tt_cparen:
		return 10;
	default:
		return -1;
	}
}

bool wc::parse::isRightAssociative(wcToken token)
{
	switch (token.type)
	{
	case tt_mod:
	case tt_expo:
		return true;
	}
	return false;
}

wc::parse::wcParseNode::wcParseNode()
{

}

wc::parse::wcAST::wcAST()
{
	parseTree = tree<wcParseNode>(wcParseNode(pn_head));
}

wc::parse::wcAST::wcAST(tree<wcParseNode> p_tree)
{
	parseTree = tree<wcParseNode>(wcParseNode(pn_head));
	parseTree = p_tree;
}

tree<wcParseNode>::iterator wc::parse::wcAST::addNode(wcASTIndex& index, wcParseNode node)
{
	//append a child to the current node, then set the index to point at that new addition
	index.set(parseTree.append_child(index.get(), node));

	//return the new addition
	return index.get();
}

tree<wcParseNode>::iterator wc::parse::wcAST::addChild(wcASTIndex& index, wcParseNode node)
{
	//append a child to the current node, dont set the index to it though
	parseTree.append_child(index.get(), node);

	//then return the new addition
	return parseTree.child(index.get(), parseTree.number_of_children(index.get()) - 1);
}

wc::parse::wcParser::wcParser()
{

}

wcParserOutput wc::parse::wcParser::parse(lex::wcTokenStream &_tokens)
{
	//create our stream indexes, and space for output data, and a handy alias (tokens)
	wcParseData data = wcParseData(_tokens);
	wcTokenStream& tokens = data.index.input;
	wcTokenStreamIndex& tokenIndex = data.index.tokenIndex;
	wcParserSymbolTable& symTab = data.output.symTab;

	wcIdent ident;
	//while there are tokens left and no parse errors
	while (tokenIndex.isValid() && !data.output.error)
		switch (tokens.get(tokenIndex).type)
		{
		//identifier - either dec or statement
		case tt_ident:
			subs.id.parse(data, ident);
			if (symTab.exists(ident))
				switch (symTab.find(ident).type)
				{
				case st_var:
				case st_func:
					goto _wcParser_parse_statements;
				case st_type:
					goto _wcParser_parse_declarations;
				default:
					return wcParserOutput(wcError(ec_par_unexpectedtoken, wcToken(tt_ident, ident.fullIdentifier, ident.line, ident.column)));
				}
			else
				return wcParserOutput(wcError(ec_par_undeclaredident, wcToken(tt_ident, ident.fullIdentifier, ident.line, ident.column)));

		//declarations
		_wcParser_parse_declarations:
		CASE_BASIC_TYPES_TT
			data.output += subs.dec.parse(data);
			break;

		//statements
		_wcParser_parse_statements:
		CASE_ALL_LITERALS_TT
		case tt_keyword_return:
		case tt_keyword_namespace:
		case tt_keyword_if:
		case tt_keyword_while:
			data.output += subs.statement.parse(data);
			break;

		//unexpected token
		default:
			return wcParserOutput();
		}

	if (data.output.error)
		return wcParserOutput();
	return data.output;
}

wc::parse::wcParserIndex::wcParserIndex(lex::wcTokenStream &_input) : input(_input), ast(*new wcAST()), astIndex(ast), tokenIndex(input)
{

}

wc::parse::wcParserIndex::wcParserIndex(lex::wcTokenStream &_input, wcAST &_ast) : input(_input), ast(_ast), astIndex(ast), tokenIndex(input)
{

}

wc::parse::wcParseData::wcParseData(lex::wcTokenStream &tokens) : index(tokens), output()
{

}

wc::parse::wcParserOutput::wcParserOutput()
{

}

wc::parse::wcParserOutput::wcParserOutput(error::wcError _error)
{
	error = _error;
}

wcParserOutput wc::parse::wcParserOutput::operator+(wcParserOutput _output)
{
	if (_output.error)
		error = _output.error;

	ast = ast + _output.ast;
	return *this;
}

wcParserOutput wc::parse::wcParserOutput::operator+=(wcParserOutput _output)
{
	if (_output.error)
		error = _output.error;

	ast += _output.ast;
	return *this;
}

wcParserOutput wc::parse::wcStatementParser::parse(wcParseData &data)
{
	//create our stream indexes, and space for output data, and a handy alias (tokens)
	wcTokenStream& tokens = data.index.input;
	wcTokenStreamIndex& tokenIndex = data.index.tokenIndex;

	//while there are tokens left and no parse errors
	while (tokenIndex.isValid() && !data.output.error)
		switch (tokens.get(tokenIndex).type)
		{
		case tt_ident:
		CASE_ALL_LITERALS_TT
				data.output += subs.exp.parse(data);
			break;

		case tt_keyword_return:
			data.output += subs.ret.parse(data);
			break;

		case tt_keyword_namespace:
			data.output += subs.ns.parse(data);
			break;

		case tt_keyword_if:
			data.output += subs.conditional.parse(data);
			break;

		case tt_keyword_while:
			data.output += subs.wloop.parse(data);
			break;

		default:
			return wcParserOutput();
		}

	if (data.output.error)
		return wcParserOutput();
	return data.output;
}

wcParserOutput wc::parse::wcIdentParser::parse(wcParseData &data)
{
	return parse(data, wcIdent());
}

wcParserOutput wc::parse::wcIdentParser::parse(wcParseData &data, wcIdent &ident)
{
	//handy aliases
	wcTokenStream& tokens = data.index.input;
	wcTokenStreamIndex& tokenIndex = data.index.tokenIndex;

	bool punctuationLexedLast = false, isFirstToken = true;
	wcParserOutput output;

	//while there are no ident or punctuation tokens left 
	while (tokenIndex.isValid())
	{
		switch (tokens.get(tokenIndex).type)
		{
		case tt_dcolon:
		case tt_period:
			if (!punctuationLexedLast && isFirstToken)
				return wcParserOutput(wcError(ec_par_malformedident, tokens.get(tokenIndex)));	//opening token is not an identifier
			punctuationLexedLast = true;
			break;

		case tt_ident:
			punctuationLexedLast = false;
			break;

		default:
			if (isFirstToken)
				return wcParserOutput(wcError(ec_par_malformedident, tokens.get(tokenIndex)));	//opening token is not an identifier
			else if (punctuationLexedLast)
				return wcParserOutput(wcError(ec_par_malformedident, wcToken(tt_ident, ident.fullIdentifier, ident.line, ident.column)));	//malformed

			output.ast.addChild(data.index.astIndex, wcParseNode(pn_ident, { wcToken(tt_ident, ident.fullIdentifier, ident.line, ident.column) }));
			return output;
		}

		if (isFirstToken)
		{
			//save the line/column data for final token
			ident.line = tokens.get(tokenIndex).line;
			ident.column = tokens.get(tokenIndex).column;
			isFirstToken = false;
		}
		ident.fullIdentifier += tokens.get(tokenIndex++).data;
	}

	//if were here, input index is no longer valid, could be an error
	if (isFirstToken)
		return wcParserOutput();
	else if (punctuationLexedLast)
		return wcParserOutput();
	else
		return output;
}
