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

	//while there are tokens left and no parse errors
	while(tokenIndex.isValid() && !data.output.error)
		switch (tokens.get(tokenIndex).type)
		{
		CASE_BASIC_TYPES_TT
			data.output += subs.dec.parse(data);
			break;

		case tt_ident:
			identSym = tryParseKnownIdent(params, true, nullptr);
			if (identSym != nullptr)
				switch (identSym->type)
				{
				case st_function:
				case st_var:
					if (onlyAllowDeclarations || (!allowAnyDeclarations))
						return setErrorReturn0(params.error, wcError(ec_par_illegalstatement, params.index.getToken()));
					if (!parseExpression(params))
						return 0;
					if (!parseSColon(params))
						return 0;
					break;
				case st_type:
					parseDec(params);
					break;
				default:
					break;
				}
			else
				return wcParserOutput();			

		CASE_ALL_LITERALS_TT
			data.output += subs.exp.parse(data);
			data.output += subs.scolon.parse(data);
			break;

		case tt_keyword_if:
			data.output += subs.conditional.parse(data);
			break;

		case tt_keyword_namespace:
			data.output += subs.ns.parse(data);
			break;

		case tt_keyword_return:
			data.output += subs.ret.parse(data);
			break;

		case tt_keyword_while:
			data.output += subs.whi.parse(data);
			break;

		default:
			//unexpected token
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

wcParserOutput wc::parse::wcParserOutput::operator+(wcParserOutput _output)
{
	if (_output.error)
		error = _output.error;

	ast = ast + _output.ast;
	return *this;
}

wcParserOutput wc::parse::wcParserOutput::operator+=(wcParserOutput _output)
{
	if(_output.error)
		error = _output.error;

	ast += _output.ast;
	return *this;
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

	//while there are tokens left and no parse errors
	bool punctuationLexedLast = false;
	while (tokenIndex.isValid())
	{
		switch (tokens.get(tokenIndex).type)
		{
		case tt_dcolon:
		case tt_period:
			punctuationLexedLast = true;
			break;

		case tt_ident:
			punctuationLexedLast = false;
			break;

		default:
			punctuationLexedLast = false;
			if (!wcTokenTypeDeriver().isDelim(tokens.get(tokenIndex).type))
				if (wcTokenTypeDeriver().isPunctuation(tokens.get(tokenIndex).type))
					punctuationLexedLast = true;
			break;
		}
		ident.fullIdentifier += tokens.get(tokenIndex++).data;
	}	

	//if we get here, end of stream was reached earlier than expected
	return wcParserOutput();
}
